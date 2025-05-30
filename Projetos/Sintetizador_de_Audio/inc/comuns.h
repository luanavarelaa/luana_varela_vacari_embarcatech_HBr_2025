#ifndef COMUNS_H
#define COMUNS_H
#include "pico/stdlib.h" // Para tipos como uint16_t, etc.

// Definições de pinos 
#define MIC_CHANNEL 2 // GPIO 28 para o microfone
#define MIC_PIN (26 + MIC_CHANNEL)

#define BUZZER_PIN 21 // buzzer A
#define PWM_WRAP_VALUE 2047 // Para mapear amostras ADC de 12 bits (0-4095) dividindo por 2

#define BUTTON_A_PIN 5 // Botão A
#define BUTTON_B_PIN 6 // Botão B 

#define LED_R_PIN 13 // LED Vermelho para indicar gravação
#define LED_G_PIN 11 // LED Verde para indicar reprodução
#define LED_B_PIN 12 // LED Azul para indicar estado ocioso/pronto

#define RECORDING_DURATION_S 2

#define TARGET_SAMPLE_RATE_HZ 8000
#define AUDIO_BUFFER_SIZE 256

#define AUDIO_BUFFER_SIZE (TARGET_SAMPLE_RATE_HZ * RECORDING_DURATION_S)

// --- Estados do Sintetizador ---
typedef enum {
    STATE_IDLE,              // Ocioso, esperando para gravar
    STATE_RECORDING_DONE,    // Gravação concluída, pronto para tocar
} SynthesizerState;

#endif // COMUNS.H