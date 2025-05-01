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
#define FIM_PINOS 36
#define MARGEM_CANALETAS 8
#define NUM_CANALETAS 32
#define LARGURA_CANALETA (LARGURA / NUM_CANALETAS)

#define MAX_BOLAS 20
#define TICKS_NOVA_BOLA 5
#define LARGURA_ABERTURA 10
#define ALTURA_HISTOGRAMA 20

typedef struct {
    int x;
    int y;
    bool ativa;
} Bola;

int frequencias[NUM_CANALETAS] = {0};
int total_bolas_caidas = 0;

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

int identificar_canaleta(int x) {
    int indice = x / LARGURA_CANALETA;
    if (indice >= NUM_CANALETAS) indice = NUM_CANALETAS - 1;
    return indice;
}

void desenhar_histograma(uint8_t *buffer) {
    int max_freq = 1;
    for (int i = 0; i < NUM_CANALETAS; i++) {
        if (frequencias[i] > max_freq) {
            max_freq = frequencias[i];
        }
    }

    int base_y = ALTURA - 1;
    for (int i = 0; i < NUM_CANALETAS; i++) {
        int altura_barra = (frequencias[i] * ALTURA_HISTOGRAMA) / max_freq;
        int x_inicio = i * LARGURA_CANALETA;
        int x_fim = x_inicio + LARGURA_CANALETA - 1;

        for (int x = x_inicio; x <= x_fim && x < LARGURA; x++) {
            for (int y = 0; y < altura_barra; y++) {
                ssd1306_draw_line(buffer, x, base_y - y, x, base_y - y, true);
            }
        }
    }
}

void desenha_cena(Bola bolas[MAX_BOLAS]) {
    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    uint8_t buffer[ssd1306_buffer_length];
    memset(buffer, 0, ssd1306_buffer_length);

    // Desenha os pinos
    for (int linha_y = INICIO_PINOS; linha_y <= FIM_PINOS; linha_y += ESPACO_VERTICAL) {
        int linha = (linha_y - INICIO_PINOS) / ESPACO_VERTICAL;
        int deslocamento = (linha % 2 == 0) ? 0 : ESPACO_HORIZONTAL / 2;

        for (int col = deslocamento; col < LARGURA; col += ESPACO_HORIZONTAL) {
            ssd1306_draw_line(buffer, col, linha_y, col, linha_y, true);
        }
    }

    // Divisórias das canaletas
    int base_y = FIM_PINOS + MARGEM_CANALETAS;
    for (int i = 1; i < NUM_CANALETAS; i++) {
        int x = i * LARGURA_CANALETA;
        ssd1306_draw_line(buffer, x, base_y, x, ALTURA - 1, true);
    }

    // Desenha bolas
    for (int i = 0; i < MAX_BOLAS; i++) {
        if (bolas[i].ativa) {
            ssd1306_draw_line(buffer, bolas[i].x, bolas[i].y, bolas[i].x, bolas[i].y, true);
        }
    }

    // Desenha o histograma
    desenhar_histograma(buffer);

    // Mostra contador de bolas caídas
    char texto[20];
    snprintf(texto, sizeof(texto), "%d", total_bolas_caidas);
    ssd1306_draw_string(buffer, 100, 0, texto); // canto superior direito

    render_on_display(buffer, &area);
}

void mover_horizontal_aleatorio(Bola *b) {
    int direcao = (rand() % 2 == 0) ? -1 : 1;
    b->x += direcao;
    if (b->x < 0) b->x = 0;
    if (b->x >= LARGURA) b->x = LARGURA - 1;
}

void imprimir_frequencias() {
    printf("Contagem por canaleta:\n");
    for (int i = 0; i < NUM_CANALETAS; i++) {
        printf("Canaleta %2d: %d\n", i, frequencias[i]);
    }
    printf("------\n");
}

int main() {
    stdio_init_all();
    oled_setup();
    oled_clear();
    srand(to_us_since_boot(get_absolute_time()));

    Bola bolas[MAX_BOLAS] = {0};
    absolute_time_t ultimo_tick = get_absolute_time();
    int tick_count = 0;

    while (true) {
        if (absolute_time_diff_us(ultimo_tick, get_absolute_time()) >= INTERVALO_TICK_US) {
            ultimo_tick = get_absolute_time();
            tick_count++;

            if (tick_count % TICKS_NOVA_BOLA == 0) {
                for (int i = 0; i < MAX_BOLAS; i++) {
                    if (!bolas[i].ativa) {
                        int inicio = LARGURA / 2 - LARGURA_ABERTURA / 2;
                        int fim = LARGURA / 2 + LARGURA_ABERTURA / 2;
                        bolas[i].x = inicio + rand() % (fim - inicio + 1);
                        bolas[i].y = 0;
                        bolas[i].ativa = true;
                        break;
                    }
                }
            }

            for (int i = 0; i < MAX_BOLAS; i++) {
                if (!bolas[i].ativa) continue;

                if (esta_em_pino(bolas[i].x, bolas[i].y)) {
                    mover_horizontal_aleatorio(&bolas[i]);
                }

                bolas[i].y++;

                if (bolas[i].y >= ALTURA - 1) {
                    int canaleta = identificar_canaleta(bolas[i].x);
                    frequencias[canaleta]++;
                    total_bolas_caidas++; // Incrementa contador
                    imprimir_frequencias();
                    bolas[i].ativa = false;
                }
            }

            desenha_cena(bolas);
        }
    }

    return 0;
}
