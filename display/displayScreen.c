//
// Created by Thijs Hulshof
//

#include "displayScreen.h"

#include <CAN/canData.h>

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

    drawText("Hoek:", 4,1);
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

struct canData can;
struct displayVariables displaydata;

void getCANdataTask(void *arg) {
    while (1) {
        can = getCANData();
        displaydata = CANtoDisplayParser(can.id, can.dlc, can.data);
        if (can.id == 0) {
            checkCAN = false;
        }
        else if(can.id != 0 && checkCAN==false){
            checkCAN = true;
        }
    }
}

void updateDisplay(void) {
    char temp_c[10];
    sprintf(temp_c,"%0.1f",displaydata.MotorTemp);
    drawText(temp_c,1,1);
    drawNumber(displaydata.FoilHoek, 5,1);
    drawNumber(displaydata.Lading,1,102);
    char buf[20];
    float power = displaydata.Voltage*displaydata.Ampere;
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
}