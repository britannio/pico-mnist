#include <stdio.h>
#include "pico/stdlib.h"
#include <tusb.h>
#include "pico/multicore.h"
#include "arducam/arducam.h"
#include "lib/st7735.h"
#include "lib/fonts.h"
#include "lib/mnist.h"
#include "hardware/vreg.h"

// Core 0
uint8_t core_0_image_buf[324 * 324];
uint8_t prediction_img_buf[28 * 28 * 2];
int8_t mnist_prediction = -1;


// Core 1
uint8_t core_1_image_buf[324 * 324];
uint8_t imageFeedBuf[80 * 80 * 2];


uint8_t header[2] = {0x55, 0xAA};

#define FLAG_VALUE 123

#define FLAG_COPY_IMG 0
#define FLAG_DISPLAY_RES 1

void create_arducam_config(struct arducam_config *config) {
    config->sccb = i2c0;
    config->sccb_mode = I2C_MODE_16_8;
    config->sensor_address = 0x24;
    config->pin_sioc = PIN_CAM_SIOC;
    config->pin_siod = PIN_CAM_SIOD;
    config->pin_resetb = PIN_CAM_RESETB;
    config->pin_xclk = PIN_CAM_XCLK;
    config->pin_vsync = PIN_CAM_VSYNC;
    config->pin_y2_pio_base = PIN_CAM_Y2_PIO_BASE;

    config->pio = pio0;
    config->pio_sm = 0;

    config->dma_channel = 0;
    config->image_buf = core_1_image_buf;
    config->image_buf_size = sizeof(core_1_image_buf);
}

void core1_entry() {
    // Initialisation
    multicore_fifo_push_blocking(FLAG_VALUE);
    uint32_t g = multicore_fifo_pop_blocking();
    if (g != FLAG_VALUE) printf("Hmm, that's not right on core 1!\n");
    else printf("It's all gone well on core 1!\n");

    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    ST7735_Init();
    ST7735_DrawImage(0, 0, 80, 160, arducam_logo);

    struct arducam_config config;
    create_arducam_config(&config);
    arducam_init(&config);

    // Clear the screen
    ST7735_FillScreen(0);

    while (true) {
        gpio_put(PIN_LED, !gpio_get(PIN_LED));
        // Writes to core_1_image_buf
        arducam_capture_frame(&config);

        bool display_result = false;

        // Multicore
        if (multicore_fifo_rvalid()) {
            multicore_fifo_pop_blocking();
            // Core 0 would like us to make a copy of core_1_image_buf that it can safely read
            memcpy(&core_0_image_buf, &core_1_image_buf, sizeof(core_1_image_buf));
            display_result = true;
        }

        // Read from the 324x324 frame and fill imageFeedBuf with a 80x80 RBG copy of the image.
        uint16_t index = 0;
        for (int y = 0; y < 80; y++) {
            for (int x = 0; x < 80; x++) {
                long i = (2 + 160 - 2 * y) * 324 + (2 + 40 + 2 * x);
                uint8_t c = core_1_image_buf[i];
                uint16_t imageRGB = ST7735_COLOR565(c, c, c);
                imageFeedBuf[index++] = (uint8_t) (imageRGB >> 8) & 0xFF;
                imageFeedBuf[index++] = (uint8_t) (imageRGB) & 0xFF;
            }
        }

        // Display the image feed.
        ST7735_DrawImage(0, 0, 80, 80, imageFeedBuf);

        // TODO Display the RGB image LTWH(6,26,28,28)
        // TODO Display the prediction (x>=40...)
        if (display_result) {
            // Display the prediction image
            ST7735_DrawImage(6, 80 + 26, 28, 28, prediction_img_buf);
            // Display the current prediction
            ST7735_FillRectangle(80 - 6 - 28, 80 + 26, 28, 28, 0XC0);
        }
    }
}

void core0_entry() {
    while (true) {
        // Request a copy of the image from core 1.
        // Once it completes, the full size image will be available in core_0_image_buf
        // This is not time sensitive unlike the realtime updates to the camera feed on core 1.
        multicore_fifo_push_blocking(0);



        // TODO Resize to 28x28 RGB to be displayed
//        prediction_img_buf
        int index = 0;

        for (int y = 0; y < 28; y++) {
            for (int x = 0; x < 28; x++) {

//                long i = (2 + 56 - 2 * y) * 324 + (2 + 14 + 2 * x);
                int x_scale_factor = 11;
                int y_scale_factor = 11;
                long i = (y_scale_factor + (28 * y_scale_factor) - y_scale_factor * y) * 324 + (x_scale_factor + (28 * x_scale_factor) + x_scale_factor * x);
                uint8_t c = core_0_image_buf[i];
                uint16_t imageRGB = ST7735_COLOR565(c, c, c);
                prediction_img_buf[index++] = (uint8_t) (imageRGB >> 8) & 0xFF;
                prediction_img_buf[index++] = (uint8_t) (imageRGB) & 0xFF;
            }
        }
//        for (int y = 0; y < 28; y++) {
//            for (int x = 0; x < 28; x++) {
////                Reference
////                long i = (2 + 160 - 2 * y) * 324 + (2 + 40 + 2 * x);
//
////                long i = (2 + (56 * 2) - 2 * y) * 324 + (2 + 28 + 2 * x);
////                long i = (2 + 160 - 2 * y) * 324 + (2 + 40 + 2 * x);
//
//                int x_scaled = x * 11;
//                int y_scaled = y * 11;
//                long i = (324 * y_scaled) + x_scaled;
//                uint8_t c = core_0_image_buf[i];
//                uint16_t imageRGB = ST7735_COLOR565(c, c, c);
//                prediction_img_buf[index++] = (uint8_t) (imageRGB >> 8) & 0xFF;
//                prediction_img_buf[index++] = (uint8_t) (imageRGB) & 0xFF;
//            }
//        }


        // TODO Resize to [28*28] 'tensor' to feed into `net()`
        // TODO Feed tensor into `net()` to get the prediction
        // TODO Print the prediction
    }
}


int main() {
    int loops = 20;
    stdio_init_all();
    while (!tud_cdc_connected()) {
        sleep_ms(100);
        if (--loops == 0)
            break;
    }
    printf("loops(%d)\n", loops);

    printf("tud_cdc_connected(%d)\n", tud_cdc_connected() ? 1 : 0);

    vreg_set_voltage(VREG_VOLTAGE_1_30);
    sleep_ms(1000);
    // Overclock from 125Mhz to 250Mhz
    set_sys_clock_khz(250000, true);

    multicore_launch_core1(core1_entry);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 0!\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("It's all gone well on core 0!\n");
    }

    core0_entry();
}
