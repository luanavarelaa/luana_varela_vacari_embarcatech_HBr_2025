#include <stdio.h>                  // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h"            // Biblioteca padrão do Raspberry Pi Pico
#include "inc/ssd1306.h"            // Biblioteca para o display OLED SSD1306
#include "hardware/i2c.h"           // Biblioteca para comunicação I2C
#include "hardware/timer.h"         // Biblioteca para temporizadores
#include "hardware/gpio.h"          // Biblioteca para controle de GPIO

#define BOTAO_A 5       
#define BOTAO_B 6  // Incrementa contador            // Define o pino GPIO 5 como o botão A

volatile bool estado_contagem = false;  // Flag global que controla se a contagem deve iniciar
volatile int contador_b = 0;  
volatile int i = 0; 

absolute_time_t ultimo_clique_A = 0;  // Último tempo de clique no botão B
absolute_time_t ultimo_clique_B = 0;  // Último tempo de clique no botão B

// Função de callback que será chamada na interrupção do botão
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BOTAO_A && (events & GPIO_IRQ_EDGE_FALL)) {
        absolute_time_t agora_A = get_absolute_time();
        // Calcula tempo desde o último clique
        if (absolute_time_diff_us(ultimo_clique_A, agora_A) > 300000) { // 300ms = 300000us
            estado_contagem = true;  // Ativa a flag para iniciar a contagem
            ultimo_clique_A = agora_A;
        }
    }

    if (gpio == BOTAO_B && (events & GPIO_IRQ_EDGE_FALL)) {

        absolute_time_t agora_B = get_absolute_time();
        // Calcula tempo desde o último clique
        if (absolute_time_diff_us(ultimo_clique_B, agora_B) > 300000 && i > 0) { // 300ms = 300000us
            contador_b++; // Incrementa contador quando botão B for pressionado
            ultimo_clique_B = agora_B;
        }
    }
}

// Inicializa o display OLED
void oled_setup() {
    i2c_init(i2c1, 100000);                  // Inicializa o I2C1 com frequência de 100kHz
    gpio_set_function(14, GPIO_FUNC_I2C);    // Define GPIO 14 como função I2C (SDA)
    gpio_set_function(15, GPIO_FUNC_I2C);    // Define GPIO 15 como função I2C (SCL)
    gpio_pull_up(14);                        // Habilita resistor de pull-up no SDA
    gpio_pull_up(15);                        // Habilita resistor de pull-up no SCL
    ssd1306_init();                          // Inicializa o display OLED SSD1306
}

// Limpa completamente o conteúdo do display OLED
void oled_clear() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);  // Calcula o tamanho do buffer
    uint8_t ssd[ssd1306_buffer_length];                // Cria buffer para envio ao display
    memset(ssd, 0, ssd1306_buffer_length);             // Preenche com zeros (limpa a tela)
    render_on_display(ssd, &frame_area);               // Envia buffer limpo para o display
}

// Exibe um texto centralizado no display OLED
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

    int text_width = strlen(text) * 6;                 // 6 pixels por caractere
    int x = (ssd1306_width - text_width) / 2;          // Centraliza horizontalmente
    int y = (ssd1306_height - 8) / 2;                  // Centraliza verticalmente (fonte = 8px)

    ssd1306_draw_string(ssd, x, y, text);              // Desenha texto no buffer
    render_on_display(ssd, &frame_area);               // Atualiza o display com o buffer
}

// Função principal do programa
int main() {
    stdio_init_all();     // Inicializa a comunicação UART (para debug, se necessário)
    oled_setup();         // Configura o display OLED
    oled_clear();         // Limpa o display

    // Configura o botão A (GPIO 5)
    gpio_init(BOTAO_A);                            // Inicializa o pino como GPIO
    gpio_set_dir(BOTAO_A, GPIO_IN);                // Define como entrada
    gpio_pull_up(BOTAO_A);                         // Habilita resistor de pull-up

    // Configura botão B (incrementar contador)
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    // Registra o callback de interrupção para ambos os botões
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);  // BOTAO_B usa o mesmo callback


    while (1) {
        if (estado_contagem) {    // Se a flag for ativada pela interrupção...
            estado_contagem = false; // Reseta a flag para a próxima vez
            contador_b = 0;

            for (i = 9; i >= 0; i--) {  // Contagem regressiva de 9 a 0
                char numero[4];             // Buffer para converter número em texto
                sprintf(numero, "%d", i);   // Converte inteiro para string
                oled_display_text(numero);  // Exibe o número no display
                sleep_ms(1000);             // Espera 1 segundo
            }

            // Quando a contagem chegar a 0, exibe o valor do contador
            char resultado[50];
            sprintf(resultado, "Valor: %d", contador_b);
            oled_display_text(resultado);
        }
    }

    return 0;
}
