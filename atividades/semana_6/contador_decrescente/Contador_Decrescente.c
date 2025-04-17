#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// Definição dos pinos dos botões
#define BOTAO_A 5
#define BOTAO_B 6

// Variáveis globais para controle da contagem e dos toques
volatile bool contando = false;
int contador = 9; // Inicia a contagem de 9
int contador_b = 0; // Contador para os toques no botão B

// Variáveis para controle do tempo
absolute_time_t ultimo_tempo; 
absolute_time_t ultimo_clique_A = 0;
absolute_time_t ultimo_clique_B = 0;
bool atualiza_display = true;  // Flag para indicar quando o display deve ser atualizado

// Função para tratar o botão A
void tratar_botao_a() {
    absolute_time_t agora = get_absolute_time(); // Obter o tempo atual
    if (absolute_time_diff_us(ultimo_clique_A, agora) > 300000) { // Verifica se o tempo entre cliques é maior que 300ms
        ultimo_clique_A = agora; // Atualiza o tempo do último clique
        contando = true; // Inicia a contagem
        contador = 9; // Reseta a contagem para 9
        contador_b = 0; // Reseta o contador de toques no botão B
        ultimo_tempo = get_absolute_time(); // Reseta o tempo de contagem
        atualiza_display = true; // Indica que o display deve ser atualizado
    }
}

// Função para tratar o botão B
void tratar_botao_b() {
    absolute_time_t agora = get_absolute_time(); // Obter o tempo atual
    if (absolute_time_diff_us(ultimo_clique_B, agora) > 300000) { // Verifica se o tempo entre cliques é maior que 300ms
        ultimo_clique_B = agora; // Atualiza o tempo do último clique
        if (contando && contador > 0) { // Se está contando e o contador for maior que 0
            contador_b++; // Incrementa o contador de toques no botão B
            atualiza_display = true; // Indica que o display deve ser atualizado
        }
    }
}

// Função de callback para as interrupções de GPIO
void gpio_callback(uint gpio, uint32_t events) {
    if ((events & GPIO_IRQ_EDGE_FALL)) { // Verifica se a interrupção foi devido a uma borda de queda (botão pressionado)
        if (gpio == BOTAO_A) { // Se o botão pressionado foi o A, chama a função de tratamento do botão A
            tratar_botao_a();
        } else if (gpio == BOTAO_B) { // Se o botão pressionado foi o B, chama a função de tratamento do botão B
            tratar_botao_b();
        }
    }
}

// Função de configuração do display OLED
void oled_setup() {
    i2c_init(i2c1, 100000); // Inicializa a comunicação I2C
    gpio_set_function(14, GPIO_FUNC_I2C); // Configura os pinos 14 e 15 como I2C
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14); // Habilita o resistor de pull-up no pino 14
    gpio_pull_up(15); // Habilita o resistor de pull-up no pino 15
    ssd1306_init(); // Inicializa o display OLED
}

// Função para exibir a contagem e os toques no display OLED
void oled_display_contagem_e_b(int valor_contagem, int valor_b) {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer de renderização
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer do display

    char linha1[20], linha2[20];
    sprintf(linha1, "Contagem: %d", valor_contagem); // Preenche a primeira linha com o valor da contagem
    sprintf(linha2, "Toques: %d", valor_b); // Preenche a segunda linha com o número de toques no botão B

    int x1 = 0;
    int x2 = 0;
    int y1 = 16; // Posição vertical para a primeira linha
    int y2 = 32; // Posição vertical para a segunda linha

    ssd1306_draw_string(ssd, x1, y1, linha1); // Desenha a primeira linha no display
    ssd1306_draw_string(ssd, x2, y2, linha2); // Desenha a segunda linha no display

    render_on_display(ssd, &frame_area); // Atualiza o display com o novo conteúdo
}

// Função para limpar o display OLED
void oled_clear() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer de renderização
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer do display
    render_on_display(ssd, &frame_area); // Atualiza o display com o conteúdo limpo
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial para depuração
    oled_setup(); // Configura o display OLED
    oled_clear(); // Limpa o display OLED

    // Configuração do pino do botão A
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BOTAO_A); // Habilita o resistor de pull-up no pino A

    // Configuração do pino do botão B
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BOTAO_B); // Habilita o resistor de pull-up no pino B

    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback); // Interrupção para o botão A
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback); // Interrupção para o botão B

    while (1) {
        if (contando) { // Se a contagem está ativa
            if (absolute_time_diff_us(ultimo_tempo, get_absolute_time()) >= 1000000) { // Verifica se passou 1 segundo
                ultimo_tempo = get_absolute_time(); // Atualiza o tempo de contagem
                contador--; // Decrementa o contador
                atualiza_display = true; // Indica que o display deve ser atualizado

                if (contador < 0) { // Se o contador chegou a 0
                    contando = false; // Para a contagem
                    oled_display_contagem_e_b(0, contador_b); // Exibe o resultado final
                    continue;
                }
            }
        }

        if (atualiza_display) { // Se o display precisa ser atualizado
            atualiza_display = false;
            if (contador >= 0) {
                oled_display_contagem_e_b(contador, contador_b); // Atualiza o display com a contagem e o número de toques
            }
        }
    }

    return 0;
}
