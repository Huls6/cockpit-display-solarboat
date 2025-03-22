//
// Created by Thijs Hulshof
//

#ifndef DISPLAY_H
#define DISPLAY_H
#include <unistd.h>
#include "gpio/gpioPins.h"
#include "display/bitmapDisplay.h"

#define DISPLAY_WIDTH 127
#define DISPLAY_ROWS 7

#define DISPLAY_UPSIDEDOWN // Uncomment to reverse display screen

// GPIO pins
#define RS  32
#define EN  33
#define CS1 18
#define CS2 5
#define RST 25

// Datapins D0-D7 are connected to the io-expander

void sendData(uint8_t data);
void sendCommand(uint8_t cmd);
void initDisplay();
void clearScreen();
void setCursor(uint8_t row, uint8_t offset);
void drawText(const char* text, uint8_t row, uint8_t offset);
void drawNumber(const uint16_t cnt, uint8_t row, uint8_t offset);
void drawEmoji(const char* text, uint8_t row, uint8_t offset);
uint8_t reverseBits(uint8_t byte);


#endif //DISPLAY_H
