//bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/display.h"
#include "include/util.h"
#include "include/bola.h"
#include "include/pinos.h"

int main() {
    stdio_init_all();           // Inicializa comunicação com terminal
    oled_setup();               // Inicializa tela OLED
    oled_clear();               // Limpa tela
    srand(to_us_since_boot(get_absolute_time()));  // Semente para aleatoriedade

    Bola bolas[MAX_BOLAS] = {0};                   // Array de bolas
    absolute_time_t ultimo_tick = get_absolute_time();
    int tick_count = 0;

    while (true) {
        // Verifica se chegou o momento de atualizar (baseado no intervalo)
        if (absolute_time_diff_us(ultimo_tick, get_absolute_time()) >= INTERVALO_TICK_US) {
            ultimo_tick = get_absolute_time();
            tick_count++;

            // Cria nova bola a cada TICKS_NOVA_BOLA
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

            // Atualiza posição das bolas
            for (int i = 0; i < MAX_BOLAS; i++) {
                if (!bolas[i].ativa) continue;

                // Se colidiu com pino, move lateralmente
                if (esta_em_pino(bolas[i].x, bolas[i].y)) {
                    mover_horizontal_aleatorio(&bolas[i]);
                }

                bolas[i].y++;  // Movimento vertical

                // Quando chega na base
                if (bolas[i].y >= ALTURA - 1) {
                    int canaleta = identificar_canaleta(bolas[i].x);
                    frequencias[canaleta]++;
                    total_bolas_caidas++;   // Atualiza contador global
                    imprimir_frequencias();
                    bolas[i].ativa = false; // Desativa a bola
                }
            }

            // Redesenha a tela com novo estado
            desenha_cena(bolas);
        }
    }

    return 0;
}
