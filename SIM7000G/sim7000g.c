//
// Created by Thijs Hulshof
//

#include "sim7000g.h"

#include <gpio/gpioPins.h>
#include <uart/uart2.h>

#include "freertos/FreeRTOS.h"

void togglePowerSIM7000G(void) {
    gpioWriteOutput(4,1);
    vTaskDelay(pdMS_TO_TICKS(1100));
    gpioWriteOutput(4,0);
    vTaskDelay(pdMS_TO_TICKS(10000));
}

void initSIM7000G(void) {
    sendATCommand("AT+CGNSPWR=1",buffer); // Enable the GNSS (GPS) power.
    sendATCommand("AT+CNMP=2",buffer); // Set the preferred network mode to automatic
    sendATCommand("AT+CFUN=1",buffer); // Set full functionality mode.
    sendATCommand("AT+COPS=0",buffer); // Automatically select operator/network.
}

void connectToLTE(void) {
    sendATCommand("AT+CGDCONT=1,\"IP\",\"portalmmm.nl\"",buffer); // Define a PDP context. Set IP and APN.
    sendATCommand("AT+CGATT=1",buffer); // Attach the device to the GPRS service.
    sendATCommand("AT+CSTT=\"portalmmm.nl\",\"\",\"\"",buffer); // Set up the APN, username, and password.
    sendATCommand("AT+CIICR",buffer); // Bring up the wireless data connection.
}
