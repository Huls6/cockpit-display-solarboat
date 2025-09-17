//
// Created by Thijs Hulshof
//

#ifndef SIM7000G_H
#define SIM7000G_H

extern char buffer[1024];

extern struct GNSSData gpsData;

void togglePowerSIM7000G(void);
void initSIM7000G(void);

void connectToLTE(void);
void sendDisplayData(float speed, int powerTotal, int powerIn, int powerOut, int battery, int motortemp, int mcutemp, int elevatorangle, double lattitude, double longitude, int rpm, float lowCelVoltage);
void initGPS(void);
struct GNSSData get_gnss_data();

#endif //SIM7000G_H
