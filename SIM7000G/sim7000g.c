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

void initGPS(void) {
    sendATCommand("AT+CGNSMOD=1,1,2,2",buffer); // Set satellite preference

    sendATCommand("AT+CGNSPWR=1",buffer); // Enable the GNSS (GPS) power.
    sendATCommand("AT+SAPBR=3,1,\"APN\",\"internet\"", buffer);   // Set APN for NTP sync to local
    sendATCommand("AT+SAPBR=1,1", buffer);                     // Open bearer
    sendATCommand("AT+CNTPCID=1", buffer);                     // Set PDP context ID for NTP
    sendATCommand("AT+CNTP=\"pool.ntp.org\",0,1", buffer);     // Set NTP server and timezone offset
    sendATCommand("AT+CNTP", buffer);                          // Sync time via NTP
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendATCommand("AT+CNACT=1,\"internet\"", buffer);           // Activate PDP context for XTRA file download
    sendATCommand("AT+HTTPTOFSRL?", buffer);                   // Query download status
    sendATCommand("AT+HTTPTOFS=\"http://xtrapath1.izatcloud.net/xtra3grc.bin\",\"/customer/xtra3grc.bin\"", buffer); // Download XTRA file
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendATCommand("AT+HTTPTOFSRL?", buffer);                   // Query download status
    sendATCommand("AT+HTTPTOFSRL?", buffer);                   // Query download status
    sendATCommand("AT+CGNSCPY", buffer);                       // Copy XTRA file to GNSS engine
    sendATCommand("AT+CGNSXTRA=1", buffer);                    // Enable XTRA assistance
    sendATCommand("AT+CGNSURC=1", buffer);                     // Send every second
    vTaskDelay(pdMS_TO_TICKS(2000));
    sendATCommand("AT+CGNSCOLD", buffer);                      // Perform a cold start (reset GNSS)
    vTaskDelay(pdMS_TO_TICKS(2000));
    sendATCommand("AT+CGNSCOLD", buffer);
}