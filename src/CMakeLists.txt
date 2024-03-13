add_executable(pico_vision
    main.cpp
)

# Add dependencies
target_link_libraries(pico_vision 
    pico_stdlib
)

# Route stdin/stdout to USB rather than UART
pico_enable_stdio_usb(pico_vision 1)
pico_enable_stdio_uart(pico_vision 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pico_vision)
