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
static int64_t last_trigger_time_us = 0;

void app_main(void) {

#ifdef DEBUGMODE
    xTaskCreate(uart_bridge_task, "uart_bridge_task", 4096, NULL, 10, NULL);
#endif

    //Startup display
    i2c_master_init();
    initDisplay();
    clearScreen();
    initDashboardScreen();
    last_trigger_time_us = esp_timer_get_time();

    //Init CAN-bus
    infoLine("I:Initializing CAN");
    initCAN();
    xTaskCreate(getCANdataTask, "getCANdataTask", 4096, NULL, 10, NULL);

    //Startup SIM7000G
    infoLine("I:Startup SIM7000G");
    initUart2();
    initSIM7000G();

    //Initialize LTE and GPS
    infoLine("I:Initializing LTE    ");
    connectToLTE(); //TODO LOGGING SYSTEM AND SEND ALL CAN DATA TO SERVER

    infoLine("I:Initializing GPS    ");
    initGPS();

    vTaskDelay(pdMS_TO_TICKS(1000));

    xTaskCreate(getSim7000gData, "getSim7000gData", 4096, NULL, 15, NULL);

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

        //Check if the display needs a re-init after time
        int64_t now_us = esp_timer_get_time();
        if ((now_us - last_trigger_time_us) >= WAIT_INTERVAL_US) {
            last_trigger_time_us = now_us;
            clearScreen();
            initDashboardScreen();
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    return;
}