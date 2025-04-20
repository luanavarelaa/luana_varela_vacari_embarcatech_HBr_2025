#include <stdio.h>
#include <string.h>  // Permite o uso de memset e funções de manipulação de strings
#include <math.h>    // Necessário para funções matemáticas (ex.: sqrt)
#include "pico/stdlib.h"      // Funções básicas do SDK Pico
#include "hardware/adc.h"     // Funções para configuração e uso do ADC
#include "inc/ssd1306.h"      // Funções para operar com o display OLED SSD1306
#include "hardware/i2c.h"     // Funções para comunicação via I2C
#include "hardware/timer.h"   // Funções para configuração de timers e interrupções

// Definições de pinos, canais e parâmetros
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)  // Pino do microfone, calculado a partir do canal
#define SAMPLES 200                 // Número de amostras do ADC a serem coletadas
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f)  // Ajuste para converter valor ADC em tensão relativa
#define FILTRO_MEDIA 5              // Número de valores usados para a média móvel

#define LIMITE_ALERTA_PADRAO 1.0f   // Limite inicial para disparo do alerta
#define LIMITE_RESET 0.8f           // Limite para resetar o alerta

#define LED_VERDE_PIN 11          // Pino do LED verde (estado normal)
#define LED_VERMELHO_PIN 13       // Pino do LED vermelho (alerta)
#define BOTAO_A_PIN 5             // Pino do botão A (diminui limite)
#define BOTAO_B_PIN 6             // Pino do botão B (aumenta limite)

#define REPEATING_TIME 200        // Tempo para debounce (200ms)

//
// Variáveis globais para controle do sistema
//
float limite_alerta = LIMITE_ALERTA_PADRAO; // Valor inicial do limite de alerta
bool mensagem_exibida = false;              // Indica se a mensagem de alerta já foi exibida
float historico[FILTRO_MEDIA] = {0};          // Buffer para armazenar os últimos valores para média móvel
uint8_t indice_historico = 0;               // Índice corrente para o buffer de média móvel
uint16_t adc_buffer[SAMPLES];               // Buffer para armazenar amostras do ADC

// Estruturas para os timers de debounce dos botões
struct repeating_timer botao_timer_A;
struct repeating_timer botao_timer_B;

//
// Funções para configuração do display OLED
//

// Inicializa a interface I2C e o display OLED
void oled_setup() {
    i2c_init(i2c1, 100000);  // Inicializa o barramento I2C com 100 kHz
    gpio_set_function(14, GPIO_FUNC_I2C);  // Configura o pino 14 para a função I2C
    gpio_set_function(15, GPIO_FUNC_I2C);  // Configura o pino 15 para a função I2C
    gpio_pull_up(14);  // Ativa resistor de pull-up no pino 14
    gpio_pull_up(15);  // Ativa resistor de pull-up no pino 15
    ssd1306_init();    // Inicializa o display OLED SSD1306
}

// Limpa o conteúdo do display OLED
void oled_clear() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);  // Calcula o tamanho do buffer de renderização
    uint8_t ssd[ssd1306_buffer_length];  // Cria um buffer local para a imagem a ser renderizada
    memset(ssd, 0, ssd1306_buffer_length);  // Preenche o buffer com zeros (limpa a tela)
    render_on_display(ssd, &frame_area);  // Envia o buffer limpo para o display
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
    memset(ssd, 0, ssd1306_buffer_length);  // Limpa o buffer antes de desenhar
    int text_width = strlen(text) * 6;        // Calcula a largura do texto (assumindo 6 pixels por caractere)
    int x = (ssd1306_width - text_width) / 2;   // Calcula o posicionamento horizontal centralizado
    int y = (ssd1306_height - 8) / 2;           // Calcula o posicionamento vertical centralizado (8 pixels de altura)
    ssd1306_draw_string(ssd, x, y, text);       // Desenha a string no buffer
    render_on_display(ssd, &frame_area);        // Atualiza o display com o conteúdo do buffer
}

// Exibe o valor atual do limite de alerta na parte inferior do display
void oled_display_alert_limit() {
    char alert_limit_text[20];
    snprintf(alert_limit_text, sizeof(alert_limit_text), "Limite: %.2f", limite_alerta);  // Formata o texto com o limite
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = ssd1306_n_pages - 1,  // Posiciona o texto na última página do display
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_draw_string(ssd, 0, 0, alert_limit_text);  // Desenha o texto no buffer
    render_on_display(ssd, &frame_area);                // Atualiza o display com o novo texto
}

//
// Funções de monitoramento e debounce dos botões
//

// Callback para monitorar o botão A (diminui o limite) com debounce
bool verificar_botao_A_callback(struct repeating_timer *t) {
    static absolute_time_t ultima_pressao_A = 0;
    static bool ultimo_estado_botao_A = false;

    bool botao_pressionado = !gpio_get(BOTAO_A_PIN);  // Lê o estado do botão (nível baixo indica pressionado)

    // Se o botão foi pressionado e o debounce expirou
    if (botao_pressionado && !ultimo_estado_botao_A &&
        absolute_time_diff_us(ultima_pressao_A, get_absolute_time()) > 300000) {  // Debounce de 300ms
        ultima_pressao_A = get_absolute_time();
        ultimo_estado_botao_A = true;
        if (limite_alerta > 0.2f) {  // Verifica se ainda é possível diminuir o limite
            limite_alerta -= 0.1f;   // Diminui o limite de alerta
            oled_display_alert_limit();  // Atualiza o display com o novo limite
        }
    } else if (!botao_pressionado) {
        ultimo_estado_botao_A = false;  // Libera o botão quando não está pressionado
    }

    return true;  // Retorna true para manter o timer ativo
}

// Callback para monitorar o botão B (aumenta o limite) com debounce
bool verificar_botao_B_callback(struct repeating_timer *t) {
    static absolute_time_t ultima_pressao_B = 0;
    static bool ultimo_estado_botao_B = false;

    bool botao_pressionado = !gpio_get(BOTAO_B_PIN);  // Lê o estado do botão

    // Se o botão foi pressionado e o debounce expirou
    if (botao_pressionado && !ultimo_estado_botao_B &&
        absolute_time_diff_us(ultima_pressao_B, get_absolute_time()) > 300000) {  // Debounce de 300ms
        ultima_pressao_B = get_absolute_time();
        ultimo_estado_botao_B = true;
        if (limite_alerta < 1.8f) {  // Verifica se ainda é possível aumentar o limite
            limite_alerta += 0.1f;   // Aumenta o limite de alerta
            oled_display_alert_limit();  // Atualiza o display com o novo limite
        }
    } else if (!botao_pressionado) {
        ultimo_estado_botao_B = false;  // Libera o botão quando não está pressionado
    }

    return true;  // Retorna true para manter o timer ativo
}

//
// Funções para captura e processamento do sinal do microfone
//

// Captura amostras do microfone utilizando o ADC e as armazena no buffer
void sample_mic() {
    for (uint i = 0; i < SAMPLES; ++i) {
        adc_buffer[i] = adc_read();  // Lê cada amostra do ADC
    }
}

// Calcula a intensidade média do som usando a média dos quadrados das amostras
float mic_power() {
    float avg = 0.f;
    for (uint i = 0; i < SAMPLES; ++i) {
        avg += adc_buffer[i] * adc_buffer[i];  // Soma o quadrado de cada amostra
    }
    return sqrt(avg / SAMPLES);  // Retorna a raiz quadrada da média, que representa a potência do som
}

// Aplica um filtro de média móvel para suavizar as variações do sinal
float calcular_media_movel(float novo_valor) {
    historico[indice_historico] = novo_valor;         // Armazena o novo valor no buffer
    indice_historico = (indice_historico + 1) % FILTRO_MEDIA;  // Atualiza o índice de forma circular
    float soma = 0.f;
    for (uint8_t i = 0; i < FILTRO_MEDIA; i++) {
        soma += historico[i];  // Soma os valores do buffer
    }
    return soma / FILTRO_MEDIA;  // Retorna a média dos valores
}

//
// Função para atualizar a interface com base na intensidade do som
//
void atualizar_mensagem(float intensidade) {
    // Se a intensidade ultrapassar o limite e a mensagem ainda não foi exibida:
    if (intensidade > limite_alerta && !mensagem_exibida) {
        oled_display_text("Silencio!");  // Exibe a mensagem "Silencio!" no display
        mensagem_exibida = true;
        gpio_put(LED_VERDE_PIN, 0);      // Desliga o LED verde
        gpio_put(LED_VERMELHO_PIN, 1);    // Acende o LED vermelho (indica alerta)
    } 
    // Se a intensidade cair abaixo do limite de reset e a mensagem estiver sendo exibida:
    else if (intensidade < LIMITE_RESET && mensagem_exibida) {
        oled_clear();                    // Limpa o display
        mensagem_exibida = false;
        gpio_put(LED_VERDE_PIN, 1);      // Acende o LED verde (indica estado normal)
        gpio_put(LED_VERMELHO_PIN, 0);    // Desliga o LED vermelho
    }
}

//
// Função principal: inicializa o sistema e entra no loop principal
//
int main() {
    stdio_init_all();  // Inicializa a comunicação serial e outros componentes padrão do Pico
    sleep_ms(5000);    // Aguarda 5 segundos para estabilização (opcional, pode ser usado para debug)

    // Configuração dos LEDs
    gpio_init(LED_VERDE_PIN);
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);
    gpio_init(LED_VERMELHO_PIN);
    gpio_set_dir(LED_VERMELHO_PIN, GPIO_OUT);
    gpio_put(LED_VERDE_PIN, 1);    // LED verde ligado inicialmente (estado normal)
    gpio_put(LED_VERMELHO_PIN, 0);   // LED vermelho desligado inicialmente

    // Configuração dos Botões
    gpio_init(BOTAO_A_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);       // Ativa pull-up para o botão A
    gpio_init(BOTAO_B_PIN);
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_B_PIN);       // Ativa pull-up para o botão B

    // Configuração do ADC para o microfone
    adc_gpio_init(MIC_PIN);          // Associa o pino do microfone ao ADC
    adc_init();                      // Inicializa o módulo ADC
    adc_select_input(MIC_CHANNEL);   // Seleciona o canal de entrada do microfone

    // Inicialização do display OLED
    oled_setup();                    // Configura a interface I2C e inicializa o display
    oled_clear();                    // Limpa o display
    oled_display_alert_limit();      // Exibe o limite de alerta inicial no display

    // Configuração dos temporizadores para debounce dos botões
    add_repeating_timer_ms(100, verificar_botao_A_callback, NULL, &botao_timer_A); // Timer para o botão A
    add_repeating_timer_ms(100, verificar_botao_B_callback, NULL, &botao_timer_B); // Timer para o botão B

    // Loop principal do sistema
    while (true) {
        sample_mic();                          // Captura as amostras do microfone
        float avg = mic_power();                 // Calcula a potência do som
        avg = 2.f * fabs(ADC_ADJUST(avg));       // Ajusta e normaliza o valor do ADC
        float intensidade_filtrada = calcular_media_movel(avg);  // Suaviza o valor com média móvel

        atualizar_mensagem(intensidade_filtrada); // Atualiza a interface com base na intensidade do som
        sleep_ms(100);                            // Pausa para permitir uma resposta mais fluida
    }
}
