#ifndef BOLA_H
#define BOLA_H

#include <stdbool.h>
#include "util.h"

// Estrutura que representa uma bola
typedef struct {
    int x;         // Posição horizontal
    int y;         // Posição vertical
    bool ativa;    // Estado ativo (em movimento) ou não
} Bola;

// Vetor que armazena quantas bolas caíram em cada canaleta
extern int frequencias[NUM_CANALETAS];

// Contador total de bolas caídas
extern int total_bolas_caidas;

// Identifica em qual canaleta a bola caiu com base no x
int identificar_canaleta(int x);

// Movimento aleatório horizontal (esquerda/direita) ao colidir com pino
void mover_horizontal_aleatorio(Bola *b);

// Imprime no terminal as contagens atuais das canaletas
void imprimir_frequencias();

#endif
