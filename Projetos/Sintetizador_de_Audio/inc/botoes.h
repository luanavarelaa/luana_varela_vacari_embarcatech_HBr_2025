#ifndef BOTOES_H
#define BOTOES_H

#include "pico/stdlib.h"

// Definição dos pinos dos botões (conforme Projeto_Final.c e nosso uso)
#define BOTAO_A_PIN 5
#define BOTAO_B_PIN 6

// Flags voláteis para serem atualizadas pelos callbacks dos timers e lidas pelo main
// Indicam que um pressionamento completo e debounced ocorreu.
extern volatile bool g_botao_a_foi_pressionado_event;
extern volatile bool g_botao_b_foi_pressionado_event;

// Inicializa os pinos dos botões e configura os timers para polling e debounce
// polling_interval_ms: intervalo em milissegundos para os timers chamarem os callbacks
void botoes_init_com_timers(uint32_t polling_interval_ms);

// Funções para o loop principal verificar se um evento de pressionamento ocorreu
// e para limpar a flag do evento.
bool verificar_e_limpar_evento_botao_a();
bool verificar_e_limpar_evento_botao_b();

#endif // BOTOES_H
