// inc/leds.h
#ifndef LEDS_H
#define LEDS_H

#include "pico/stdlib.h"

// Inicializa os pinos dos LEDs como saída
void leds_init();

// Controla o LED de gravação (Vermelho)
// on = true para acender, false para apagar
void led_gravacao_set(bool on);

// Controla o LED de reprodução (Verde)
// on = true para acender, false para apagar
void led_reproducao_set(bool on);

// Opcional: Controla um LED de estado ocioso (ex: Azul)
// void led_idle_set(bool on);

// Apaga todos os LEDs de feedback
void leds_desligar_todos();

#endif // LEDS_H
