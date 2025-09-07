//
// Created by thijs on 5-9-2025.
//

#ifndef A7670E_H
#define A7670E_H

extern char buffer[1024];

extern struct GNSSData gpsData;

void togglePowerA7670E(void);
void initA7670E(void);
void connectTo4G(void);
void initGNSS(void);
struct GNSSData getGNSSData();

#endif //A7670E_H
