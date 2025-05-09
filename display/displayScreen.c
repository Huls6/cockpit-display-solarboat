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

void initDashboardScreen(void) {
    for (int i =0; i < 8; i++) {
        drawText("|",i,35);
        drawText("|",i,98);
    }
    drawEmoji("E",0,108);
    drawText("%",1,120);

    drawText("Temp:", 0, 1);
    drawText("*C",1,23);

    drawText("Temp:",2,1);
    drawText("*C",3,23);

    drawText("Angle:", 4,1);
    drawText("*", 5,23);

    drawText("----------------------------------------------",6,0);
    drawText("I: Initializing",7,0);

    drawNumberLarge("0",0,40);
    drawText("KM/h",2,40);

    drawNumberLarge("0",3,40);
    drawText("Watt",5,40);

}

void infoLine(char* input) {
    drawText(input,7,0);
}

struct displayVariables displayData;

void updateDisplay(void) {
    char temp_c[10];
    sprintf(temp_c,"%0.1f",displayData.motorControllerTemp);
    drawText(temp_c,3,1);

    char temp_c2[10];
    sprintf(temp_c2,"%0.1f",displayData.motorTemp);
    drawText(temp_c2,1,1);

    drawNumber(displayData.foilAngle, 5,1);
    drawNumber(displayData.percentage,1,102);

    char buf[20];
    float power = (-1)*displayData.voltage*displayData.ampere;
    if (power < 100) {
        sprintf(buf, "%0.2f ", power);
    }
    else if (power >= 100 && power < 1000) {
        sprintf(buf, "%0.1f ", power);
    }
    else {
        sprintf(buf, "%d ", (int)power);
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

void getGPSdataTask(void *arg) {
    while (1) {
        gpsData = get_gnss_data();
        vTaskDelay(pdMS_TO_TICKS(250));
    }

}

void checkLTEconnection(void *arg) {
    while (1) {
        sendATCommand("AT+CREG?",buffer);

        char *data_start = strchr(buffer, ':');
        if (data_start == NULL) {

        }
        else {
            data_start++; // skip ':'
            const char* comma = strchr(data_start, ',');
            if (comma != NULL && (*(comma + 1) == '1' || *(comma + 1) == '5')) {
                checkLTE = true;
            }
            else {
                checkLTE = false;
            }
            vTaskDelay(pdMS_TO_TICKS(10000));
        }

    }
}
