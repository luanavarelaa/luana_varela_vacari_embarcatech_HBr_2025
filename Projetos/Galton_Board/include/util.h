#ifndef UTIL_H
#define UTIL_H

// Dimensões do display OLED
#define ALTURA 64
#define LARGURA 128

// Intervalo de tempo entre atualizações (100ms)
#define INTERVALO_TICK_US 100000

// Espaçamento entre pinos na simulação
#define ESPACO_HORIZONTAL 2
#define ESPACO_VERTICAL 2

// Limites verticais da região com pinos
#define INICIO_PINOS 16
#define FIM_PINOS 36

// Margem superior das canaletas (base onde as bolas param)
#define MARGEM_CANALETAS 8

// Definição das canaletas (histograma)
#define NUM_CANALETAS 32
#define LARGURA_CANALETA (LARGURA / NUM_CANALETAS)

// Máximo de bolas simultâneas na tela
#define MAX_BOLAS 20

// Intervalo (em "ticks") para gerar nova bola
#define TICKS_NOVA_BOLA 5

// Largura da abertura superior por onde caem as bolas
#define LARGURA_ABERTURA 20

// Altura máxima das barras do histograma
#define ALTURA_HISTOGRAMA 20

#endif
