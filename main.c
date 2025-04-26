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

#include "CAN/canData.h"

bool checkCAN = false;
bool checkGPS = false;
bool checkLTE = false;

struct GNSSData gpsData;
struct canData can;

char buffer[MAX_BUFLEN];

void app_main(void) {

#ifdef DEBUGMODE
    xTaskCreate(uart_bridge_task, "uart_bridge_task", 4096, NULL, 10, NULL);
#endif

    //Startup display
    i2c_master_init();
    initDisplay();
    clearScreen();
    initDashboardScreen();

    //Init CAN-bus
    infoLine("I:Initializing CAN");
    initCAN();
    xTaskCreate(getCANdataTask, "getCANdataTask", 4096, NULL, 10, NULL);

    //Startup SIM7000G
    infoLine("I:Startup SIM7000G");
    initUart2();
    initSIM7000G(); //TODO toggle check if already on

    //Initialize LTE and GPS
    infoLine("I:Initializing LTE    ");
    connectToLTE(); //TODO LOGGING SYSTEM AND SEND TO SERVER + CHECK FOR LTE CONNECTION AFTER SOME TIME

    sendATCommand("AT+CGREG?",buffer);
    sendATCommand("AT+CREG?",buffer);
    if (strcmp(buffer,"+CREG: 0,1")) { //TODO: MAKE THIS WORK BECAUSE strcmp returns 0 when matches
        checkLTE = true;
    }

    infoLine("I:Initializing GPS    ");
    initGPS();
    xTaskCreate(getGPSdataTask, "getGPSdataTask", 4096, NULL, 9, NULL);

    while (1) {
        updateDisplay();
        if(checkGPS) {
            drawText("GPS",3,100);
        }
        else {
            drawText("    ",3,100);
            infoLine("E: NO GPS connection  ");
        }
        if(checkLTE) {
            drawText("LTE",4,100);
        }
        else {
            drawText("    ",4,100);
            infoLine("E: NO LTE connection  ");
        }
        if(checkCAN) {
            drawText("CAN",5,100);
        }
        else {
            drawText("    ",5,100);
            infoLine("E: NO CAN connected  ");
        }
        if(checkCAN && checkGPS && checkLTE) {
            infoLine("I: Nothing on the hand");
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    return;
}
