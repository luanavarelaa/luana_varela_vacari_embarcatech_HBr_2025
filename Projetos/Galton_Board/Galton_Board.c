#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>

// Dimensões do display OLED
#define ALTURA 64
#define LARGURA 128
#define INTERVALO_TICK_US 100000  // 100 ms em microssegundos

typedef struct {
    int x;
    int y;
} Bola;

void oled_setup() {
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    ssd1306_init();
}

void oled_clear() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t buffer[ssd1306_buffer_length];
    memset(buffer, 0, ssd1306_buffer_length);
    render_on_display(buffer, &frame_area);
}

void desenha_bola(int x, int y) {
    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    uint8_t buffer[ssd1306_buffer_length];
    memset(buffer, 0, ssd1306_buffer_length);

    ssd1306_draw_line(buffer, x, y, x, y, true);  // desenha pixel
    render_on_display(buffer, &area);            // atualiza display
}

int main() {
    stdio_init_all();
    oled_setup();
    oled_clear();

    Bola bola;
    bola.x = LARGURA / 2;
    bola.y = 0;

    absolute_time_t ultimo_tick = get_absolute_time();

    while (bola.y < ALTURA) {
        // Verifica se passou 100ms desde o último tick
        if (absolute_time_diff_us(ultimo_tick, get_absolute_time()) >= INTERVALO_TICK_US) {
            ultimo_tick = get_absolute_time(); // atualiza tempo do último tick

            desenha_bola(bola.x, bola.y); // mostra a bolinha
            bola.y++;                     // move para a próxima linha
        }
    }

    return 0;
}
