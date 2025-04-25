//
// Created by Thijs Hulshof
//

#ifndef SIM7000G_H
#define SIM7000G_H

extern char buffer[1024];

struct GNSSData{
    int fix;           // 0 = no fix, 1 = fix
    double latitude;
    double longitude;
    char speed[16];
};

void togglePowerSIM7000G(void);
void initSIM7000G(void);
void connectToLTE(void);
void initGPS(void);
struct GNSSData get_gnss_data();

#endif //SIM7000G_H
