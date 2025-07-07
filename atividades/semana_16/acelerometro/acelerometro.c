/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * SDA: GP14
 * SCL: GP15
 * Porta I2C: i2c1
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// Endereço I2C do dispositivo MPU6050/MPU6500
static int addr = 0x68;

// --- DEFINIÇÃO DOS PINOS I2C ---
#define I2C_PORT i2c1
#define I2C_SDA_PIN 2 // i2c 1 (sem ser o oled)
#define I2C_SCL_PIN 3 // i2c 1 (sem ser o oled)
// --------------------------------

// Função para resetar o sensor e tirá-lo do modo de repouso
static void mpu6050_reset() {
    uint8_t buf[] = {0x6B, 0x80}; // Reset do dispositivo
    i2c_write_blocking(I2C_PORT, addr, buf, 2, false); // ALTERADO: Usa a porta I2C definida
    sleep_ms(100); // Aguarda o reset estabilizar

    buf[1] = 0x00; // Limpa o modo de repouso
    i2c_write_blocking(I2C_PORT, addr, buf, 2, false); // ALTERADO: Usa a porta I2C definida
    sleep_ms(10); // Aguarda estabilizar
}

// Função para ler os dados brutos de todos os sensores
static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    uint8_t buffer[6];

    // Lê 6 bytes a partir do registrador 0x3B (Acelerômetro)
    uint8_t val = 0x3B;
    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);   // ALTERADO
    i2c_read_blocking(I2C_PORT, addr, buffer, 6, false); // ALTERADO

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Lê 6 bytes a partir do registrador 0x43 (Giroscópio)
    val = 0x43;
    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);   // ALTERADO
    i2c_read_blocking(I2C_PORT, addr, buffer, 6, false); // ALTERADO

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Lê 2 bytes a partir do registrador 0x41 (Temperatura)
    val = 0x41;
    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);   // ALTERADO
    i2c_read_blocking(I2C_PORT, addr, buffer, 2, false); // ALTERADO

    *temp = buffer[0] << 8 | buffer[1];
}

int main() {
    stdio_init_all();
    sleep_ms(2000); // Um pequeno atraso para dar tempo de abrir o monitor serial
    
    printf("Iniciando MPU6050 com pinos customizados...\n");
    printf("SDA no pino %d, SCL no pino %d\n", I2C_SDA_PIN, I2C_SCL_PIN);

    // --- INICIALIZAÇÃO DO I2C COM OS PINOS DEFINIDOS ---
    i2c_init(I2C_PORT, 400 * 1000); // ALTERADO: Usa a porta I2C definida
    
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C); // ALTERADO
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C); // ALTERADO
    
    gpio_pull_up(I2C_SDA_PIN); // ALTERADO
    gpio_pull_up(I2C_SCL_PIN); // ALTERADO
    
    // Adiciona informação dos pinos para depuração (opcional)
    bi_decl(bi_2pins_with_func(I2C_SDA_PIN, I2C_SCL_PIN, GPIO_FUNC_I2C));
    // ----------------------------------------------------

    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;

    while (1) {
        mpu6050_read_raw(acceleration, gyro, &temp);

        // Imprime os valores brutos lidos do chip
        printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        
        // Converte e imprime a temperatura usando a fórmula para o MPU-6050
        printf("Temp. = %.2f C\n", (temp / 340.0) + 36.53);
        printf("--------------------------------------\n");

        sleep_ms(1000); // Aumentei o tempo de espera para facilitar a leitura
    }

    return 0;
}