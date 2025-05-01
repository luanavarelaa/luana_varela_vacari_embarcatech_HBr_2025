#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

#define ALTURA 64
#define LARGURA 128
#define INTERVALO_TICK_US 100000
#define ESPACO_HORIZONTAL 2
#define ESPACO_VERTICAL 2
#define INICIO_PINOS 16
#define FIM_PINOS 40

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

bool esta_em_pino(int x, int y) {
    if (y < INICIO_PINOS || y > FIM_PINOS) return false;

    int linha = (y - INICIO_PINOS) / ESPACO_VERTICAL;
    int deslocamento = (linha % 2 == 0) ? 0 : ESPACO_HORIZONTAL / 2;

    int esperado_x = deslocamento;
    while (esperado_x < LARGURA) {
        if (x == esperado_x) return true;
        esperado_x += ESPACO_HORIZONTAL;
    }
    return false;
}

void desenha_cena(Bola *b) {
    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    uint8_t buffer[ssd1306_buffer_length];
    memset(buffer, 0, ssd1306_buffer_length);

    for (int linha_y = INICIO_PINOS; linha_y <= FIM_PINOS; linha_y += ESPACO_VERTICAL) {
        int linha = (linha_y - INICIO_PINOS) / ESPACO_VERTICAL;
        int deslocamento = (linha % 2 == 0) ? 0 : ESPACO_HORIZONTAL / 2;

        for (int col = deslocamento; col < LARGURA; col += ESPACO_HORIZONTAL) {
            ssd1306_draw_line(buffer, col, linha_y, col, linha_y, true);
        }
    }

    // Desenha a bolinha
    ssd1306_draw_line(buffer, b->x, b->y, b->x, b->y, true);

    render_on_display(buffer, &area);
}

void mover_horizontal_aleatorio(Bola *b) {
    int direcao = (rand() % 2 == 0) ? -1 : 1;
    b->x += direcao;

    if (b->x < 0) b->x = 0;
    if (b->x >= LARGURA) b->x = LARGURA - 1;
}

int main() {
    oled_setup();
    oled_clear();
    srand(to_us_since_boot(get_absolute_time()));

    Bola bola;
    bola.x = LARGURA / 2;
    bola.y = 0;

    absolute_time_t ultimo_tick = get_absolute_time();

    while (bola.y < ALTURA - 1) {
        if (absolute_time_diff_us(ultimo_tick, get_absolute_time()) >= INTERVALO_TICK_US) {
            ultimo_tick = get_absolute_time();

            if (esta_em_pino(bola.x, bola.y)) {
                mover_horizontal_aleatorio(&bola);
            }

            desenha_cena(&bola);
            bola.y++;
        }
    }

    return 0;
}
