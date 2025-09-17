//
// Created by Thijs Hulshof
//

#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H
#include <stdbool.h>
#include <SIM7000G/sim7000g.h>


extern bool checkCAN;
extern bool checkGPS;
extern bool checkLTE;

struct GNSSData{
    int fix;           // 2 = 2D fix, 3 = 3D fix
    int satellites;
    double latitude;
    double longitude;
    char speed[16];
};

extern struct canData can;
extern struct displayVariables displayData;

void initDashboardScreen(void);
void infoLine(char* input);
void updateDisplay(void);

void getCANdataTask(void *arg);
void getSimData(void *arg);
#endif //DISPLAYSCREEN_H
