#include <stdio.h>
#include <stdlib.h>
#include "bola.h"

// Vetor que armazena quantas bolas caíram em cada canaleta
int frequencias[NUM_CANALETAS] = {0};

// Contador total de bolas caídas
int total_bolas_caidas = 0;

// Identifica em qual canaleta a bola caiu com base no x
int identificar_canaleta(int x) {
    int indice = x / LARGURA_CANALETA;
    if (indice >= NUM_CANALETAS) indice = NUM_CANALETAS - 1;
    return indice;
}
// Favorecendo o lado direito
/*void mover_horizontal_aleatorio(Bola *b) {
    // 0 → esquerda (25%), 1/2/3 → direita (75%)
    int sorteio = rand() % 4; 
    int direcao = (sorteio == 0) ? -1 : 1; 

    b->x += direcao;

    if (b->x < 0) b->x = 0;
    if (b->x >= LARGURA) b->x = LARGURA - 1;
}*/

// Movimento aleatório horizontal (esquerda/direita) ao colidir com pino
void mover_horizontal_aleatorio(Bola *b) {
    int direcao = (rand() % 2 == 0) ? -1 : 1;
    b->x += direcao;
    if (b->x < 0) b->x = 0;
    if (b->x >= LARGURA) b->x = LARGURA - 1;
}

// Imprime no terminal as contagens atuais das canaletas
void imprimir_frequencias() {
    printf("Contagem por canaleta:\n");
    for (int i = 0; i < NUM_CANALETAS; i++) {
        printf("Canaleta %2d: %d\n", i, frequencias[i]);
    }
    printf("------\n");
}
