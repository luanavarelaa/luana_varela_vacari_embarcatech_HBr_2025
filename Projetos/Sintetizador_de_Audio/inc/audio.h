#ifndef AUDIO_H
#define AUDIO_H

#include "pico/stdlib.h"
#include "comuns.h" // Para definições como AUDIO_BUFFER_SIZE, TARGET_SAMPLE_RATE_HZ

// --- Funções de Inicialização ---
// Inicializa o ADC para o microfone com uma taxa de amostragem alvo
void audio_init(uint32_t target_sample_rate_hz);

// Inicializa o hardware PWM para saída de áudio no pino especificado
void audio_init_pwm(uint gpio_pin, uint16_t wrap_value);

// --- Funções de Gravação ---
// Preenche o buffer de áudio interno com um número especificado de amostras
// Retorna o número de amostras realmente gravadas.
uint32_t audio_record_to_internal_buffer(uint32_t num_samples_to_record);

// Retorna um ponteiro (const) para o buffer de áudio interno (apenas para leitura)
const uint16_t* audio_get_internal_buffer_ptr();

// Retorna o número de amostras atualmente armazenadas no buffer interno
uint32_t audio_get_internal_buffer_sample_count();

// Reseta o contador de amostras do buffer interno (prepara para nova gravação)
void audio_reset_internal_buffer();

// --- Funções de Reprodução ---
// Define o nível de saída do PWM com base em uma única amostra de áudio
// A amostra é o valor cru do ADC (0-4095)
void audio_pwm_set_sample(uint gpio_pin, uint16_t adc_sample);

// Reproduz todo o buffer de áudio interno via PWM no pino especificado
void audio_play_buffer_via_pwm(uint gpio_pin);

#endif // AUDIO_H