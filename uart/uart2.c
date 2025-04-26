//
// Created by Thijs Hulshof
//

#include "uart2.h"

#include <log.h>
#include <esp_timer.h>
#include <string.h>

#include "driver/uart.h"

void initUart2(void) {
    // Configureer UART2
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, 1024 * 2, 0, 0, NULL, 0);
}

void sendATCommand(const char *cmd, char* response) {
    uart_flush_input(UART_NUM);
    uart_write_bytes(UART_NUM, cmd, strlen(cmd));
    uart_write_bytes(UART_NUM, "\r\n", 2);  // Stuur CRLF
    vTaskDelay(pdMS_TO_TICKS(10));
    readResponse(response);
}

void readResponse(char* response) {
    int64_t timeout = esp_timer_get_time() + 100000;  // 100 milliseconds timeout
    uint16_t cnt = 0;
    char tmp[2] = {'\0'};
    while (esp_timer_get_time() < timeout) {
        int len = uart_read_bytes(UART_NUM, tmp, 1, pdMS_TO_TICKS(10));
        if (len > 0) {
            response[cnt] = tmp[0];
            cnt++;
        }
        else {
            break;
        }
    }
    response[cnt] = '\0';
    #ifdef DEBUGMODE
        uart_write_bytes(UART_NUM_0, response, strlen(response));
    #endif

    char* rest = response;
    char *token = strtok_r(rest, "\r\n", &rest);  // First token: before \r\n
    token = strtok_r(rest, "", &rest);                  // Second token: after \r\n

    if (token != NULL) {
        strncpy(response, token, MAX_BUFLEN - 1);
    } else {
        response[0] = '\0';
    }
}
