#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// Dimensões do display OLED
#define ALTURA 64
#define LARGURA 128
#define INTERVALO_TICK_US 100000  // 100 ms

// Estrutura da bola
typedef struct {
    int x;
    int y;
} Bola;

// Inicializa a comunicação com o OLED
void oled_setup() {
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    ssd1306_init();
}

// Limpa o display OLED
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

// Desenha a bolinha no OLED
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

    // Desenha um pixel na posição (x, y)
    ssd1306_draw_line(buffer, x, y, x, y, true);
    render_on_display(buffer, &area);
}

// Move horizontalmente para esquerda (-1) ou direita (+1) aleatoriamente
void mover_horizontal_aleatorio(Bola *b) {
    int direcao = (rand() % 2 == 0) ? -1 : 1;
    b->x += direcao;

    // Impede a bola de sair da tela
    if (b->x < 0) b->x = 0;
    if (b->x >= LARGURA) b->x = LARGURA - 1;
}

int main() {
    stdio_init_all();
    oled_setup();
    oled_clear();

    // Inicializa aleatoriedade com o tempo atual
    srand(to_us_since_boot(get_absolute_time()));

    Bola bola;
    bola.x = LARGURA / 2;  // centro
    bola.y = 0;            // topo

    absolute_time_t ultimo_tick = get_absolute_time();

    while (bola.y < ALTURA) {
        if (absolute_time_diff_us(ultimo_tick, get_absolute_time()) >= INTERVALO_TICK_US) {
            ultimo_tick = get_absolute_time();

            desenha_bola(bola.x, bola.y);    // desenha a bola
            bola.y++;                         // desce
            mover_horizontal_aleatorio(&bola); // desvia aleatoriamente
        }
    }

    return 0;
}
