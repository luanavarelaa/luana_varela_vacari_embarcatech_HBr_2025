#include "botoes.h"
#include "hardware/gpio.h"
#include "pico/time.h" // Para add_repeating_timer_ms, get_absolute_time, absolute_time_diff_us
#include <stdio.h>     // Para printf na inicialização
#include "comuns.h"

// Flags de evento (true quando um pressionamento debounced é detectado)
volatile bool g_botao_a_foi_pressionado_event = false;
volatile bool g_botao_b_foi_pressionado_event = false;

// Tempo de debounce em microssegundos.
// O Projeto_Final.c usa 300ms (300000us).
// Para resposta mais rápida, 50ms (50000us) é uma boa opção.
// Pode ajustar este valor conforme a sua preferência.
#define DEBOUNCE_TIME_US (50 * 1000) // 50ms 

// Variáveis de estado para o debounce do Botão A (inspirado em Projeto_Final.c)
static absolute_time_t ultima_pressao_A = {0};    // Timestamp do último evento de pressionamento válido
static bool ultimo_estado_botao_A = false;      // false = botão estava solto ou o pressionamento já foi processado
                                                // true = botão está sendo considerado pressionado (após debounce ter gerado evento)

// Variáveis de estado para o debounce do Botão B
static absolute_time_t ultima_pressao_B = {0};
static bool ultimo_estado_botao_B = false;

// Estruturas para os timers de debounce dos botões
static struct repeating_timer botao_timer_A_info;
static struct repeating_timer botao_timer_B_info;

// Callback para monitorar o botão A
bool verificar_botao_A_callback(struct repeating_timer *t) {
    bool botao_pressionado_agora = !gpio_get(BOTAO_A_PIN); // true se GPIO está LOW (pressionado)

    if (botao_pressionado_agora) {
        // Botão está fisicamente pressionado
        if (!ultimo_estado_botao_A) { // Se não estávamos já considerando ele pressionado (evita múltiplos eventos por manter pressionado)
            if (absolute_time_diff_us(ultima_pressao_A, get_absolute_time()) > DEBOUNCE_TIME_US) {
                g_botao_a_foi_pressionado_event = true; // Sinaliza o evento para main.c
                ultima_pressao_A = get_absolute_time();   // Atualiza o timestamp do último evento válido
                ultimo_estado_botao_A = true;           // Marca que estamos tratando este pressionamento
            }
        }
    } else {
        // Botão está fisicamente solto
        ultimo_estado_botao_A = false; // Reseta, pronto para o próximo ciclo de pressionamento
    }
    return true; // Mantém o timer ativo
}

// Callback para monitorar o botão B
bool verificar_botao_B_callback(struct repeating_timer *t) {
    bool botao_pressionado_agora = !gpio_get(BOTAO_B_PIN);

    if (botao_pressionado_agora) {
        if (!ultimo_estado_botao_B) {
            if (absolute_time_diff_us(ultima_pressao_B, get_absolute_time()) > DEBOUNCE_TIME_US) {
                g_botao_b_foi_pressionado_event = true;
                ultima_pressao_B = get_absolute_time();
                ultimo_estado_botao_B = true;
            }
        }
    } else {
        ultimo_estado_botao_B = false;
    }
    return true; // Mantém o timer ativo
}

void botoes_init_com_timers(uint32_t polling_interval_ms) {
    // Inicializa Botão A
    gpio_init(BOTAO_A_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);
    ultima_pressao_A = nil_time; 
    ultimo_estado_botao_A = false;

    // Inicializa Botão B
    gpio_init(BOTAO_B_PIN);
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_B_PIN);
    ultima_pressao_B = nil_time;
    ultimo_estado_botao_B = false;

    // Configuração dos temporizadores para debounce dos botões
    if (!add_repeating_timer_ms(- (int32_t)polling_interval_ms, 
                                verificar_botao_A_callback, 
                                NULL, 
                                &botao_timer_A_info)) {
        printf("Falha ao adicionar timer para Botao A!\n");
    }

    if (!add_repeating_timer_ms(- (int32_t)polling_interval_ms, 
                                verificar_botao_B_callback, 
                                NULL, 
                                &botao_timer_B_info)) {
        printf("Falha ao adicionar timer para Botao B!\n");
    }
    
    printf("Botoes inicializados com timers separados (intervalo de polling: %lu ms, debounce: %lu us).\n", 
           polling_interval_ms, (unsigned long)DEBOUNCE_TIME_US);
    printf("Botao A (GP%d), Botao B (GP%d)\n", BUTTON_A_PIN, BUTTON_B_PIN);
}

bool verificar_e_limpar_evento_botao_a() {
    bool evento_ocorreu = false;
    // Em um sistema com múltiplas interrupções ou RTOS, proteger o acesso a g_botao_a_foi_pressionado_event
    // com uma seção crítica (desabilitar interrupções brevemente) seria mais robusto.
    // uint32_t RASCUNHO_INTERRUPCAO = save_and_disable_interrupts();
    if (g_botao_a_foi_pressionado_event) {
        evento_ocorreu = true;
        g_botao_a_foi_pressionado_event = false; 
    }
    // restore_interrupts(RASCUNHO_INTERRUPCAO);
    return evento_ocorreu;
}

bool verificar_e_limpar_evento_botao_b() {
    bool evento_ocorreu = false;
    if (g_botao_b_foi_pressionado_event) {
        evento_ocorreu = true;
        g_botao_b_foi_pressionado_event = false;
    }
    return evento_ocorreu;
}
