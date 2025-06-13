#ifndef TAREFAS_H
#define TAREFAS_H

#include "FreeRTOS.h"
#include "task.h"

// --- Handles das Tarefas ---
// Declarados como 'extern' para serem acessíveis globalmente.
// Eles serão definidos em main.c.
extern TaskHandle_t led_task_handle;
extern TaskHandle_t buzzer_task_handle;

// --- Declarações das Funções de Tarefa ---
// Permite que main.c crie as tarefas cujas lógicas estão em outros arquivos.
void led_task(void *params);
void buzzer_task(void *params);
void button_task(void *params);

#endif 
