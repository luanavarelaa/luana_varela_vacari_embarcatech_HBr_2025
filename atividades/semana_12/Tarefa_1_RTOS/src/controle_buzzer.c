
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "comuns.h" // Usa o  pino BUZZER_PIN (10)

// Configuração da frequência do buzzer (em Hz) 
#define BUZZER_FREQUENCY 200

/**
 Definição de uma função para inicializar o PWM no pino do buzzer.
 */
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

   
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, (float)clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096.0f));
    // É preciso definir o wrap para que o duty cycle funcione corretamente
    pwm_config_set_wrap(&config, 4095);
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(pin, 0); // Inicia com o buzzer desligado
}

/**
  Definição de uma função para emitir um beep com duração especificada.
 */
void beep(uint pin, uint duration_ms) {
    // Configurar o duty cycle 
    pwm_set_gpio_level(pin, 50);


    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);
}

/**
 Tarefa FreeRTOS que executa a lógica do buzzer.
 */
void buzzer_task(void *params) {
    // Inicializar o PWM no pino do buzzer 
    pwm_init_buzzer(BUZZER_PIN);

    // Loop infinito da tarefa
    while (true) {
        // Chama a versão adaptada da função 'beep'.
        beep(BUZZER_PIN, 500); // Bipe de 500ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
