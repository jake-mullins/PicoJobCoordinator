cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(PicoJobCoordinator)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# HELLO WORLD
add_executable(hello_world
    src/hello_world.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(hello_world pico_stdlib)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(hello_world)

# BLINK
add_executable(blink
    src/blink.c
)
target_link_libraries(blink pico_stdlib)
pico_add_extra_outputs(blink)

# SERIAL
add_executable(serial
    src/serial.c
)
target_link_libraries(serial pico_stdlib)
pico_enable_stdio_usb(serial 1)
pico_add_extra_outputs(serial)

# MULTICORE
add_executable(multicore
    src/multicore.c
)

target_link_libraries(multicore
    pico_multicore
    pico_stdlib
    hardware_adc
)
pico_enable_stdio_usb(multicore 1)
pico_enable_stdio_uart(multicore 0)
pico_add_extra_outputs(multicore)

# SERVERLESS
add_executable(serverless
    src/serverless.c
)

target_link_libraries(serverless
    pico_multicore
    pico_stdlib
    hardware_adc
)
pico_enable_stdio_usb(serverless 1)
pico_enable_stdio_uart(serverless 0)
pico_add_extra_outputs(serverless)


# PARTITION
add_executable(partition
    src/partition.c
)

target_link_libraries(partition
    pico_multicore
    pico_stdlib
    hardware_adc
)
pico_enable_stdio_usb(partition 1)
pico_enable_stdio_uart(partition 0)
pico_add_extra_outputs(partition)
