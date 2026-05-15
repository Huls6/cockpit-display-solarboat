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
#include <driver/uart.h>
#include <uart/uart2.h>

#ifdef SIM7000G
#include <SIM7000G/sim7000g.h>
#endif

#ifdef A7670E
#include <A7670E/A7670E.h>
#endif

#include "CAN/canData.h"

struct GNSSData gpsData;
struct canData can;
struct displayVariables displayData;

char buffer[1024];
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
    vTaskDelay(pdMS_TO_TICKS(250));

    //Startup SIM7000G
    initUart2();
#ifdef A7670E
    initA7670E();
#else
    initSIM7000G();
#endif

    //Init CAN-bus
    initCAN();
    xTaskCreate(getCANdataTask, "getCANdataTask", 4096, NULL, 10, NULL);

    //Initialize LTE and GPS 
    xTaskCreate(getSimData, "getSimData", 8192, NULL, 15, NULL);

    while (1) {
        updateDisplay();

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