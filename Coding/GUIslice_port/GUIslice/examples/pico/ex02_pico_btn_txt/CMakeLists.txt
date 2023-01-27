cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ex02_pico_btn_txt C CXX ASM)
set(CMAKE_C_STANDARD_11)
set(CMAKE_CXX_STANDARD_17)

pico_sdk_init()

set(GUIslice_dir ../../../)
set(SOURCES ex02_pico_btn_txt.c ${GUIslice_dir}/src/GUIslice.c ${GUIslice_dir}/src/GUIslice_drv_pico_vga.c )
set(HEADERS ${GUIslice_dir}/src/GUIslice.h)
include_directories(${GUIslice_dir}/src/)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../src/timing.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../src/rgb.pio)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_pio
    hardware_dma
    pico_multicore
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
