//
// Created by Thijs Hulshof
//

#ifndef CANDATA_H
#define CANDATA_H

#include "stdio.h"

enum CANID { BMS1 = 0x302, BMS2 = 0x402, Foil = 0x360, MotorController = 0x601};

struct canData {
    uint32_t id;
    uint32_t extd;
    uint8_t dlc;
    uint8_t data[8];
};

struct displayVariables {
    float voltage;
    float ampere;
    uint8_t percentage;
    uint8_t foilAngle;
    float motorTemp;
    float speed;
};

void initCAN();
struct canData getCANData();
struct displayVariables CANtoDisplayParser(enum CANID ID, uint8_t Length, uint8_t Message[]);

#endif //CANDATA_H
