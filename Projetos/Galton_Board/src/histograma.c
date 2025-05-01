#include "histograma.h"

// Desenha o histograma com base nas frequências das canaletas
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
