#include "pico/stdlib.h"
#include "hardware/pwm.h" // Adicionado para controlar o PWM do buzzer diretamente
#include "FreeRTOS.h"
#include "task.h"
#include "comuns.h"
#include "tarefas.h"

/**
 Tarefa para monitorar os botões A e B.
 Suspende uma tarefa enquanto o botão está pressionado e a retoma ao soltar.
 */
void button_task(void *params) {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    
    // Variáveis de estado para evitar chamadas repetidas de suspend/resume
    bool is_led_suspended_by_button = false;
    bool is_buzzer_suspended_by_button = false;

    while (1) {
        // --- Lógica para o Botão A (LED) ---
        if (!gpio_get(BUTTON_A_PIN)) { // Se o botão A está pressionado...
            if (!is_led_suspended_by_button) {
                vTaskSuspend(led_task_handle); // Suspende a tarefa do LED
                is_led_suspended_by_button = true;
            }
        } else { // Se o botão A está solto...
            if (is_led_suspended_by_button) {
                vTaskResume(led_task_handle); // Retoma a tarefa do LED
                is_led_suspended_by_button = false;
            }
        }

        // --- Lógica para o Botão B (Buzzer) ---
        if (!gpio_get(BUTTON_B_PIN)) { // Se o botão B está pressionado...
            if (!is_buzzer_suspended_by_button) {
                // Silencia o buzzer IMEDIATAMENTE, antes de suspender a tarefa
                pwm_set_gpio_level(BUZZER_PIN, 0); 
                vTaskSuspend(buzzer_task_handle); // Suspende a tarefa do buzzer
                is_buzzer_suspended_by_button = true;
            }
        } else { // Se o botão B está solto...
            if (is_buzzer_suspended_by_button) {
                vTaskResume(buzzer_task_handle); // Retoma a tarefa do buzzer
                is_buzzer_suspended_by_button = false;
            }
        }
        
        // Pausa a tarefa de verificação para não sobrecarregar a CPU
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
