cmake_minimum_required(VERSION 3.12)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(rp2040)

# Initialize the SDK
pico_sdk_init()

add_executable(rp2040
    rp2040.c
    ../../src/st7789.c)

target_include_directories(rp2040 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../src/)

target_link_libraries(rp2040 PRIVATE
    pico_stdlib
    hardware_spi
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(rp2040)