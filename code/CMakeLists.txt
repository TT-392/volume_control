cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (TARGET tinyusb_device)
# rest of your project
    add_executable(main
        src/main.c
        src/monitor_keys.c
    )

    target_include_directories(main PRIVATE src/)

# Add pico_stdlib library which aggregates commonly used features
    target_link_libraries(main pico_stdlib pico_bootrom pico_multicore)

    pico_enable_stdio_usb(main 1)
    pico_enable_stdio_uart(main 0)

# create map/bin/hex/uf2 file in addition to ELF.
    pico_add_extra_outputs(main)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in
the SDK")
endif()
