add_executable(pico_vision
    arducam/arducam.c
    main.c
    lib/mnist.c
    # Pico4ML Display
    lib/fonts.c
    lib/st7735.c
    lib/DEV_Config.c
)

pico_generate_pio_header(pico_vision ${CMAKE_CURRENT_LIST_DIR}/image.pio)

# Add dependencies
target_link_libraries(pico_vision
    pico_stdlib
	hardware_dma
    hardware_pwm
    hardware_pio
    hardware_spi
    hardware_i2c
    pico_time
	pico_multicore
)

# Route stdin/stdout to USB rather than UART
pico_enable_stdio_usb(pico_vision 1)
pico_enable_stdio_uart(pico_vision 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pico_vision)
