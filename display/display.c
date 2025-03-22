//
// Created by pi on 2/14/25.
//

#include "display/display.h"

#include <display/PCF8574.h>

#include "gpio/gpioPins.h"

#include <string.h>

// Functie om een commando te sturen

void sendCommand(uint8_t cmd) {
    gpioWriteOutput(RS, 0);  // Commando-modus
    gpioWriteOutput(CS1, 1);
    gpioWriteOutput(CS2, 1);

    i2c_write_byte(cmd);

    gpioWriteOutput(EN, 1);
    usleep(1);
    gpioWriteOutput(EN, 0);
}

// Functie om data te sturen (pixels of tekst)
void sendData(uint8_t data) {
    gpioWriteOutput(RS, 1); // Data-modus

    i2c_write_byte(data);

    gpioWriteOutput(EN, 1);
    usleep(1);
    gpioWriteOutput(EN, 0);
}

// Initialiseer het LCD
void initDisplay() {

    gpioSetOutputPin(RS);
    gpioSetOutputPin(EN);
    gpioSetOutputPin(CS1);
    gpioSetOutputPin(CS2);
    gpioSetOutputPin(RST);

    gpioWriteOutput(RST, 1); // Zet reset aan
    usleep(10000);
    sendCommand(0x003F);  // LCD inschakelen
}

// Scherm wissen
void clearScreen() {
    gpioWriteOutput(CS1, 1);
    gpioWriteOutput(CS2, 1);
    for (int page = 0; page <= DISPLAY_ROWS; page++) {
        sendCommand(0x00B8 | page); // Pagina selecteren
        sendCommand(0x0040);        // Zet cursor aan begin
        for (int col = 0; col < 64; col++) {
            sendData(0x00);
        }
    }
}

//functie om cursor op de plek te zetten.
void setCursor(uint8_t row, uint8_t offset)
{
    #ifdef DISPLAY_UPSIDEDOWN
        row = DISPLAY_ROWS-row;
        offset = DISPLAY_WIDTH-offset;
    #endif
    bool check1 = true;
    bool check2 = false;
    uint8_t column = offset;
    if (row > DISPLAY_ROWS)
    {
        row = DISPLAY_ROWS;
    }
    if (offset > 63 && offset <= DISPLAY_WIDTH)
    {
        #ifdef DISPLAY_UPSIDEDOWN
        #else
        column = offset - 63;
        #endif
        check1 = false;
        check2 = true;
    }
    else if (offset > DISPLAY_WIDTH)
    {
        column = 0;
    }

    sendCommand(0xB8 | row);         // Selecteer een rij
    sendCommand(0x40 | column);      // Zet cursor

    gpioWriteOutput(CS1, check1);
    gpioWriteOutput(CS2, check2);
}


//Simpele functie om tekst weer te geven
void drawText(const char* text, uint8_t row, uint8_t offset) {
    uint8_t cnt = offset;
    size_t size;
    const uint8_t* bitmap = 0;
    for (uint32_t i = 0; i < strlen(text); i++) {
        bitmap = getLetterBitmap(text[i],&size);
        for (size_t y = 0; y < size; y++) {
            setCursor(row, cnt);
            cnt++;
            #ifdef DISPLAY_UPSIDEDOWN
                sendData(reverseBits(bitmap[y]));
            #else
                sendData(bitmap[y]);
            #endif
        }
    }
}

void drawNumber(const uint16_t cnt, uint8_t row, uint8_t offset) {
    char character[10];
    snprintf(character, sizeof(character), "%-3u", cnt);
    drawText(character, row, offset);
}

void drawEmoji(const char* text, uint8_t row, uint8_t offset) {
    size_t size;
    uint8_t cnt = offset;
    const uint8_t* bitmap = 0;
    for (uint32_t i = 0; i < strlen(text); i++) {
        bitmap = getEmojiBitmap(text[i],&size);
        for (size_t y = 0; y < size; y++) {
            setCursor(row, cnt);
            cnt++;
            #ifdef DISPLAY_UPSIDEDOWN
                sendData(reverseBits(bitmap[y]));
            #else
                sendData(bitmap[y]);
            #endif
        }
    }
}

uint8_t reverseBits(uint8_t byte) {
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}



