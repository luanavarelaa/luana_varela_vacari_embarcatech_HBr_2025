#include "audio.h"
#include "comuns.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <stdio.h>
#include <math.h> // Para round()

// Buffer interno para armazenar as amostras de áudio
static uint16_t internal_audio_buffer[AUDIO_BUFFER_SIZE];
// Contador para o número de amostras atualmente no buffer
static uint32_t internal_sample_count = 0;
// Variável para armazenar a taxa de amostragem teórica calculada a partir do clkdiv do ADC
static uint32_t theoretical_actual_sample_rate_from_clkdiv = 0;

// Variável estática para o número do slice do PWM
static uint pwm_slice_num_global;


// --- Funções de Inicialização ---

void audio_init(uint32_t target_sample_rate_hz) {
    adc_init(); 
    adc_run(false); 

    adc_gpio_init(MIC_PIN);
    adc_select_input(MIC_CHANNEL);

    uint32_t clk_sys_hz = clock_get_hz(clk_sys);
    float calculated_div_float = (float)clk_sys_hz / (float)(target_sample_rate_hz * 96);
    
    adc_set_clkdiv(calculated_div_float);

    if (calculated_div_float > 0) {
        theoretical_actual_sample_rate_from_clkdiv = (uint32_t)round(((float)clk_sys_hz / calculated_div_float) / 96.0f);
    } else {
        theoretical_actual_sample_rate_from_clkdiv = 0; 
    }
    
    printf("Microfone ADC Inicializado no GP%u (Canal %u)\n", MIC_PIN, MIC_CHANNEL);
    printf("Taxa de Amostragem Alvo: %lu Hz\n", target_sample_rate_hz);
    printf("Divisor de Clock do ADC Calculado (float): %.2f\n", calculated_div_float);
    printf("Taxa de Amostragem Teórica Esperada (baseada no clkdiv): ~%lu Hz\n", theoretical_actual_sample_rate_from_clkdiv);
}

void audio_init_pwm(uint gpio_pin, uint16_t wrap_value) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    pwm_slice_num_global = pwm_gpio_to_slice_num(gpio_pin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f); 
    pwm_config_set_wrap(&config, wrap_value);
    pwm_init(pwm_slice_num_global, &config, true);

    float pwm_freq_khz = (clock_get_hz(clk_sys) / 1.0f / (wrap_value + 1.0f)) / 1000.0f;
    printf("Audio PWM Inicializado no GP%u, Slice %u, Wrap %u (Freq. PWM: ~%.2f kHz)\n", 
           gpio_pin, pwm_slice_num_global, wrap_value, pwm_freq_khz);
}

// --- Funções de Gravação ---

void audio_reset_internal_buffer() {
    internal_sample_count = 0;
}

uint32_t audio_record_to_internal_buffer(uint32_t num_samples_to_record) {
    audio_reset_internal_buffer();

    uint32_t total_time_per_sample_us = 1000000 / TARGET_SAMPLE_RATE_HZ; 
    uint32_t adc_read_overhead_us = 5; 
    uint32_t delay_needed_us = 0;

    if (total_time_per_sample_us > adc_read_overhead_us) {
        delay_needed_us = total_time_per_sample_us - adc_read_overhead_us;
    }

    for (uint32_t i = 0; i < num_samples_to_record; i++) {
        if (internal_sample_count < AUDIO_BUFFER_SIZE) {
            internal_audio_buffer[internal_sample_count] = adc_read();
            internal_sample_count++;

            if (delay_needed_us > 0) {
                sleep_us(delay_needed_us); 
            }
        } else {
            break;
        }
    }
    return internal_sample_count;
}

const uint16_t* audio_get_internal_buffer_ptr() {
    return internal_audio_buffer;
}

uint32_t audio_get_internal_buffer_sample_count() {
    return internal_sample_count;
}

// --- Funções de Reprodução ---

void audio_pwm_set_sample(uint gpio_pin, uint16_t adc_sample) {
    uint16_t pwm_level = adc_sample / 2; 
    
    if (pwm_level > PWM_WRAP_VALUE) { 
        pwm_level = PWM_WRAP_VALUE;
    }
    pwm_set_gpio_level(gpio_pin, pwm_level);
}

void audio_play_buffer_via_pwm(uint gpio_pin) {
    if (internal_sample_count == 0) {
        printf("Buffer de áudio está vazio. Nada para tocar.\n");
        return;
    }

    printf("Tocando %lu amostras via PWM no pino GP%u...\n", internal_sample_count, gpio_pin);

    uint32_t time_per_sample_playback_us = 1000000 / TARGET_SAMPLE_RATE_HZ;

    for (uint32_t i = 0; i < internal_sample_count; i++) {
        audio_pwm_set_sample(gpio_pin, internal_audio_buffer[i]);
        sleep_us(time_per_sample_playback_us); 
    }

    pwm_set_gpio_level(gpio_pin, 0); 
    printf("Reprodução concluída.\n");
}
