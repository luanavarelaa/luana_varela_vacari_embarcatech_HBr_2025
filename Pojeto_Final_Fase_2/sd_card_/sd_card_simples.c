#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "sd_card.h"
#include "sd_card.h"
#include "ff.h"
#include "string.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

// Definição dos pinos dos LEDs.
#define LED_BLUE_PIN   12
#define LED_GREEN_PIN  11
#define LED_RED_PIN    13

int main() {
    // Inicializa a comunicação serial.
    stdio_init_all();
    sleep_ms(2000);

    // Configura os pinos dos LEDs como saída.
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    // Apaga todos os LEDs no início.
    gpio_put(LED_BLUE_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_RED_PIN, 0);

    // Inicializa o Relógio de Tempo Real (RTC).
    rtc_init();
    
    // Simula a sincronização do RTC, que no projeto final virá via Wi-Fi.
    datetime_t t = {
        .year  = 2025, .month = 8, .day   = 14, .dotw  = 4,
        .hour  = 14,   .min   = 06, .sec   = 00
    };
    rtc_set_datetime(&t);
    
    printf("Inicializando cartao SD...\n");
    // Tenta inicializar e montar o cartão SD.
    FRESULT fr = sd_card_init();
    
    if (fr != FR_OK) {
        // Se houver erro, acende o LED vermelho e entra em um loop infinito.
        gpio_put(LED_RED_PIN, 1);
        printf("Erro ao inicializar: %s\n", FRESULT_str(fr));
        while(true);
    } else {
        // Se for bem-sucedido, acende o LED azul.
        gpio_put(LED_BLUE_PIN, 1);
        printf("Cartao SD inicializado e montado com sucesso!\n");
    }

    // Variáveis para simular as medições.
    float tensao = 127.0;
    float corrente = 2.5;
    float energia = 0.0;
    const char* status = "OK";
    char csv_data[256];
    const char* filename = "dados.csv";

    // Loop principal onde a lógica de medição e gravação acontece.
    while (true) {
        // Simulação das leituras dos sensores e cálculos.
        tensao += 0.1;
        corrente += 0.05;
        energia = tensao * corrente;

        // Obtém o timestamp formatado do RTC usando a função do módulo.
        char timestamp_buffer[32];
        sd_card_get_formatted_timestamp(timestamp_buffer, sizeof(timestamp_buffer));

        // Formata os dados em uma linha de texto CSV.
        sprintf(csv_data, "%s,%.2f,%.2f,%.2f,%s\n", timestamp_buffer, tensao, corrente, energia, status);
        
        // Chama a função do módulo para gravar os dados.
        fr = sd_card_append_to_csv(filename, csv_data);

        if (fr == FR_OK) {
            // Sucesso na gravação: apaga o azul, acende o verde.
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_GREEN_PIN, 1);
            printf("Dados gravados com sucesso!\n");
        } else {
            // Erro na gravação: apaga o azul, acende o vermelho.
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_RED_PIN, 1);
            printf("Erro ao gravar dados: %s\n", FRESULT_str(fr));
        }
        
        // Delay para indicar sucesso/erro.
        sleep_ms(1000);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_RED_PIN, 0);
        
        // Espera para a próxima medição.
        sleep_ms(4000);
    }

    // Código de desmontagem, que não será alcançado no loop infinito.
    sd_card_unmount();
    gpio_put(LED_BLUE_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_RED_PIN, 0);
    printf("\nPrograma encerrado e cartao SD desmontado.\n");
    
    while(true);
}