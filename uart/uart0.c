//
// Created by Thijs Hulshof
//

#include "uart0.h"

#include <log.h>
#include "esp_log.h"

#include <soc/gpio_num.h>

#include "uart2.h"
#include "driver/uart.h"

char buffer0[512];
char response[512];
int buf_index = 0;

void uart_bridge_task(void *arg) {
    uint8_t data[10];
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, 1024 * 2, 0, 0, NULL, 0);

    while (1) {
        int lenlocal = uart_read_bytes(UART_NUM_0, data, 1, pdMS_TO_TICKS(10));

        if (lenlocal > 0) {
            char c = (char)data[0];

            if (c == '\r' || c == '\n') {
                // ENTER detected → terminate string
                buffer0[buf_index] = '\0';

                if (buf_index > 0) {
                    // Send full line + CRLF over UART2
                    uart_write_bytes(UART_NUM_2, buffer0, buf_index);
                    uart_write_bytes(UART_NUM_2, "\r\n", 2);

                    LOGI("S: %s", buffer0);
                    buf_index = 0;  // reset buffer

                    vTaskDelay(pdMS_TO_TICKS(100));

                    readResponse(response);
                    LOGI("R: %s", response);
                }
            } else {
                // Store character in buffer
                if (buf_index < sizeof(buffer0) - 1) {
                    buffer0[buf_index++] = c;
                } else {
                    // buffer full, reset to avoid overflow
                    LOGW("Input buffer overflow, resetting.");
                    buf_index = 0;
                }
            }
        }
    }
}