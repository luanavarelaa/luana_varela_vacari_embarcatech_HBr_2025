# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

# --- ADIÇÕES E MODIFICAÇÕES PARA O FREERTOS ---

# Define o caminho para a pasta do FreeRTOS 
SET(FREERTOS_PATH ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS)
message("FreeRTOS Kernel located in ${FREERTOS_PATH}")

# Importa o pico-sdk e o FreeRTOS 
include(pico_sdk_import.cmake)
include(${FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# --- FIM DAS ADIÇÕES ---

project(meu_projeto_freertos C CXX ASM)

# Inicializa o Raspberry Pi Pico SDK 
pico_sdk_init()

# MODIFICADO: Aponta para o arquivo src/main.c conforme a estrutura do tutorial 
add_executable(meu_projeto_freertos
        src/main.c
)

# Mantidas as suas configurações de stdio
pico_enable_stdio_uart(meu_projeto_freertos 0)
pico_enable_stdio_usb(meu_projeto_freertos 1)

# MODIFICADO: Adiciona os diretórios de include para encontrar FreeRTOSConfig.h e o src 
target_include_directories(meu_projeto_freertos PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/src
)

# MODIFICADO: Adiciona as bibliotecas do FreeRTOS ao build 
target_link_libraries(meu_projeto_freertos
        pico_stdlib
        FreeRTOS-Kernel-Heap4
)

pico_add_extra_outputs(meu_projeto_freertos)