// src/main.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "audio.h"
#include "botoes.h" 
#include "leds.h"   
#include "comuns.h"

int main() {
    stdio_init_all(); 

    for(int i = 5; i > 0; i--){
        printf("Aguardando para iniciar... %d\n", i);
        sleep_ms(1000);
    }
    printf("Sintetizador de Audio com Feedback Visual Completo\n");
    
    // Inicializações
    audio_init(TARGET_SAMPLE_RATE_HZ);
    audio_init_pwm(BUZZER_PIN, PWM_WRAP_VALUE);
    botoes_init_com_timers(15); 
    leds_init(); 

    printf("Pressione Botao A (GP%d) para Gravar.\n", BUTTON_A_PIN);
    printf("Pressione Botao B (GP%d) para Tocar (apos gravar).\n", BUTTON_B_PIN);

    uint32_t samples_to_collect = AUDIO_BUFFER_SIZE;
    SynthesizerState current_state = STATE_IDLE; 
    uint32_t recorded_samples_count = 0;

    leds_feedback_acao_desligar(); // Apaga LEDs de gravação/reprodução
    led_idle_set(true);          // NOVO: Acende LED azul para indicar sistema pronto/ocioso

    while (true) {
        switch (current_state) {
            case STATE_IDLE:
                // LED Idle já deve estar aceso.
                if (verificar_e_limpar_evento_botao_a()) { 
                    led_idle_set(false);       // NOVO: Apaga LED idle
                    led_gravacao_set(true);    // Acende LED vermelho
                    printf("\nBotao A pressionado! Iniciando gravacao de %lu amostras...\n", samples_to_collect);
                    
                    uint64_t start_time_us = time_us_64();
                    recorded_samples_count = audio_record_to_internal_buffer(samples_to_collect);
                    uint64_t end_time_us = time_us_64();
                    
                    led_gravacao_set(false);   // Apaga LED vermelho
                    
                    uint64_t recording_duration_us = end_time_us - start_time_us;
                    float recording_duration_s = recording_duration_us / 1000000.0f;
                    
                    printf("Gravacao levou aprox. %.3f segundos para %lu amostras.\n", recording_duration_s, recorded_samples_count);
                    if (recording_duration_s > 0 && recorded_samples_count > 0) {
                        float measured_sample_rate = recorded_samples_count / recording_duration_s;
                        printf("Taxa de amostragem medida: %.2f Hz\n", measured_sample_rate);
                        current_state = STATE_RECORDING_DONE; 
                        printf("Gravacao concluida. Pressione Botao B para tocar.\n");
                        // Não acende o LED idle aqui ainda, pois estamos em "recording_done"
                    } else {
                        printf("Falha na gravacao ou nenhuma amostra gravada.\n");
                        led_idle_set(true); // NOVO: Volta para o estado idle visualmente
                    }
                }
                break;

            case STATE_RECORDING_DONE:
                // Se não estiver fazendo nada, o LED idle pode ficar aceso para indicar "pronto para próxima ação"
                led_idle_set(true); // NOVO: Acende LED idle enquanto espera o botão B

                if (verificar_e_limpar_evento_botao_b()) { 
                    led_idle_set(false);      // NOVO: Apaga LED idle
                    printf("\nBotao B pressionado! Iniciando reproducao...\n");
                    if (audio_get_internal_buffer_sample_count() > 0) {
                        led_reproducao_set(true); // Acende LED verde
                        audio_play_buffer_via_pwm(BUZZER_PIN);
                        led_reproducao_set(false); // Apaga LED verde
                    } else {
                        printf("Nenhuma gravacao encontrada para tocar.\n");
                    }
                    current_state = STATE_IDLE; 
                    printf("Reproducao finalizada. Pressione Botao A para nova gravacao.\n");
                    led_idle_set(true); // NOVO: Volta para o estado idle visualmente
                } 
                else if (verificar_e_limpar_evento_botao_a()) { // Permitir gravar novamente
                    led_idle_set(false);       // NOVO: Apaga LED idle
                    led_gravacao_set(true);    // Acende LED vermelho
                    printf("\nBotao A pressionado! Iniciando NOVA gravacao de %lu amostras...\n", samples_to_collect);
                    
                    uint64_t start_time_us = time_us_64();
                    recorded_samples_count = audio_record_to_internal_buffer(samples_to_collect);
                    uint64_t end_time_us = time_us_64();
                    
                    led_gravacao_set(false);   // Apaga LED vermelho

                    uint64_t recording_duration_us = end_time_us - start_time_us;
                    float recording_duration_s = recording_duration_us / 1000000.0f;
                    printf("Gravacao levou aprox. %.3f segundos para %lu amostras.\n", recording_duration_s, recorded_samples_count);
                     if (recording_duration_s > 0 && recorded_samples_count > 0) {
                        printf("Nova gravacao concluida. Pressione Botao B para tocar.\n");
                        // Permanece em STATE_RECORDING_DONE, o LED idle será aceso no início do próximo ciclo deste estado
                    } else {
                        printf("Falha na nova gravacao.\n");
                        current_state = STATE_IDLE; 
                        led_idle_set(true); // NOVO: Volta para o estado idle visualmente
                    }
                }
                break;
        }
        sleep_ms(1); 
    }
    return 0; 
}
