//
// Created by thijs on 31-3-2025.
//

#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H
#include <stdbool.h>
#include <SIM7000G/sim7000g.h>
#define samples 10

extern bool checkCAN;
extern bool checkGPS;
extern bool checkLTE;
extern struct GNSSData gpsData;
extern struct canData can;

void initDashboardScreen(void);
void infoLine(char* input);
void updateDisplay(void);

void getCANdataTask(void *arg);
void getGPSdataTask(void *arg);
void checkLTEconnection(void *arg);
float movingAvr(void);
#endif //DISPLAYSCREEN_H
