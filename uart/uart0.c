//
// Created by Thijs Hulshof
//

#include "uart0.h"

#include <soc/gpio_num.h>

#include "driver/uart.h"

void uart_bridge_task(void *arg) {
    uint8_t data[1024];
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
        // Read from UART0 (ESP-IDF Monitor)
        int lenlocal = uart_read_bytes(UART_NUM_0, data, 1, pdMS_TO_TICKS(10));

        if (lenlocal > 0) {
            // Send to UART2
            uart_write_bytes(UART_NUM_2, (const char *)data, lenlocal);
        }
    }
}