#ifndef DISPLAY_H
#define DISPLAY_H

#include "ssd1306.h"
#include "util.h"
#include "bola.h"

// Inicializa o display OLED via I2C
void oled_setup();

// Limpa a tela OLED
void oled_clear();

// Desenha todos os elementos visuais da cena
void desenha_cena(Bola bolas[MAX_BOLAS]);


#endif
