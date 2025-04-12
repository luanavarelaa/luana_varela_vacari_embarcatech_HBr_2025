#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"            // Biblioteca para o display OLED SSD1306
#include "hardware/i2c.h"           // Biblioteca para comunicação I2C
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040

// Inicializa o display OLED
void oled_setup() {
    i2c_init(i2c1, 100000);                  // Inicializa o I2C1 com frequência de 100kHz
    gpio_set_function(14, GPIO_FUNC_I2C);    // Define GPIO 14 como função I2C (SDA)
    gpio_set_function(15, GPIO_FUNC_I2C);    // Define GPIO 15 como função I2C (SCL)
    gpio_pull_up(14);                        // Habilita resistor de pull-up no SDA
    gpio_pull_up(15);                        // Habilita resistor de pull-up no SCL
    ssd1306_init();                          // Inicializa o display OLED SSD1306
}

// Limpa completamente o conteúdo do display OLED
void oled_clear() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);  // Calcula o tamanho do buffer
    uint8_t ssd[ssd1306_buffer_length];                // Cria buffer para envio ao display
    memset(ssd, 0, ssd1306_buffer_length);             // Preenche com zeros (limpa a tela)
    render_on_display(ssd, &frame_area);               // Envia buffer limpo para o display
}

// Exibe um texto centralizado no display OLED
void oled_display_text(const char *text) {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);             // Limpa o buffer antes de desenhar

    int text_width = strlen(text) * 6;                 // 6 pixels por caractere
    int x = (ssd1306_width - text_width) / 2;          // Centraliza horizontalmente
    int y = (ssd1306_height - 8) / 2;                  // Centraliza verticalmente (fonte = 8px)

    ssd1306_draw_string(ssd, x, y, text);              // Desenha texto no buffer
    render_on_display(ssd, &frame_area);               // Atualiza o display com o buffer
}


int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
