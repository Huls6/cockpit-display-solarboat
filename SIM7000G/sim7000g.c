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
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void initSIM7000G(void) {
    gpioSetOutputPin(4);
    togglePowerSIM7000G();
    sendATCommand(SIMPIN,buffer);
}

void connectToLTE(void) {
    sendATCommand("AT+CNMP=2",buffer); // Set the preferred network mode to automatic
    sendATCommand("AT+CFUN=1",buffer); // Set full functionality mode.
    vTaskDelay(pdMS_TO_TICKS(500));
    sendATCommand("AT+COPS=0",buffer); // Automatically select operator/network.
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendATCommand("AT+CGDCONT=1,\"IP\",\"portalmmm.nl\"",buffer); // Define a PDP context. Set IP and APN.
    sendATCommand("AT+CGATT=1",buffer); // Attach the device to the GPRS service.
    sendATCommand("AT+CSTT=\"portalmmm.nl\",\"\",\"\"",buffer); // Set up the APN, username, and password.
    vTaskDelay(pdMS_TO_TICKS(500));
    sendATCommand("AT+CIICR",buffer); // Bring up the wireless data connection.
}

void initGPS(void) {
    sendATCommand("AT+CGNSMOD=1,1,2,2",buffer);                     // Set satellite preference
    sendATCommand("AT+CGNSRTMS=250",buffer);                        // Set sample moment to 4 Hz (250 ms)
    sendATCommand("AT+CGNSHOR=1",buffer);                           // Set positioning desired accuracy

    //A-GPS and XTRA for faster fix
    sendATCommand("AT+SAPBR=3,1,\"APN\",\"portalmmm.nl\"", buffer); // Set APN for NTP sync to local
    sendATCommand("AT+SAPBR=1,1", buffer);                          // Open bearer
    sendATCommand("AT+CNTPCID=1", buffer);                          // Set PDP context ID for NTP
    sendATCommand("AT+CNTP=\"pool.ntp.org\",0,1", buffer);          // Set NTP server and timezone offset
    sendATCommand("AT+CNTP", buffer);                               // Sync time via NTP
    vTaskDelay(pdMS_TO_TICKS(2000));

    sendATCommand("AT+CGNSSUPL=1",buffer);                          //Activate A-GPS

    sendATCommand("AT+CNACT=1,\"portalmmm.nl\"", buffer);           // Activate PDP context for XTRA file download
    vTaskDelay(pdMS_TO_TICKS(500));
    sendATCommand("AT+HTTPTOFSRL?", buffer);                        // Query download status
    sendATCommand("AT+HTTPTOFS=\"http://xtrapath3.izatcloud.net/xtra3grc.bin\",\"/customer/xtra3grc.bin\"", buffer); // Download XTRA file
    vTaskDelay(pdMS_TO_TICKS(5000));
    sendATCommand("AT+HTTPTOFSRL?", buffer);                        // Query download status
    sendATCommand("AT+CGNSCPY", buffer);                            // Copy XTRA file to GNSS engine
    sendATCommand("AT+CGNSXTRA=1", buffer);                         // Enable XTRA assistance

    sendATCommand("AT+CGNSURC=0", buffer);                          // Do not send automatic
    sendATCommand("AT+SGPIO=0,4,1,1",buffer);                       // Set GPIO output
    sendATCommand("AT+CGNSPWR=1",buffer);                           // Enable the GNSS (GPS) power.
    vTaskDelay(pdMS_TO_TICKS(2000));
    sendATCommand("AT+CGNSCOLD", buffer);                           // Perform a cold start (reset GNSS)
}

struct GNSSData get_gnss_data() {
    sendATCommand("AT+CGNSINF", buffer);

    struct GNSSData data = {0, 0.0, 0.0, "0.00"};

    char *data_start = strchr(buffer, ':');
    if (data_start == NULL) {
        return data;
    }

    data_start++; // skip ':'

    // Tokenize fields
    char *token;
    int field = 0;
    char* rest = data_start;

    while ((token = strtok_r(rest, ",", &rest))) {
        switch (field) {
            case 1:
                data.fix = atoi(token);
            break;
            case 3:
                data.latitude = atof(token);
            break;
            case 4:
                data.longitude = atof(token);
            break;
            case 6:
                sprintf(data.speed, "%-6s", token);
            data.speed[sizeof(data.speed) - 1] = '\0';
            break;
            default:
            break;
        }

        field++;
        if (field > 6) break;
    }

    return data;
}
