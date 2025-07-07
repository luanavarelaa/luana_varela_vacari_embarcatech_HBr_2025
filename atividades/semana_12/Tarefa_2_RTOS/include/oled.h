#ifndef OLED_H
#define OLED_H

#include "pico/stdlib.h"
// Inclui o header principal da SUA biblioteca OLED
#include "ssd1306.h" 

// Inicializa o display OLED (configura I2C e o display)
void oled_init_display();

// Limpa todo o conteúdo do display OLED
void oled_clear_display();

// Exibe uma mensagem de texto centralizada no display
void oled_draw_text_centered(const char *text);

#endif // OLED_H