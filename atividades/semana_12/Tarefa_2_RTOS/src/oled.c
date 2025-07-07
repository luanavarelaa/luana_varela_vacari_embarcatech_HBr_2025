// src/oled.c
#include "oled.h"       // Nosso header do módulo OLED
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <string.h>     // Para memset
#include <stdio.h>      // Para printf

// Pinos I2C - Confirme se são estes para a sua BitDogLab
#define OLED_I2C_PORT i2c1
#define OLED_I2C_SDA_PIN 14
#define OLED_I2C_SCL_PIN 15

// As constantes ssd1306_width, ssd1306_height, ssd1306_n_pages, 
// e ssd1306_buffer_length são fornecidas pela sua biblioteca
// através de "ssd1306_i2c.h" (que é incluído por "ssd1306.h").

void oled_init_display() {
    printf("Inicializando I2C para OLED (modulo oled.c)...\n");
    i2c_init(OLED_I2C_PORT, 400 * 1000); // 400 kHz para I2C, comum para OLEDs
    gpio_set_function(OLED_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_I2C_SDA_PIN);
    gpio_pull_up(OLED_I2C_SCL_PIN);

    printf("Inicializando display SSD1306 via ssd1306_init()...\n");
    ssd1306_init(); // Função da sua biblioteca ssd1306.h/ssd1306_i2c.c
    
    oled_clear_display(); // Limpa ao inicializar
    printf("Display OLED inicializado e limpo (modulo oled.c).\n");
}

void oled_clear_display() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area); 
    
    uint8_t buffer[frame_area.buffer_length]; 
    memset(buffer, 0, frame_area.buffer_length); 
    render_on_display(buffer, &frame_area); 
}

void oled_draw_text_centered(const char *text) {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t buffer[frame_area.buffer_length]; 
    memset(buffer, 0, frame_area.buffer_length); 
    
    // Sua biblioteca ssd1306_draw_char avança x em 8.
    int char_width = 8; 
    int char_height = 8; 
    int text_len = strlen(text);
    int text_pixel_width = text_len * char_width;
    
    int x_pos = (ssd1306_width - text_pixel_width) / 2;
    int y_pos = (ssd1306_height - char_height) / 2; 

    if (x_pos < 0) x_pos = 0;
    if (y_pos < 0) y_pos = 0;

    ssd1306_draw_string(buffer, x_pos, y_pos, (char*)text);

    render_on_display(buffer, &frame_area); 
}

