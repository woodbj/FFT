#ifndef HARDWARE_H
#define HARDWARE_H

// Pins
#define I2S_WS 18
#define I2S_SCK 33
#define I2S_SD 34
#define LED_PIN 11
#define ENC_DT 15
#define ENC_CLK 13
#define ENC_SW 14

// Matrix
#define X_TILES 1
#define Y_TILES 1
#define X_LED_PER_TILE 16
#define Y_LED_PER_TILE 16
#define NUM_LED (X_TILES * X_LED_PER_TILE * Y_TILES * Y_LED_PER_TILE)
#define BAND_COUNT (X_TILES * X_LED_PER_TILE)

#endif