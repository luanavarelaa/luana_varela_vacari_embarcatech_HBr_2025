# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
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

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

project(teste_unitario C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(teste_unitario 
    teste/teste_adc.c
    src/adc.c
    unity/unity.c
)

pico_set_program_name(teste_unitario "teste_unitario")
pico_set_program_version(teste_unitario "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(teste_unitario 0)
pico_enable_stdio_usb(teste_unitario 1)

# pico_stdlib e pico_stdio_usb
target_link_libraries(teste_unitario
    pico_stdlib
    pico_stdio_usb   
)

target_include_directories(teste_unitario PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/unity
)

# Deixa essa seção limpa se não precisar de mais libs
# target_link_libraries(teste_unitario)

pico_add_extra_outputs(teste_unitario)
