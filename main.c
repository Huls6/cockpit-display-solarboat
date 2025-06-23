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

bool prevInit = false;
bool prevGPS = false;
bool prevLTE = false;
bool prevCAN = false;
bool prevAllOK = false;

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
    infoLine("I:Init CAN");
    initCAN();
    xTaskCreate(getCANdataTask, "getCANdataTask", 4096, NULL, 10, NULL);

    //Startup SIM7000G
    infoLine("I:Start SIM");
    initUart2();
    initSIM7000G();

    //Initialize LTE and GPS
    infoLine("I:Init LTE");
    connectToLTE(); //TODO LOGGING SYSTEM AND SEND ALL CAN DATA TO SERVER

    infoLine("I:Init GPS");
    initGPS();

    vTaskDelay(pdMS_TO_TICKS(1000));

    xTaskCreate(getSim7000gData, "getSim7000gData", 4096, NULL, 15, NULL);

    while (1) {
        updateDisplay();

        if(checkGPS) {
            drawText("GPS",4,100);
        }
        else {
            drawText("    ",4,100);
        }
        if(checkLTE) {
            drawText("LTE",5,100);
        }
        else {
            drawText("    ",5,100);
        }
        if(checkCAN) {
            drawText("CAN",6,100);
        }
        else {
            drawText("    ",6,100);
        }

        if (!prevInit) {
            prevGPS = !checkGPS;
            prevLTE = !checkLTE;
            prevCAN = !checkCAN;
            prevInit = true;
        }

        if (checkGPS != prevGPS) {
            if (!checkGPS) {
                infoLine("E:NO GPS");
            }
            prevGPS = checkGPS;
        }
        if (checkLTE != prevLTE) {
            if (!checkLTE) {
                infoLine("E:NO LTE");
            }
            prevLTE = checkLTE;
        }
        if (checkCAN != prevCAN) {
            if (!checkCAN) {
                infoLine("E:NO CAN");
            }
            prevCAN = checkCAN;
        }

        bool currentAllOK = checkGPS && checkLTE && checkCAN;
        if (currentAllOK != prevAllOK) {
            if (currentAllOK) {
                infoLine("I:OK");
            }
            prevAllOK = currentAllOK;
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