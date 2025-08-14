#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card.h"
#include "ff.h"
#include "hardware/gpio.h"
#include "string.h"

// Definição dos pinos SPI conforme sua solicitação
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// Definição dos pinos dos LEDs
#define LED_BLUE_PIN   12
#define LED_GREEN_PIN  11
#define LED_RED_PIN    13

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // Inicializa os pinos dos LEDs
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    // Garante que todos os LEDs estão apagados no início
    gpio_put(LED_BLUE_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_RED_PIN, 0);

    spi_init(SPI_PORT, 1000 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    sd_card_t *sd_card = sd_get_by_num(0);
    FRESULT fr = f_mount(&sd_card->fatfs, sd_card->pcName, 1);
    
    if (fr != FR_OK) {
        // Erro: acende o LED vermelho
        gpio_put(LED_RED_PIN, 1);
        printf("Erro ao montar o sistema de arquivos: %s\n", FRESULT_str(fr));
    } else {
        // Sucesso na inicialização: acende o LED azul
        gpio_put(LED_BLUE_PIN, 1);
        printf("Cartao SD inicializado e montado com sucesso!\n");

        FIL file;
        const char* filename = "dados.csv";
        
        fr = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);

        if (fr == FR_OK) {
            // Escreve os cabeçalhos e alguns dados no formato CSV
            const char* header = "nome,idade,cidade\n";
            const char* data = "Luana,20,Campinas\n";
            
            UINT bytes_written;
            fr = f_write(&file, header, strlen(header), &bytes_written);
            
            if (fr == FR_OK) {
                fr = f_write(&file, data, strlen(data), &bytes_written);
            }

            if (fr == FR_OK) {
                // Sucesso na escrita: apaga o azul, acende o verde
                gpio_put(LED_BLUE_PIN, 0);
                gpio_put(LED_GREEN_PIN, 1);
                printf("Dados gravados em CSV com sucesso!\n");
            } else {
                // Erro na escrita: apaga o azul, acende o vermelho
                gpio_put(LED_BLUE_PIN, 0);
                gpio_put(LED_RED_PIN, 1);
                printf("Erro ao escrever no arquivo: %s\n", FRESULT_str(fr));
            }

            f_close(&file);
            f_unmount(sd_card->pcName);
        } else {
            // Erro ao abrir arquivo: apaga o azul, acende o vermelho
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_RED_PIN, 1);
            printf("Erro ao abrir o arquivo: %s\n", FRESULT_str(fr));
        }
    }

    while (true) {
        tight_loop_contents();
    }
}