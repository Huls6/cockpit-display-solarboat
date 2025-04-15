//
// Created by thijs on 31-3-2025.
//

#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H
#include <stdbool.h>

extern bool checkCAN;
extern bool checkGPS;
extern bool checkLTE;

void initDashboardScreen(void);
void infoLine(char* input);
void getCANdataTask(void *arg);
void updateDisplay(void);

#endif //DISPLAYSCREEN_H
