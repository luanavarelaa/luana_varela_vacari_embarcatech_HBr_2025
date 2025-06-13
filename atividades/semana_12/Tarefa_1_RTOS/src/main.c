#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tarefas.h" // Inclui as declarações das tarefas e handles

// --- Definição dos Handles das Tarefas ---
// Estas são as variáveis globais que o 'extern' em tasks.h aponta.
TaskHandle_t led_task_handle = NULL;
TaskHandle_t buzzer_task_handle = NULL;

/**
 Ponto de entrada principal do programa.
 * Responsável por inicializar o sistema, criar as tarefas e iniciar o FreeRTOS.
 */
int main() {
    stdio_init_all();

    // Cria as 3 tarefas, passando suas funções, nomes, tamanho de pilha e prioridades.
    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, &led_task_handle);
    xTaskCreate(buzzer_task, "Buzzer_Task", 256, NULL, 1, &buzzer_task_handle);
    xTaskCreate(button_task, "Button_Task", 256, NULL, 2, NULL);

    // Inicia o escalonador do FreeRTOS.
    vTaskStartScheduler();

    

    // O código abaixo desta linha nunca deve ser alcançado.
    while (1) {};
}
