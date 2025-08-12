#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ff.h"   // Biblioteca FatFs
#include "diskio.h"

// Pinos definidos pela sua pinagem
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// Objeto da biblioteca FatFs
FATFS fs;
FIL fil;
FRESULT fr;
UINT bw, br;
char buffer[100];

int main() {
    stdio_init_all();

    // Inicializa SPI1 (pode usar SPI0 se preferir, mas vou seguir SPI1 para não conflitar com pinos padrão)
    spi_init(spi0, 1000 * 1000); // 1 MHz para inicialização
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Configura pino CS
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    sleep_ms(1000);
    printf("Iniciando acesso ao cartão SD...\n");

    // Monta o sistema de arquivos
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        printf("Falha ao montar o cartão SD. Código: %d\n", fr);
        while (1) tight_loop_contents();
    }
    printf("Cartão SD montado com sucesso!\n");

    // Cria arquivo e escreve
    fr = f_open(&fil, "teste.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fr == FR_OK) {
        char *texto = "Ola, Luana! Cartao SD funcionando.\n";
        f_write(&fil, texto, strlen(texto), &bw);
        f_close(&fil);
        printf("Arquivo escrito com sucesso.\n");
    } else {
        printf("Erro ao criar arquivo: %d\n", fr);
    }

    // Lê o arquivo
    fr = f_open(&fil, "teste.txt", FA_READ);
    if (fr == FR_OK) {
        f_read(&fil, buffer, sizeof(buffer)-1, &br);
        buffer[br] = '\0'; // Garante fim de string
        f_close(&fil);
        printf("Conteudo lido: %s\n", buffer);
    } else {
        printf("Erro ao ler arquivo: %d\n", fr);
    }

    while (1) {
        tight_loop_contents();
    }
}
