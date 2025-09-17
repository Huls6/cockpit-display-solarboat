//
// Created by thijs on 5-9-2025.
//

#ifndef A7670E_H
#define A7670E_H

#define POWERKEY 4
#define POWERPIN 12
#define RESETPIN 5

extern char buffer[1024];
extern struct displayVariables displayData;
extern struct GNSSData gpsData;

void togglePowerA7670E(void);
void initA7670E(void);

void connectTo4G(void);
void mqttConnect(void);
void sendMqttData(void);
void mqttDisconnect(void);

void initGNSS(void);
struct GNSSData getGNSSData();

#endif //A7670E_H
