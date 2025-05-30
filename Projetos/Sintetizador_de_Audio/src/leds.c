#include "leds.h"
#include "comuns.h" // Para as definições dos pinos dos LEDs
#include "hardware/gpio.h"
#include <stdio.h>

void leds_init() {
    // Inicializa LED de Gravação (Vermelho)
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_put(LED_R_PIN, false); // Começa apagado

    // Inicializa LED de Reprodução (Verde)
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_put(LED_G_PIN, false); // Começa apagado

    // NOVO: Inicializa LED de Idle (Azul)
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_put(LED_B_PIN, false); // Começa apagado, o main.c vai acendê-lo

    printf("LEDs de feedback inicializados: Gravacao (GP%d), Reproducao (GP%d), Idle (GP%d)\n",
           LED_R_PIN, LED_G_PIN, LED_B_PIN);
}

void led_gravacao_set(bool on) {
    gpio_put(LED_R_PIN, on);
}

void led_reproducao_set(bool on) {
    gpio_put(LED_G_PIN, on);
}

// NOVO
void led_idle_set(bool on) {
    gpio_put(LED_B_PIN, on);
}

void leds_feedback_acao_desligar() {
    led_gravacao_set(false);
    led_reproducao_set(false);
    // O LED de idle não é desligado aqui, pois ele indica o estado geral.
    // O main.c controlará o LED de idle especificamente.
}
