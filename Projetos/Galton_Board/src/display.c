#include <string.h>
#include <stdio.h>
#include "display.h"
#include "histograma.h"
#include "pinos.h"
#include "bola.h"

// Inicializa o display OLED via I2C
void oled_setup() {
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    ssd1306_init();
}

// Limpa a tela OLED
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

// Desenha todos os elementos visuais da cena
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

    // Desenha pinos em padrão triangular
    for (int linha_y = INICIO_PINOS; linha_y <= FIM_PINOS; linha_y += ESPACO_VERTICAL) {
        int linha = (linha_y - INICIO_PINOS) / ESPACO_VERTICAL;
        int deslocamento = (linha % 2 == 0) ? 0 : ESPACO_HORIZONTAL / 2;

        for (int col = deslocamento; col < LARGURA; col += ESPACO_HORIZONTAL) {
            ssd1306_draw_line(buffer, col, linha_y, col, linha_y, true);
        }
    }

    // Desenha divisões verticais das canaletas
    int base_y = FIM_PINOS + MARGEM_CANALETAS;
    for (int i = 1; i < NUM_CANALETAS; i++) {
        int x = i * LARGURA_CANALETA;
        ssd1306_draw_line(buffer, x, base_y, x, ALTURA - 1, true);
    }

    // Desenha as bolas ativas
    for (int i = 0; i < MAX_BOLAS; i++) {
        if (bolas[i].ativa) {
            ssd1306_draw_line(buffer, bolas[i].x, bolas[i].y, bolas[i].x, bolas[i].y, true);
        }
    }

    // Desenha o histograma
    desenhar_histograma(buffer);

    // Mostra o contador de bolas no canto superior direito
    char texto[20];
    snprintf(texto, sizeof(texto), "%d", total_bolas_caidas);
    ssd1306_draw_string(buffer, 90, 0, texto);

    // Renderiza a cena completa
    render_on_display(buffer, &area);
}
