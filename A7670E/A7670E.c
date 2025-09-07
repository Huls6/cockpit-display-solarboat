
#include "A7670E.h"

#include <display/displayScreen.h>
#include <gpio/gpioPins.h>
#include <uart/uart2.h>

#include "freertos/FreeRTOS.h"

void togglePowerA7670E(void) {
    gpioSetOutputPin(4);
    gpioWriteOutput(4,0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpioWriteOutput(4,1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpioWriteOutput(4,0);

    while (1) {
        sendATCommand("AT",buffer);
        if (strcmp(buffer,"\r\nOK\r\n")==0) {
            break;
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void initA7670E(void) {
    gpioSetOutputPin(12);
    gpioWriteOutput(12,1);

    gpioSetOutputPin(5);
    gpioWriteOutput(5,0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpioWriteOutput(5,1);
    vTaskDelay(pdMS_TO_TICKS(2600));
    gpioWriteOutput(5,0);

    gpioSetOutputPin(25);
    gpioWriteOutput(25,0);

    togglePowerA7670E();

}

void connectTo4G(void) {
    sendATCommand("AT+CNMP=2",buffer); // Set the preferred network mode to automatic
    sendATCommand("AT+CFUN=1",buffer); // Set full functionality mode.
    sendATCommand("AT+COPS=0",buffer); // Automatically select operator/network.
    sendATCommand("AT+CGDCONT=1,\"IP\",\"internet\"",buffer); // Define a PDP context. Set IP and APN.
    sendATCommand("AT+CGACT=1,1",buffer);
    sendATCommand("AT+CGPADDR=1",buffer);
    sendATCommand(SIMPIN,buffer);
    vTaskDelay(pdMS_TO_TICKS(2000));
}

void initGNSS(void) {
    sendATCommand("AT+CGNSSPWR=1",buffer);                      // Turn on
    vTaskDelay(pdMS_TO_TICKS(11000));
    sendATCommand("AT+CAGPS",buffer);                           // A-GPS
    vTaskDelay(pdMS_TO_TICKS(2000));
}

struct GNSSData getGNSSData() {
    sendATCommand("AT+CGNSSINFO", buffer);

    struct GNSSData data = {0, 0,0.0, 0.0, "0.00"};

    char *data_start = strchr(buffer, ':');
    if (data_start == NULL) {
        return data;
    }

    data_start++; // Skip the colon
    int field = 0;
    char *start = data_start;
    char *end;

    while (start && *start != '\0') {
        end = strchr(start, ',');
        char token[32] = {0};

        if (end) {
            size_t len = end - start;
            if (len > 0) {
                strncpy(token, start, len);
                token[len] = '\0';
            }
            start = end + 1;
        } else {
            // Last field (no comma)
            strncpy(token, start, sizeof(token) - 1);
            start = NULL;
        }

        // Process the fields
        switch (field) {
            case 0:
                data.fix = atoi(token);
            break;
            case 1:
                data.satellites = atoi(token);
            break;
            case 5:
                data.latitude = atof(token);
            break;
            case 7:
                data.longitude = atof(token);
            break;
            case 12:
                if(data.fix == 2 || data.fix == 3) {
                    snprintf(data.speed, sizeof(data.speed), "%-6.1f", atof(token));
                } else {
                    snprintf(data.speed, sizeof(data.speed), "0.0");
                }
            break;
        }

        field++;
        if (field > 13) break; // Adjust if you need more fields
    }

    return data;
}