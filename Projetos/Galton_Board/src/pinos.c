#include "pinos.h"

// Verifica se uma posição (x,y) está em cima de um pino
bool esta_em_pino(int x, int y) {
    if (y < INICIO_PINOS || y > FIM_PINOS) return false;

    int linha = (y - INICIO_PINOS) / ESPACO_VERTICAL;
    int deslocamento = (linha % 2 == 0) ? 0 : ESPACO_HORIZONTAL / 2;

    for (int esperado_x = deslocamento; esperado_x < LARGURA; esperado_x += ESPACO_HORIZONTAL) {
        if (x == esperado_x) return true;
    }

    return false;
}
