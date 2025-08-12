cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(sd_card_simples C CXX ASM)

pico_sdk_init()

add_executable(sd_card_simples
    sd_card_simples.c
    fatfs/ff.c
    fatfs/diskio.c
    sd_driver/spi.c
    sd_driver/sd_card.c
    sd_driver/hw_config.c
)

pico_set_program_name(sd_card_simples "sd_card_simples")
pico_set_program_version(sd_card_simples "0.1")

pico_enable_stdio_uart(sd_card_simples 0)
pico_enable_stdio_usb(sd_card_simples 1)

# Includes
target_include_directories(sd_card_simples PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/fatfs
    ${CMAKE_CURRENT_LIST_DIR}/sd_driver
)

# Bibliotecas necessárias
target_link_libraries(sd_card_simples
    pico_stdlib
    hardware_spi
    hardware_dma
    sd_driver
)

pico_add_extra_outputs(sd_card_simples)
