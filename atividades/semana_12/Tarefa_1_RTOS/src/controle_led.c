#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "comuns.h"
#include "tarefas.h"

/**
 Tarefa para controlar o LED RGB.
 * Alterna entre as cores vermelho, verde e azul a cada 500ms.
 */
void led_task(void *params) {
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    while (1) {
        // Acende o LED Vermelho
        gpio_put(LED_R_PIN, 1);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        // Acende o LED Verde
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 1);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        // Acende o LED Azul
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
