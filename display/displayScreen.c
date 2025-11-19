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

float movingAvr(void);
float movingAvr2(void);
float movingAvr3(void);

void checkLteConnection(void);

void initDashboardScreen(void) {
    for (int i =0; i < 8; i++) {
        drawText("|",i,35);
        drawText("|",i,98);
    }
    drawEmoji("E",0,108);
    drawText("%",1,122);
    drawText("V",2,122);
    drawText("W",3,122);

    drawText("Temp:", 0, 1);
    drawText("*C",1,23);

    drawText("Temp:",2,1);
    drawText("*C",3,23);

    drawText("Angle:", 4,1);

    drawText("-------------------",6,0);

    drawNumberLarge("0.0",0,40);
    drawText("KM/h",2,40);

    drawNumberLarge("0.0",3,40);
    drawText("Watt",5,40);

}

void infoLine(char* input) {
    drawText("                  ",7,0);
    drawText(input,7,0);
}

float power[samples] = {0};
float input[samples] = {0};
float avgSpeed[samples] = {0};

int sInd =0;
int sInd2 =0;
int sInd3 =0;

void updateDisplay(void) {
    char temp_c[10];
    sprintf(temp_c,"%0.1f",displayData.motorControllerTemp);
    drawText(temp_c,3,1);

    char temp_c2[10];
    sprintf(temp_c2,"%0.1f",displayData.motorTemp);
    drawText(temp_c2,1,1);

    drawNumber(displayData.foilAngle, 5,1);
    drawNumber(displayData.percentage,1,100);

    char temp_c4[10];
    sprintf(temp_c4,"%0.2f",displayData.lowCelVoltage);
    drawText(temp_c4,2,100);

    char temp_c3[10];
    input[sInd2] = displayData.voltage*displayData.currentIn;
    float avrPowerIn = movingAvr2();
    sprintf(temp_c3,"%-4.0f",avrPowerIn);
    drawText(temp_c3,3,100);

    char temp_c5[20];
    power[sInd] = displayData.voltage*displayData.currentOut;
    float avrPower = movingAvr();

    if (avrPower < 100 && avrPower > -100) {
        sprintf(temp_c5, "%-4.1f ", avrPower);  // One decimal for small values
    } else {
        sprintf(temp_c5, "%-4.0f ", avrPower);  // No decimal for 100 and above
    }
    drawNumberLarge(temp_c5,3,40);

    char temp_c6[20];
    avgSpeed[sInd3] = gpsData.speed;
    float avrSpeed = movingAvr3();
    snprintf(temp_c6, sizeof(temp_c6), "%.1f ", avrSpeed);
    drawNumberLarge(temp_c6,0,40);
    if (gpsData.fix) {
        checkGPS = true;
    }
    else {
        checkGPS = false;
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
        vTaskDelay(pdMS_TO_TICKS(10));
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
        //printf("TIME:%lu\n",elapsedTime);

        if (elapsedTime < pdMS_TO_TICKS(250)) {
            vTaskDelay(pdMS_TO_TICKS(250) - elapsedTime);
        }
    }
}

float movingAvr(void) {
    float Avr = 0;
    for (int i = 0; i < samples; i++) {
        Avr += power[i];
    }
    Avr = Avr/samples;
    if (sInd > 0) {
        sInd--;
    }else {
        sInd = samples-1;
    }
    return Avr;
}

float movingAvr2(void) {
    float Avr = 0;
    for (int i = 0; i < samples; i++) {
        Avr += input[i];
    }
    Avr = Avr/samples;
    if (sInd2 > 0) {
        sInd2--;
    }else {
        sInd2 = samples-1;
    }
    return Avr;
}

float movingAvr3(void) {
    float Avr = 0;
    for (int i = 0; i < samples; i++) {
        Avr += avgSpeed[i];
    }
    Avr = Avr/samples;
    if (sInd3 > 0) {
        sInd3--;
    }else {
        sInd3 = samples-1;
    }
    return Avr;
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