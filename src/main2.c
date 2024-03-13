#include "lib/fonts.h"
#include "lib/st7735.h"
#include "pico/stdlib.h"
#include "pico/time.h"


void classifyPipeline();
void start();
void readImage();
uint8_t classifyImage();
void updateDisplay(uint8_t predictedNumber);

// Timers
struct repeating_timer globalTimer;

int main() {
  // INITIALISE SERIAL IN/OUTPUT
  stdio_init_all();

  // ENABLE WATCHDOG
  // watchdog_enable(WATCHDOG_MILLIS, true);
  // add_repeating_timer_ms(WATCHDOG_MILLIS - 10, monitoringTask, NULL,
  // &monitoringTimer);

  // INITIALISE SCREEN (https://github.com/plaaosert/st7735-guide)
  // ---------------------------------------------------------------------------
  // Disable line and block buffering on stdout (for talking through serial)
  setvbuf(stdout, NULL, _IONBF, 0);
  // Give the Pico some time to think...
  sleep_ms(1000);
  // Initialise the screen
  ST7735_Init();
  ST7735_FillScreen(ST7735_BLACK);

  start();
}

void start() {
  printf("Starting program\n");

  // Timers
  // ---------------------------------------------------------------------------
  const int32_t tick = -16;
  add_repeating_timer_ms(tick, classifyPipeline, NULL, &globalTimer);

  while (true)
    tight_loop_contents();
}

void classifyPipeline() {
  readImage();
  uint8_t prediction = classifyImage();
  updateDisplay(prediction);
}

void readImage() {}
uint8_t classifyImage() {}
void updateDisplay(uint8_t predictedNumber) {}
