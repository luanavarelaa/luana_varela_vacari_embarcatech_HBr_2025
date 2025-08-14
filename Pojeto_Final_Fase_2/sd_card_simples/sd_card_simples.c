#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "sd_card.h"
#include "sd_card.h"
#include "ff.h"
#include "string.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

// Definição dos pinos dos LEDs
#define LED_BLUE_PIN   12
#define LED_GREEN_PIN  11
#define LED_RED_PIN    13

int main() {
    stdio_init_all();
    sleep_ms(2000);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    gpio_put(LED_BLUE_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_RED_PIN, 0);

    // Inicializa o RTC da Pico
    rtc_init();
    
    // Simula a sincronização do RTC com um horário real
    datetime_t t = {
        .year  = 2025,
        .month = 8,
        .day   = 14,
        .dotw  = 4,
        .hour  = 14,
        .min   = 06,
        .sec   = 00
    };
    rtc_set_datetime(&t);
    
    printf("Inicializando cartao SD...\n");
    FRESULT fr = sd_card_init();
    
    if (fr != FR_OK) {
        gpio_put(LED_RED_PIN, 1);
        printf("Erro ao inicializar: %s\n", FRESULT_str(fr));
        while(true);
    } else {
        gpio_put(LED_BLUE_PIN, 1);
        printf("Cartao SD inicializado e montado com sucesso!\n");
    }

    float tensao = 127.0;
    float corrente = 2.5;
    float energia = 0.0;
    const char* status = "OK";
    char csv_data[256];
    const char* filename = "dados.csv";

    while (true) {
        tensao += 0.1;
        corrente += 0.05;
        energia = tensao * corrente;

        char timestamp_buffer[32];
        sd_card_get_formatted_timestamp(timestamp_buffer, sizeof(timestamp_buffer));

        sprintf(csv_data, "%s,%.2f,%.2f,%.2f,%s\n", timestamp_buffer, tensao, corrente, energia, status);
        
        fr = sd_card_append_to_csv(filename, csv_data);

        if (fr == FR_OK) {
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_GREEN_PIN, 1);
            printf("Dados gravados com sucesso!\n");
        } else {
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_RED_PIN, 1);
            printf("Erro ao gravar dados: %s\n", FRESULT_str(fr));
        }
        
        sleep_ms(1000);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_RED_PIN, 0);
        
        sleep_ms(4000);
    }

    sd_card_unmount();
    gpio_put(LED_BLUE_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_RED_PIN, 0);
    printf("\nPrograma encerrado e cartao SD desmontado.\n");
    
    while(true);
}