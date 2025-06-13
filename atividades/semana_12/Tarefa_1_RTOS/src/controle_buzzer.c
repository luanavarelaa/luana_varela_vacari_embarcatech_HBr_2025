
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Includes do nosso projeto modular
#include "FreeRTOS.h"
#include "task.h"
#include "comuns.h" // Usa o nosso pino BUZZER_PIN (10)

// Configuração da frequência do buzzer (em Hz) - EXATAMENTE COMO NO SEU EXEMPLO
#define BUZZER_FREQUENCY 200

/**
 Definição de uma função para inicializar o PWM no pino do buzzer.
 */
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // CÁLCULO ORIGINAL MANTIDO. Isso implica um wrap de 4095.
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, (float)clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096.0f));
    // É preciso definir o wrap para que o duty cycle funcione corretamente
    pwm_config_set_wrap(&config, 4095);
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(pin, 0); // Inicia com o buzzer desligado
}

/**
  Definição de uma função para emitir um beep com duração especificada.
 * LÓGICA ORIGINAL MANTIDA, com a adaptação CRÍTICA para o RTOS.
 */
void beep(uint pin, uint duration_ms) {
    // Configurar o duty cycle para 50% (ativo) - VALOR ORIGINAL MANTIDO (2048 é 50% de 4096)
    pwm_set_gpio_level(pin, 50);

    // --- ADAPTAÇÃO RTOS ---
    // Substituído sleep_ms(duration_ms) por vTaskDelay.
    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);
}

/**
 Tarefa FreeRTOS que executa a lógica do buzzer.
 */
void buzzer_task(void *params) {
    // Inicializar o PWM no pino do buzzer (chamando sua função)
    pwm_init_buzzer(BUZZER_PIN);

    // Loop infinito da tarefa
    while (true) {
        // Chama a versão adaptada da sua função 'beep'.
        beep(BUZZER_PIN, 500); // Bipe de 500ms

        // A pausa entre os beeps que existia no seu exemplo original (sleep_ms(100)).
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
