//
// Created by Thijs Hulshof
//

#include "displayScreen.h"

#include <CAN/canData.h>

#include "display.h"

void initDashboardScreen(void) {
    for (int i =0; i < 8; i++) {
        drawText("|",i,30);
        drawText("|",i,98);
    }
    drawEmoji("E",0,108);
    drawText("%",1,120);

    drawText("Temp:", 0, 1);
    drawText("*C",1,18);

    drawText("Hoek:", 4,1);
    drawText("*", 5,18);

    drawText("----------------------------------------------",6,0);
    drawText("I: Initializing",7,0);

    drawNumberLarge("0",0,35);
    drawText("KM/h",2,35);

    drawNumberLarge("0",3,35);
    drawText("Watt",5,35);

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
        if(can.id == MotorController) {
            printf("ID:0x%x\n",(uint16_t)can.id);
            for(int i=0;i<can.dlc;i++) {
                printf("Data %d:0x%x\n",i,(uint8_t)can.data[i]);
            }
        }
    }
}

void updateDisplay(void) {
    // drawNumber(displaydata.Motortemp,1,1);
    drawNumber(displaydata.FoilHoek, 5,1);
    drawNumber(displaydata.Lading,1,102);
    char buf[20];
    float power = (displaydata.Voltage*displaydata.Ampere);
    if (power < 100) {
        sprintf(buf, "%0.2f ", power);
    }
    else if (power >= 100 && power < 1000) {
        sprintf(buf, "%0.1f ", power);
    }
    else {
        sprintf(buf, "%d ", (int)power);
    }
    drawNumberLarge(buf,3,35);

}