# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

# ... (Mantenha a seção do VS Code Extension como está) ...
#== DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==#
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Define o caminho para a pasta do FreeRTOS
SET(FREERTOS_PATH ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS)
message("FreeRTOS Kernel located in ${FREERTOS_PATH}")

# Importa o pico-sdk e o FreeRTOS
include(pico_sdk_import.cmake)
include(${FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# --- NOME DO PROJETO ATUALIZADO AQUI ---
project(Tarefa_1_RTOS C CXX ASM)

# Inicializa o Raspberry Pi Pico SDK
pico_sdk_init()

# --- NOME DO EXECUTÁVEL ATUALIZADO ---
add_executable(Tarefa_1_RTOS
        src/main.c
        src/controle_led.c
        src/controle_buzzer.c
        src/controle_botao.c
)

# Adiciona as bibliotecas necessárias
# IMPORTANTE: Adicionamos "hardware_pwm" para o buzzer
# --- NOME DO PROJETO ATUALIZADO ---
target_link_libraries(Tarefa_1_RTOS
        pico_stdlib
        hardware_pwm 
        FreeRTOS-Kernel-Heap4
)

# Adiciona os diretórios de include para o projeto
# --- NOME DO PROJETO ATUALIZADO ---
target_include_directories(Tarefa_1_RTOS PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
)

# --- NOME DO PROJETO ATUALIZADO ---
pico_enable_stdio_usb(Tarefa_1_RTOS 1)
pico_add_extra_outputs(Tarefa_1_RTOS)
