//
// Created by Thijs Hulshof
//

#include "displayScreen.h"

#include <string.h>
#include <A7670E/A7670E.h>
#include <CAN/canData.h>
#include <uart/uart2.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display.h"
#include "config.h"

float avgPowerO[samples] = {0};
float avgPowerI[samples] = {0};
float avgSpeed[samples] = {0};

int sInd =0;
int sInd2 =0;
int sInd3 =0;

int prevGPS;
bool prevCAN;
bool prevLTE;
bool checkCAN;
bool checkLTE;

float movingAvg(float values[], int *index, float newValue);
void checkLteConnection(void);

void initDashboardScreen(void) {
    for (int i =0; i < 8; i++) {
        drawText("|",i,35);
        drawText("|",i,98);
    }
    drawEmoji("E",0,108);
    drawText("%",1,122);
    drawText("V",2,122);
    drawText("V",3,122);
    drawText("W",4,122);

    drawText("Temp:", 0, 1);
    drawText("*C",1,23);

    drawText("Temp:",2,1);
    drawText("*C",3,23);

    drawText("Angle:", 4,1);

    drawNumberLarge("0.0",0,40);
    drawText("KM/h",2,40);

    drawNumberLarge("0.0",3,40);
    drawText("Watt",5,40);

    prevGPS = 0;
    prevCAN = false;
    prevLTE = false;
}

void updateDisplay(void) {
    char temp_c1[10];
    sprintf(temp_c1,"%0.1f",displayData.motorControllerTemp);
    drawText(temp_c1,3,1);

    char temp_c2[10];
    sprintf(temp_c2,"%0.1f",displayData.motorTemp);
    drawText(temp_c2,1,1);

    drawNumber(displayData.foilAngle, 5,1);
    drawNumber(displayData.percentage,1,100);

    char temp_c3[10];
    sprintf(temp_c3,"%0.2f",displayData.lowCelVoltage);
    drawText(temp_c3,3,100);

    char temp_c4[10];
    sprintf(temp_c4,"%0.2f",displayData.highCelVoltage);
    drawText(temp_c4,2,100);

    char temp_c5[10];
    avgPowerI[sInd2] = displayData.voltage*displayData.currentIn;
    float avrPowerIn = movingAvg(avgPowerI,&sInd2,displayData.voltage*displayData.currentIn);
    sprintf(temp_c5,"%-4.0f",avrPowerIn);
    drawText(temp_c5,4,100);

    char temp_c6[20];
    float avrPower = movingAvg(avgPowerO,&sInd,displayData.voltage*displayData.currentOut);

    if (avrPower < 100 && avrPower > -100) {
        sprintf(temp_c6, "%-4.1f ", avrPower);  // One decimal for small values
    } else {
        sprintf(temp_c6, "%-4.0f ", avrPower);  // No decimal for 100 and above
    }
    drawNumberLarge(temp_c6,3,40);

    char temp_c7[20];
    float avrSpeed = movingAvg(avgSpeed,&sInd3,gpsData.speed);
    snprintf(temp_c7, sizeof(temp_c7), "%.1f ", avrSpeed);
    drawNumberLarge(temp_c7,0,40);

    //Update flags
    if (gpsData.fix != prevGPS) {
        prevGPS = gpsData.fix;
        if(gpsData.fix == 2 || gpsData.fix == 3) {
            drawText("GPS",5,100);
        }
        else {
            drawText("    ",5,100);
        }
    }

    if (checkCAN != prevCAN) {
        prevCAN = checkCAN;
        if(checkCAN) {
            drawText("CAN",7,100);
        }
        else {
            drawText("    ",7,100);
        }
    }

    if (checkLTE != prevLTE) {
        prevLTE = checkLTE;
        if(checkLTE) {
            drawText("LTE",6,100);
        }
        else {
            drawText("    ",6,100);
        }
    }
}

uint32_t idCounter[100] = {0};

//FREE-RTOS TASKS
void getCANdataTask(void *arg) {
    while (1) {
        can = getCANData();
#ifdef DEBUGMODE
        for (int i = 0; i < 100; i++) {
            if (idCounter[i] == can.id) break;
            if (idCounter[i] == 0) {
                idCounter[i] = can.id;
                LOGI("New ID:%lx", idCounter[i]);
                break;
            }
        }
#endif
        displayData = CANtoDisplayParser(can.id, can.dlc, can.data);
        if (can.id == 0) {
            checkCAN = false;
        }
        else if(can.id != 0 && checkCAN==false){
            checkCAN = true;
        }
    }
}

void getSimData(void *arg) {
    int cnt = 0;

#ifdef A7670E
    connectTo4G();
    initGNSS();
    mqttConnect();
#else
    connectToLTE();
    initGPS();
#endif

    vTaskDelay(pdMS_TO_TICKS(1000));

    //Check LTE by initialize
    checkLteConnection();

    while (1) {
        // Time start function for consistent delay
        const TickType_t startTime = xTaskGetTickCount();

        // Every half second
        if (cnt % 2 == 0) {
#ifdef SIM7000G
            gpsData = get_gnss_data();
#endif
#ifdef A7670E
            gpsData = getGNSSData();
#endif
        }

        // Every second
        if (cnt % 4 == 0) {
#ifdef SIM7000G
            sendDisplayData(
                atof(gpsData.speed),
                ((int)(-1) * displayData.voltage * displayData.currentTotal),
                ((int)displayData.voltage * displayData.currentIn),
                ((int)displayData.voltage * displayData.currentOut),
                displayData.percentage,
                displayData.motorTemp,
                displayData.motorControllerTemp,
                displayData.foilAngle,
                gpsData.latitude,
                gpsData.longitude,
                displayData.rpm,
                displayData.lowCelVoltage
            );
#endif
#ifdef A7670E
        sendMqttData();
#endif
        }

        // Every 10 seconds
        if (cnt % 40 == 0) {
            checkLteConnection();
        }

        // Prevent overflow
        if (++cnt >= 1000) {
            cnt = 0;
        }

        // Delay calculation
        const TickType_t endTime = xTaskGetTickCount();
        const TickType_t elapsedTime = endTime - startTime;

        if (elapsedTime < pdMS_TO_TICKS(250)) {
            vTaskDelay(pdMS_TO_TICKS(250) - elapsedTime);
        }
    }
}

float movingAvg(float values[], int *index, float newValue)
{
    values[*index] = newValue;

    float avg = 0.0f;

    for (int i = 0; i < samples; i++) {
        avg += values[i];
    }

    avg /= samples;

    // Circular decrement
    if (*index > 0) {
        (*index)--;
    } else {
        *index = samples - 1;
    }

    return avg;
}

void checkLteConnection(void) {
    sendATCommand("AT+CREG?", buffer);
    char *data_start = strchr(buffer, ':');
    if (data_start != NULL) {
        data_start++;  // Skip ':'
        const char* comma = strchr(data_start, ',');
        if (comma != NULL && (*(comma + 1) == '1' || *(comma + 1) == '5')) {
            checkLTE = true;
        } else {
            checkLTE = false;
        }
    }
}