//
// Created by thijs on 31-3-2025.
//

#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H
#include <stdbool.h>
#include <SIM7000G/sim7000g.h>

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

#endif //DISPLAYSCREEN_H
