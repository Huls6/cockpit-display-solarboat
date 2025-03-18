#include <esp_log.h>
#include <display/display.h>
#include <display/PCF8574.h>
#include <gpio/gpioPins.h>


#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TX_PIN 27  // ESP32 TX -> SIM7000 RX
#define RX_PIN 26  // ESP32 RX -> SIM7000 TX
#define UART_NUM 2  // Gebruik UART2


static const char *TAG = "SIM7000";

// Functie om AT-commando te verzenden
void sendATCommand(const char *command) {
    const char *cmd = command;
    uart_write_bytes(UART_NUM, cmd, strlen(cmd));
    uart_write_bytes(UART_NUM, "\r\n", 2);  // Stuur CRLF
    vTaskDelay(pdMS_TO_TICKS(100));  // Wacht 100 ms
}

// Functie om een antwoord te lezen met timeout
void readResponse() {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int64_t timeout = esp_timer_get_time() + 3000000;  // 3 sec timeout in microseconden
    while (esp_timer_get_time() < timeout) {
        int len = uart_read_bytes(UART_NUM, (uint8_t *)buffer, sizeof(buffer) - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            buffer[len] = '\0';
            ESP_LOGI(TAG, "Response: %s", buffer);
        }
    }
}

void init() {
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

void app_main(void)
{
    gpioSetOutputPin(4);
    gpioWriteOutput(4,1);
    i2c_master_init();
    initDisplay();
    clearScreen();

    for (int i =0; i < 8; i++) {
        drawText("|",i,30);
        drawText("|",i,98);
    }
    drawEmoji("E",0,108);
    drawText("100%",1,102);

    drawText("Temp:", 0, 1);
    drawText("75*C",1,5);

    drawText("Hoek:", 4,1);
    drawText("10*", 5,5);

    init();
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Start GPS test...");

    // Zet GPS aan
    ESP_LOGI(TAG, "GPS inschakelen...");
    sendATCommand("AT+CGNSPWR=1");
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Vraag GPS-data op
    ESP_LOGI(TAG, "GPS info opvragen...");
    sendATCommand("AT+CGNSINF");
    vTaskDelay(pdMS_TO_TICKS(500));
    readResponse();
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wacht 1 seconde na elke commando-uitvoering
    sendATCommand("AT+CSQ");
    readResponse();
    vTaskDelay(pdMS_TO_TICKS(3000));  // Wacht 1 seconde na elke commando-uitvoering
}


