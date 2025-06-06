//
// Created by Thijs Hulshof
//

#include "displayScreen.h"

#include <string.h>
#include <CAN/canData.h>
#include <uart/uart2.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display.h"
#include "config.h"

float movingAvr(void);
void checkLteConnection(void);

void initDashboardScreen(void) {
    for (int i =0; i < 8; i++) {
        drawText("|",i,35);
        drawText("|",i,98);
    }
    drawEmoji("E",0,108);
    drawText("%",1,122);
    drawText("V",2,122);

    drawText("Temp:", 0, 1);
    drawText("*C",1,23);

    drawText("Temp:",2,1);
    drawText("*C",3,23);

    drawText("Angle:", 4,1);

    drawText("----------------------------------------------",6,0);
    drawText("I: Initializing",7,0);

    drawNumberLarge("0.0",0,40);
    drawText("KM/h",2,40);

    drawNumberLarge("0.0",3,40);
    drawText("Watt",5,40);

}

void infoLine(char* input) {
    drawText(input,7,0);
}

struct displayVariables displayData;

float power[samples] = {0};
int sInd =0;

void updateDisplay(void) {
    char temp_c[10];
    sprintf(temp_c,"%0.1f",displayData.motorControllerTemp);
    drawText(temp_c,3,1);

    char temp_c2[10];
    sprintf(temp_c2,"%0.1f",displayData.motorTemp);
    drawText(temp_c2,1,1);

    drawNumber(displayData.foilAngle, 5,1);
    drawNumber(displayData.percentage,1,100);

    char temp_c3[10];
    sprintf(temp_c3,"%0.2f",displayData.lowCelVoltage);
    drawText(temp_c3,2,100);

    char buf[20];
    power[sInd] = displayData.voltage*displayData.currentOut;
    float avrPower = movingAvr();

    if (avrPower < 100 && avrPower > -100) {
        sprintf(buf, "%-4.1f ", avrPower);  // One decimal for small values
    } else {
        sprintf(buf, "%-4.0f ", avrPower);  // No decimal for 100 and above
    }

    drawNumberLarge(buf,3,40);

    drawNumberLarge(gpsData.speed,0,40);
    if (gpsData.fix) {
        checkGPS = true;
    }
    else {
        checkGPS = false;
    }
}

//FREE-RTOS TASKS
void getCANdataTask(void *arg) {
    while (1) {
        can = getCANData();
        displayData = CANtoDisplayParser(can.id, can.dlc, can.data);
        if (can.id == 0) {
            checkCAN = false;
        }
        else if(can.id != 0 && checkCAN==false){
            checkCAN = true;
        }
    }
}

void getSim7000gData(void *arg) {
    int cnt = 0;

    //Check LTE by initialize
    checkLteConnection();

    while (1) {
        // Time start function for consistent delay
        const TickType_t startTime = xTaskGetTickCount();

        gpsData = get_gnss_data();

        // Every second
        if (cnt % 4 == 0) {
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
            sendATCommand("AT+CFUN=1,1",buffer);
        }
    }
}