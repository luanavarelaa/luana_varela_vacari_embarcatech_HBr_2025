#include <stdio.h>        // Biblioteca padrão de entrada e saída
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040
#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico
#include "inc/ssd1306.h"            // Biblioteca para o display OLED SSD1306
#include "hardware/i2c.h"           // Biblioteca para comunicação I2C
#include <string.h>

// Definição dos pinos usados para o joystick e LEDs
const int vRx = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
const int vRy = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick

// Função para configurar o joystick (pinos de leitura e ADC)
void iniciar_joystick()
{
  adc_init();         // Inicializa o módulo ADC
  adc_gpio_init(vRx); // Configura o pino VRX (eixo X) para entrada ADC
  adc_gpio_init(vRy); // Configura o pino VRY (eixo Y) para entrada ADC
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

// Exibe os valores de X e Y um embaixo do outro no display OLED
void oled_display_xy(uint16_t x_val, uint16_t y_val) {
  struct render_area frame_area = {
      .start_column = 0,
      .end_column = ssd1306_width - 1,
      .start_page = 0,
      .end_page = ssd1306_n_pages - 1
  };
  calculate_render_area_buffer_length(&frame_area);
  uint8_t ssd[ssd1306_buffer_length];
  memset(ssd, 0, ssd1306_buffer_length);  // Limpa o buffer

  char linha1[16], linha2[16];
  sprintf(linha1, "X: %d", x_val);
  sprintf(linha2, "Y: %d", y_val);

  int x1 = (ssd1306_width - strlen(linha1) * 6) / 2; // Centraliza X
  int x2 = (ssd1306_width - strlen(linha2) * 6) / 2; // Centraliza Y

  ssd1306_draw_string(ssd, x1, 16, linha1); // Linha 1 (y=16px)
  ssd1306_draw_string(ssd, x2, 32, linha2); // Linha 2 (y=32px)

  render_on_display(ssd, &frame_area);
}

// Função para ler os valores dos eixos do joystick (X e Y)
void ler_eixos_joystick(uint16_t *eixo_x, uint16_t *eixo_y)
{
  adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
  sleep_us(2);                     // Pequeno delay
  *eixo_x = adc_read();            // Lê valor X

  adc_select_input(ADC_CHANNEL_1); // Canal ADC para Y
  sleep_us(2);
  *eixo_y = adc_read();            // Lê valor Y
}

// Função principal
int main()
{
  stdio_init_all();     // Inicializa UART
  oled_setup();         // Configura OLED
  oled_clear();         // Limpa tela
  iniciar_joystick();   // Configura joystick

  uint16_t valor_x, valor_y;

  while (1)
  {
    ler_eixos_joystick(&valor_x, &valor_y);
    oled_display_xy(valor_x, valor_y);  // Mostra X e Y em duas linhas
    sleep_ms(100);
  }
}
