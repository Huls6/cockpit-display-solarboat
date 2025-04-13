//
// Created by Thijs Hulshof
//

#ifndef UART2_H
#define UART2_H

#include <log.h>
#include <esp_timer.h>
#include <string.h>
#include "config.h"

#define UART_NUM 2  // Gebruik UART2
#define TX_PIN 27  // ESP32 TX -> SIM7000 RX
#define RX_PIN 26  // ESP32 RX -> SIM7000 TX
#define MAX_BUFLEN 1024

void initUart2(void);
void sendATCommand(const char *cmd, char* response);
void readResponse(char* response);

#endif //UART2_H
