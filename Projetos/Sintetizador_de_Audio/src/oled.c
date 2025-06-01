// src/oled.c
#include "oled.h"       // Nosso header do módulo OLED
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <string.h>     // Para memset
#include <stdio.h>      // Para printf

// Pinos I2C - Confirme se são estes para a sua BitDogLab
#define OLED_I2C_PORT i2c1
#define OLED_I2C_SDA_PIN 14
#define OLED_I2C_SCL_PIN 15

// As constantes ssd1306_width, ssd1306_height, ssd1306_n_pages, 
// e ssd1306_buffer_length são fornecidas pela sua biblioteca
// através de "ssd1306_i2c.h" (que é incluído por "ssd1306.h").

void oled_init_display() {
    printf("Inicializando I2C para OLED (modulo oled.c)...\n");
    i2c_init(OLED_I2C_PORT, 400 * 1000); // 400 kHz para I2C, comum para OLEDs
    gpio_set_function(OLED_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_I2C_SDA_PIN);
    gpio_pull_up(OLED_I2C_SCL_PIN);

    printf("Inicializando display SSD1306 via ssd1306_init()...\n");
    ssd1306_init(); // Função da sua biblioteca ssd1306.h/ssd1306_i2c.c
    
    oled_clear_display(); // Limpa ao inicializar
    printf("Display OLED inicializado e limpo (modulo oled.c).\n");
}

void oled_clear_display() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area); 
    
    uint8_t buffer[frame_area.buffer_length]; 
    memset(buffer, 0, frame_area.buffer_length); 
    render_on_display(buffer, &frame_area); 
}

void oled_draw_text_centered(const char *text) {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t buffer[frame_area.buffer_length]; 
    memset(buffer, 0, frame_area.buffer_length); 
    
    // Sua biblioteca ssd1306_draw_char avança x em 8.
    int char_width = 8; 
    int char_height = 8; 
    int text_len = strlen(text);
    int text_pixel_width = text_len * char_width;
    
    int x_pos = (ssd1306_width - text_pixel_width) / 2;
    int y_pos = (ssd1306_height - char_height) / 2; 

    if (x_pos < 0) x_pos = 0;
    if (y_pos < 0) y_pos = 0;

    ssd1306_draw_string(buffer, x_pos, y_pos, (char*)text);

    render_on_display(buffer, &frame_area); 
}

// Função para desenhar forma de onda
void oled_display_waveform(const uint16_t* audio_buffer, uint32_t num_samples) {
    if (num_samples == 0) {
        oled_draw_text_centered("Buffer Vazio");
        return;
    }

    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t buffer[frame_area.buffer_length]; 
    memset(buffer, 0, frame_area.buffer_length); // Limpa o buffer do display

    int mid_y = ssd1306_height / 2;     // Linha central vertical do display
    float max_adc_val = 4095.0f;      // Valor máximo de um ADC de 12 bits
    float center_adc_val = max_adc_val / 2.0f; // Ponto central do ADC (offset DC)

    // Desenha uma linha horizontal no centro como referência (opcional)
    // for (int x_ref = 0; x_ref < ssd1306_width; x_ref++) {
    //    ssd1306_set_pixel(buffer, x_ref, mid_y, true); // Se sua lib tiver set_pixel
    // }
    // Ou usando draw_line:
    // ssd1306_draw_line(buffer, 0, mid_y, ssd1306_width - 1, mid_y, true);


    for (int x_pixel = 0; x_pixel < ssd1306_width; ++x_pixel) {
        // Mapeia a coluna de pixel atual para um índice no buffer de áudio.
        // Isso efetivamente faz um sub-sampling se num_samples > ssd1306_width.
        uint32_t sample_index = 0;
        if (num_samples > 1 && ssd1306_width > 1) { // Evita divisão por zero
             sample_index = (uint32_t)((float)x_pixel * (float)(num_samples - 1) / (float)(ssd1306_width - 1));
        } else if (num_samples == 1) {
            sample_index = 0;
        } else { // num_samples == 0, já tratado no início
            continue;
        }
       
        if (sample_index >= num_samples) { // Segurança
            sample_index = num_samples - 1;
        }

        uint16_t adc_val = audio_buffer[sample_index]; 

        // Normaliza a amostra: -1.0 para o mínimo do ADC, +1.0 para o máximo, 0.0 para o centro.
        float normalized_sample = ((float)adc_val - center_adc_val) / center_adc_val;
        
        // Escala a amostra normalizada para a altura do display.
        // (mid_y - 1) para evitar que a forma de onda toque as bordas superior/inferior.
        int line_height_from_center = (int)(normalized_sample * (float)(mid_y - 1)); 
        
        // Calcula a coordenada y final da linha da forma de onda.
        int y_waveform_pixel = mid_y - line_height_from_center;

        // Garante que y_waveform_pixel está dentro dos limites do display.
        if (y_waveform_pixel < 0) y_waveform_pixel = 0;
        if (y_waveform_pixel >= ssd1306_height) y_waveform_pixel = ssd1306_height - 1;

        // Desenha uma linha vertical da linha central (mid_y) até o ponto da amostra (y_waveform_pixel)
        // na coluna x_pixel atual.
        ssd1306_draw_line(buffer, x_pixel, mid_y, x_pixel, y_waveform_pixel, true);
    }

    render_on_display(buffer, &frame_area); 
    printf("Forma de onda exibida no OLED.\n");
}
