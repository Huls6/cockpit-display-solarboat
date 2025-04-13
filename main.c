#include <stdio.h>
#include <string.h>
#include <lwip/sockets.h>

#include "log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <gpio/gpioPins.h>
#include <uart/uart0.h>
#include <uart/uart2.h>

#include <display/display.h>
#include <display/PCF8574.h>
#include <display/displayScreen.h>

#include <SIM7000G/sim7000g.h>

char buffer[1024];

void app_main(void) {

    //Startup
    i2c_master_init();
    initDisplay();
    clearScreen();
    initDashboardScreen();

    initUart2();
    #ifdef DEBUGMODE
        xTaskCreate(uart_bridge_task, "uart_bridge_task", 4096, NULL, 10, NULL);
    #endif

    gpioSetOutputPin(4);
    togglePowerSIM7000G();

    sendATCommand("AT+CPIN=0000",buffer);

    initSIM7000G();
    vTaskDelay(pdMS_TO_TICKS(10000));
    sendATCommand("AT+CGREG?",buffer);
    sendATCommand("AT+CREG?",buffer);
    vTaskDelay(pdMS_TO_TICKS(500));
    connectToLTE();

    while (1) {
        sendATCommand("AT+CGNSINF",buffer); //Geef parameters
        vTaskDelay(pdMS_TO_TICKS(1000));
        drawText(buffer,7,12);
    }

    return;
}

//Send SMS to me (Thijs H)
// sendATCommand("AT+CMGF=1");
// vTaskDelay(pdMS_TO_TICKS(1000));
// sendATCommand("AT+CMGS=\"+31600000000\"");
// vTaskDelay(pdMS_TO_TICKS(1000));
// sendATCommand("Hi Thijs het is gelukt");  // Add \r to signal end of text
// vTaskDelay(pdMS_TO_TICKS(100));
// sendATCommand("\x1A");  // Send Ctrl+Z separately
// vTaskDelay(pdMS_TO_TICKS(3000));  // Wait for modem response

// EXPERIMENTAL
//  LOGI("Set URL\n");
//  //char url[] = "";
//  sendATCommand("AT+SHCONF=URL,");
//  sendATCommand("AT+SHCONF=BODYLEN,1024");
//  sendATCommand("AT+SHCONF=HEADERLEN,350");
//  vTaskDelay(pdMS_TO_TICKS(1000));
//
//  LOGI("Start Connection\n");
//  sendATCommand("AT+SHCONN");
//  vTaskDelay(pdMS_TO_TICKS(10000));
//
//  LOGI("Close Connection\n");
//  sendATCommand("AT+SHDISC");
//  vTaskDelay(pdMS_TO_TICKS(1000));
//
//  // Zet GPS aan
//  LOGI("GPS inschakelen...");
//  sendATCommand("AT+CGNSPWR=1");
//  vTaskDelay(pdMS_TO_TICKS(2000));
//
//  // Vraag GPS-data op
//  LOGI("GPS info opvragen...");
//  sendATCommand("AT+CGNSINF=?");
//  vTaskDelay(pdMS_TO_TICKS(1000));  // Wacht 1 seconde na elke commando-uitvoering
// sendATCommand("AT+CSQ");
// readResponse();
// vTaskDelay(pdMS_TO_TICKS(3000));  // Wacht 1 seconde na elke commando-uitvoering