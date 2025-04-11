#include <stdio.h>              // Biblioteca padrão de entrada e saída
#include "pico/stdlib.h"        // Biblioteca padrão do Raspberry Pi Pico
#include "inc/ssd1306.h"        // Biblioteca para controle do display OLED SSD1306
#include "hardware/i2c.h"       // Biblioteca para comunicação I2C
#include "hardware/timer.h"     // Biblioteca para funções de temporização

// Função para configurar o display OLED
void oled_setup() {
    i2c_init(i2c1, 100000);                     // Inicializa o barramento I2C1 com 100 kHz
    gpio_set_function(14, GPIO_FUNC_I2C);       // Configura o pino 14 como I2C (SDA)
    gpio_set_function(15, GPIO_FUNC_I2C);       // Configura o pino 15 como I2C (SCL)
    gpio_pull_up(14);                           // Habilita resistor de pull-up no pino 14
    gpio_pull_up(15);                           // Habilita resistor de pull-up no pino 15
    ssd1306_init();                             // Inicializa o display OLED
}

// Função para limpar o conteúdo do display
void oled_clear() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);  // Calcula o tamanho do buffer
    uint8_t ssd[ssd1306_buffer_length];                // Cria buffer para a tela
    memset(ssd, 0, ssd1306_buffer_length);             // Zera o buffer (limpa a tela)
    render_on_display(ssd, &frame_area);               // Envia o buffer vazio para o display
}

// Função para exibir um texto centralizado no display OLED
void oled_display_text(const char *text) {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);             // Limpa o buffer antes de desenhar

    int text_width = strlen(text) * 6;                 // Calcula largura do texto (6 pixels por caractere)
    int x = (ssd1306_width - text_width) / 2;          // Centraliza horizontalmente
    int y = (ssd1306_height - 8) / 2;                  // Centraliza verticalmente (altura do texto = 8px)

    ssd1306_draw_string(ssd, x, y, text);              // Desenha o texto no buffer
    render_on_display(ssd, &frame_area);               // Atualiza o display com o buffer
}

// Função principal
int main() {
    stdio_init_all();      // Inicializa comunicação serial padrão
    oled_setup();          // Configura o display
    oled_clear();          // Limpa o display no início

    while (1) {            // Loop infinito
        for (int i = 9; i >= 0; i--) {   // Contagem regressiva de 9 a 0
            char buffer[4];             // Buffer para armazenar número convertido em string
            sprintf(buffer, "%d", i);   // Converte o número inteiro para string
            oled_display_text(buffer);  // Exibe o número no display
            sleep_ms(1000);             // Espera 1 segundo
        }
        sleep_ms(2000);   // Espera 2 segundos antes de recomeçar a contagem
    }

    return 0;  // Nunca será alcançado, mas é boa prática incluir
}
