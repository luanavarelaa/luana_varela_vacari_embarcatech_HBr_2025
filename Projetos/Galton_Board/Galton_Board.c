#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

#define ALTURA 64
#define LARGURA 128
#define INTERVALO_TICK_US 100000
#define OBSTACULO_Y 32  // Obstáculo fixo na linha 32

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

    ssd1306_draw_line(buffer, x, y, x, y, true);
    render_on_display(buffer, &area);
}

void mover_horizontal_aleatorio(Bola *b) {
    int direcao = (rand() % 2 == 0) ? -1 : 1;
    b->x += direcao;

    if (b->x < 0) b->x = 0;
    if (b->x >= LARGURA) b->x = LARGURA - 1;
}

int main() {
    stdio_init_all();
    oled_setup();
    oled_clear();
    srand(to_us_since_boot(get_absolute_time()));

    Bola bola;
    bola.x = LARGURA / 2;
    bola.y = 0;

    absolute_time_t ultimo_tick = get_absolute_time();

    printf("Iniciando simulação da Galton Board Digital...\n");
    printf("Obstáculo virtual está na linha Y = %d\n", OBSTACULO_Y);

    while (bola.y < ALTURA) {
        if (absolute_time_diff_us(ultimo_tick, get_absolute_time()) >= INTERVALO_TICK_US) {
            ultimo_tick = get_absolute_time();

            printf("Bola em (x=%d, y=%d)", bola.x, bola.y);

            if (bola.y == OBSTACULO_Y) {
                mover_horizontal_aleatorio(&bola);
                printf(" --> Obstáculo atingido! Novo X: %d", bola.x);
            }

            printf("\n");

            desenha_bola(bola.x, bola.y);
            bola.y++;
        }
    }

    printf("Bola chegou ao fundo! Posição final X: %d\n", bola.x);

    return 0;
}
