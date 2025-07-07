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
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

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


project(Tarefa_2_RTOS C CXX ASM)

pico_sdk_init()

add_executable(Tarefa_2_RTOS
    src/main.c
    src/buttons.c
    src/rgb_led.c
    src/reaction_time.c
    src/oled.c
    src/ssd1306_i2c.c
)


target_link_libraries(Tarefa_2_RTOS
        pico_stdlib 
        FreeRTOS-Kernel-Heap4
        hardware_gpio
        pico_stdio_usb
        hardware_i2c
)

target_include_directories(Tarefa_2_RTOS PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
)

pico_enable_stdio_usb(Tarefa_2_RTOS 1)
pico_add_extra_outputs(Tarefa_2_RTOS)
