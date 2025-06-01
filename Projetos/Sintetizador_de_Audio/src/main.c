// src/main.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "audio.h"
#include "botoes.h" 
#include "leds.h"   
#include "oled.h"   // Inclui o nosso módulo OLED
#include "comuns.h"

int main() {
    stdio_init_all(); 

    for(int i = 3; i > 0; i--){ 
        printf("Sintetizador iniciando em... %d\n", i);
        sleep_ms(1000);
    }
    printf("Iniciando Sintetizador de Audio com Display OLED...\n");
    
    audio_init(TARGET_SAMPLE_RATE_HZ);
    audio_init_pwm(BUZZER_PIN, PWM_WRAP_VALUE);
    botoes_init_com_timers(15); 
    leds_init(); 
    oled_init_display(); 

    leds_feedback_acao_desligar(); 
    led_idle_set(true);          
    oled_draw_text_centered("Pronto!"); 
    
    printf("Pressione Botao A (GP%d) para Gravar.\n", BOTAO_A_PIN);
    printf("Pressione Botao B (GP%d) para Tocar (apos gravar).\n", BOTAO_B_PIN);

    uint32_t samples_to_collect = AUDIO_BUFFER_SIZE;
    SynthesizerState current_state = STATE_IDLE; 
    uint32_t recorded_samples_count = 0;

    while (true) {
        switch (current_state) {
            case STATE_IDLE:
                if (verificar_e_limpar_evento_botao_a()) { 
                    led_idle_set(false);       
                    led_gravacao_set(true);    
                    oled_draw_text_centered("Gravando..."); 
                    printf("\nBotao A pressionado! Iniciando gravacao...\n");
                    
                    recorded_samples_count = audio_record_to_internal_buffer(samples_to_collect);
                    
                    led_gravacao_set(false);   
                    
                    if (recorded_samples_count > 0) {
                        current_state = STATE_RECORDING_DONE; 
                        printf("Gravacao concluida. Exibindo forma de onda...\n");
                        // Exibe a forma de onda APÓS a gravação
                        oled_display_waveform(audio_get_internal_buffer_ptr(), audio_get_internal_buffer_sample_count());
                        printf("Forma de onda exibida. Pressione Botao B para tocar.\n");
                    } else {
                        printf("Falha na gravacao.\n");
                        oled_draw_text_centered("Falha Grav."); 
                        sleep_ms(1500); 
                        oled_draw_text_centered("Pronto!"); 
                        led_idle_set(true); 
                    }
                }
                break;

            case STATE_RECORDING_DONE:
                led_idle_set(true); 
                // A forma de onda já foi exibida. Se o usuário demorar para apertar B,
                // ela permanecerá na tela. Se quiser, pode reexibir "Gravado!" aqui ou
                // limpar e exibir "Pressione B". Por ora, a forma de onda fica.

                if (verificar_e_limpar_evento_botao_b()) { 
                    led_idle_set(false);      
                    printf("\nBotao B pressionado! Iniciando reproducao...\n");
                    if (audio_get_internal_buffer_sample_count() > 0) {
                        oled_draw_text_centered("Tocando..."); 
                        
                        led_reproducao_set(true); 
                        audio_play_buffer_via_pwm(BUZZER_PIN);
                        led_reproducao_set(false); 
                        
                        oled_draw_text_centered("Pronto!"); 
                    } else {
                        printf("Nenhuma gravacao para tocar.\n");
                        oled_draw_text_centered("Nada Grav."); 
                        sleep_ms(1500);
                        oled_draw_text_centered("Pronto!");    
                    }
                    current_state = STATE_IDLE; 
                    printf("Reproducao finalizada. Pressione Botao A para nova gravacao.\n");
                    led_idle_set(true); 
                } 
                else if (verificar_e_limpar_evento_botao_a()) { // Permitir gravar novamente
                    led_idle_set(false);       
                    led_gravacao_set(true);    
                    oled_draw_text_centered("Gravando...");
                    printf("\nBotao A pressionado! Iniciando NOVA gravacao...\n");
                    recorded_samples_count = audio_record_to_internal_buffer(samples_to_collect);
                    led_gravacao_set(false);   
                    if (recorded_samples_count > 0) {
                        printf("Nova gravacao concluida. Exibindo forma de onda...\n");
                        oled_display_waveform(audio_get_internal_buffer_ptr(), audio_get_internal_buffer_sample_count());
                        printf("Pressione Botao B para tocar.\n");
                        // Permanece em STATE_RECORDING_DONE
                    } else {
                        printf("Falha na nova gravacao.\n");
                        oled_draw_text_centered("Falha Grav.");
                        sleep_ms(1500);
                        oled_draw_text_centered("Pronto!"); 
                        current_state = STATE_IDLE; 
                        led_idle_set(true); 
                    }
                }
                break;
        }
        sleep_ms(10); 
    }
    return 0; 
}
