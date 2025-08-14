# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(sd_card_simples C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adicionando o subdiretório da biblioteca do cartão SD
add_subdirectory(no-OS-FatFs-SD-SPI-RPi-Pico/FatFs_SPI)

# Adicionando os arquivos de código-fonte necessários para a compilação.
# Inclua seu arquivo 'sd_card_simples.c' e o 'hw_config.c'.
add_executable(sd_card_simples
    sd_card_simples.c
    hw_config.c
    sd_card.c
)

pico_set_program_name(sd_card_simples "sd_card_simples")
pico_set_program_version(sd_card_simples "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(sd_card_simples 0)
pico_enable_stdio_usb(sd_card_simples 1)

# Vinculando a biblioteca padrão e a do cartão SD
target_link_libraries(sd_card_simples
        pico_stdlib
        FatFs_SPI)

# Add the standard include files to the build
target_include_directories(sd_card_simples PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        no-OS-FatFs-SD-SPI-RPi-Pico/FatFs_SPI
)

# A linha vazia 'target_link_libraries' pode ser removida ou comentada.
# target_link_libraries(sd_card_simples 
# 
# )

pico_add_extra_outputs(sd_card_simples)