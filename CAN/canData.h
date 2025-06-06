//
// Created by Thijs Hulshof
//

#ifndef CANDATA_H
#define CANDATA_H

#include "stdio.h"

//CAN-bus ID's
enum CANID { BMS1 = 0x302, BMS2 = 0x402, Foil = 0x360, MotorController1 = 0x601, MotorController2 = 0x481};

//Values of the NTC of the motor.
#define Beta    3892.0
#define R25     2252.0

struct canData {
    uint32_t id;
    uint32_t extd;
    uint8_t dlc;
    uint8_t data[8];
};

struct displayVariables {
    float voltage;
    float currentTotal;
    float currentIn;
    float currentOut;
    float percentage;
    uint8_t foilAngle;
    float motorTemp;
    float motorControllerTemp;
    float rpm;
    float lowCelVoltage;
};

void initCAN();
struct canData getCANData();
struct displayVariables CANtoDisplayParser(enum CANID ID, uint8_t Length, uint8_t Message[]);

#endif //CANDATA_H
