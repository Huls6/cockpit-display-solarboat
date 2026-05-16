
#include "A7670E.h"

#include <CAN/canData.h>
#include <display/displayScreen.h>
#include <gpio/gpioPins.h>
#include <uart/uart2.h>

#include "freertos/FreeRTOS.h"

void togglePowerA7670E(void) {
    gpioSetOutputPin(POWERKEY);
    gpioWriteOutput(POWERKEY,0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpioWriteOutput(POWERKEY,1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpioWriteOutput(POWERKEY,0);

    while (1) {
        sendATCommand("AT",buffer);
        if (strcmp(buffer,"\r\nOK\r\n")==0) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void initA7670E(void) {
    gpioSetOutputPin(POWERPIN);
    gpioWriteOutput(POWERPIN,1);

    gpioSetOutputPin(RESETPIN);
    gpioWriteOutput(RESETPIN,0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpioWriteOutput(RESETPIN,1);
    vTaskDelay(pdMS_TO_TICKS(2600));
    gpioWriteOutput(RESETPIN,0);

    gpioSetOutputPin(25); // DTR PIN
    gpioWriteOutput(25,1);

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

void mqttConnect(void) {
    sendATCommand("AT+CMQTTSTART",buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    sendATCommand("AT+CMQTTACCQ=0,\"boat\"",buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    sendATCommand("AT+CMQTTWILLTOPIC=0,9",buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    sendATCommand("boat/data", buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    vTaskDelay(pdMS_TO_TICKS(100));
    //sendATCommand("AT+CMQTTCONNECT=0,\"tcp://server.com:port\",60,1,\"user\",\"passwd\"",buffer);
    sendATCommand(LTE_URL,buffer);
    vTaskDelay(pdMS_TO_TICKS(500));
}

// Helper to publish to a topic with given payload
void mqttPublishTopic(const char *topic, const char *payload) {
    char cmd[64];
    int len;

    // Topic
    len = strlen(topic);
    snprintf(cmd, sizeof(cmd), "AT+CMQTTTOPIC=0,%d", len);
    sendATCommand(cmd, buffer);
    vTaskDelay(pdMS_TO_TICKS(50));
    sendATCommand(topic, buffer);
    vTaskDelay(pdMS_TO_TICKS(50));

    // Payload
    len = strlen(payload);
    snprintf(cmd, sizeof(cmd), "AT+CMQTTPAYLOAD=0,%d", len);
    sendATCommand(cmd, buffer);
    vTaskDelay(pdMS_TO_TICKS(50));
    sendATCommand(payload, buffer);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Publish
    snprintf(cmd, sizeof(cmd), "AT+CMQTTPUB=0,1,%d", len);
    sendATCommand(cmd, buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    if (strcmp(buffer, "\r\nOK\r\n") != 0) {
        mqttDisconnect();
        vTaskDelay(pdMS_TO_TICKS(100));
        mqttConnect();
    }
}

void sendMqttData(void) {
    char payload[512];
    snprintf(payload, sizeof(payload),
        "{"
          "\"s\":%.1f,"
          "\"pt\":%d,"
          "\"pi\":%d,"
          "\"po\":%d,"
          "\"b\":%d,"
          "\"lbv\":%.2f,"
          "\"hbv\":%.2f,"
          "\"mt\":%.1f,"
          "\"mct\":%.1f,"
          "\"fa\":%d,"
          "\"lat\":%.6f,"
          "\"lon\":%.6f,"
          "\"rpm\":%.2f"
        "}",
        gpsData.speed,
        (int)((-1) * displayData.voltage * displayData.currentTotal),
        (int)(displayData.voltage * displayData.currentIn),
        (int)(displayData.voltage * displayData.currentOut),
        (int)displayData.percentage,
        displayData.lowCelVoltage,
        displayData.highCelVoltage,
        displayData.motorTemp,
        displayData.motorControllerTemp,
        displayData.foilAngle,
        gpsData.latitude,
        gpsData.longitude,
        displayData.rpm
    );
    mqttPublishTopic("boat/data",payload);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void mqttDisconnect(void) {
    vTaskDelay(pdMS_TO_TICKS(500));
    sendATCommand("AT+CMQTTDISC=0,120",buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    sendATCommand("AT+CMQTTREL=0",buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    sendATCommand("AT+CMQTTSTOP",buffer);
    vTaskDelay(pdMS_TO_TICKS(500));
}

void initGNSS(void) {
    sendATCommand("AT+CGNSSPWR=1",buffer);                      // Turn on
    vTaskDelay(pdMS_TO_TICKS(11000));
    sendATCommand("AT+CAGPS",buffer);                           // A-GPS
    vTaskDelay(pdMS_TO_TICKS(2000));
}

struct GNSSData getGNSSData() {
    sendATCommand("AT+CGNSSINFO", buffer);

    struct GNSSData data = {0, 0,0.0, 0.0, 0};

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
                    data.speed = atof(token)*1.852;
                } else {
                    data.speed = 0;
                }
            break;
        }

        field++;
        if (field > 13) break; // Adjust if you need more fields
    }

    return data;
}