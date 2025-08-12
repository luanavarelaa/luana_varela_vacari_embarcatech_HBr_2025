#include "hw_config.h"
#include "spi.h"

// Definições da sua pinagem
static spi_t spi = {
    .hw_inst = spi0,       // Usando SPI0
    .sck_gpio = 18,
    .mosi_gpio = 19,
    .miso_gpio = 16,
    .baud_rate = 1000 * 1000 // 1 MHz
};

static sd_card_t sd_card = {
    .pcName = "0:",
    .spi = &spi,
    .ss_gpio = 17,
    .use_card_detect = false,
    .card_detect_gpio = 0,
    .card_detected_true = 0
};

void setup_default_sd_config(void) {
    // Registra este cartão para o FatFs
    sd_card_register(&sd_card);
}
