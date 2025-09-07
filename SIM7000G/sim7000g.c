//
// Created by Thijs Hulshof
//

#include "sim7000g.h"

#include <display/displayScreen.h>
#include <gpio/gpioPins.h>
#include <uart/uart2.h>

#include "freertos/FreeRTOS.h"

void togglePowerSIM7000G(void) {
    gpioWriteOutput(4,0);
    vTaskDelay(pdMS_TO_TICKS(1100));
    gpioWriteOutput(4,1);
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void initSIM7000G(void) {
    gpioSetOutputPin(4);

    //Check if already on
    sendATCommand("AT",buffer);
    if (strcmp(buffer,"\r\nOK\r\n")==0) {
        togglePowerSIM7000G();
    }

    togglePowerSIM7000G();
    sendATCommand(SIMPIN,buffer);
}

void connectToLTE(void) {
    sendATCommand("AT+CNMP=2",buffer); // Set the preferred network mode to automatic
    sendATCommand("AT+CFUN=1",buffer); // Set full functionality mode.
    vTaskDelay(pdMS_TO_TICKS(500));
    sendATCommand("AT+COPS=0",buffer); // Automatically select operator/network.
    vTaskDelay(pdMS_TO_TICKS(2000));
    sendATCommand("AT+CGDCONT=1,\"IP\",\"internet\"",buffer); // Define a PDP context. Set IP and APN.
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendATCommand("AT+CGATT=1",buffer); // Attach the device to the GPRS service.
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendATCommand("AT+CSTT=\"internet\",\"\",\"\"",buffer); // Set up the APN, username, and password.
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendATCommand("AT+CIICR",buffer); // Bring up the wireless data connection.
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void sendDisplayData(float speed, int powerTotal, int powerIn, int powerOut, int battery, int motortemp, int mcutemp, int elevatorangle, double lattitude, double longitude,float rpm, float lowCelVoltage) {

    //sendATCommand("AT+SHCONF=\"URL\",\"http://server.domain.org\"",buffer);
    sendATCommand("AT+SHCONF=\"BODYLEN\",1024",buffer);
    sendATCommand("AT+SHCONF=\"HEADERLEN\",350",buffer);
    sendATCommand("AT+SHCONN",buffer);
    vTaskDelay(pdMS_TO_TICKS(500));
    sendATCommand("AT+SHCHEAD",buffer);
    sendATCommand("AT+SHAHEAD=\"Content-Type\",\"application/x- www-form-urlencoded\"",buffer);
    sendATCommand("AT+SHAHEAD=\"Cache-control\",\"no-cache\"",buffer);
    sendATCommand("AT+SHAHEAD=\"Connection\",\"keep-alive\"",buffer);
    sendATCommand("AT+SHAHEAD=\"Accept\",\"*/*\"",buffer);

    char data[150] = {0};
    sprintf(data,"AT+SHREQ=\"/php/insert_data.php?s=%0.1f&p=%d&pi=%d&po=%d&b=%d&mt=%d&mct=%d&ea=%d&lat=%lf&lon=%lf&rpm=%d&lcv=%f\",3",speed,powerTotal,powerIn,powerOut,battery,motortemp,mcutemp, elevatorangle,lattitude,longitude,(int)rpm,lowCelVoltage);
    sendATCommand(data,buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
    sendATCommand("AT+SHDISC",buffer);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void initGPS(void) {
    sendATCommand("AT+CGNSMOD=1,1,2,2",buffer);                     // Set satellite preference
    sendATCommand("AT+CGNSRTMS=250",buffer);                        // Set sample moment to 4 Hz (250 ms)
    sendATCommand("AT+CGNSHOR=1",buffer);                           // Set positioning desired accuracy
    vTaskDelay(pdMS_TO_TICKS(2000));

    //A-GPS and XTRA for faster fix
    sendATCommand("AT+SAPBR=3,1,\"APN\",\"internet\"", buffer); // Set APN for NTP sync to local
    sendATCommand("AT+SAPBR=1,1", buffer);                          // Open bearer
    sendATCommand("AT+CNTPCID=1", buffer);                          // Set PDP context ID for NTP
    sendATCommand("AT+CNTP=\"pool.ntp.org\",0,1", buffer);          // Set NTP server and timezone offset
    sendATCommand("AT+CNTP", buffer);                               // Sync time via NTP
    vTaskDelay(pdMS_TO_TICKS(2000));

    sendATCommand("AT+CGNSSUPL=1",buffer);                          //Activate A-GPS

    sendATCommand("AT+CNACT=1,\"internet\"", buffer);           // Activate PDP context for XTRA file download
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

    struct GNSSData data = {0,0, 0.0, 0.0, "0.00"};

    char *data_start = strchr(buffer, ':');
    if (data_start == NULL) {
        return data;
    }

    data_start++; // Skip the colon
    int field = 0;
    char *start = data_start;
    char *end;

    while (start && *start != '\0') {
        end = strchr(start, ',');
        char token[32] = {0};

        if (end) {
            size_t len = end - start;
            if (len > 0) {
                strncpy(token, start, len);
                token[len] = '\0';
            }
            start = end + 1;
        } else {
            // Last field (no comma)
            strncpy(token, start, sizeof(token) - 1);
            start = NULL;
        }

        // Process the fields
        switch (field) {
            case 0:
                if (strcmp(token, "0") == 0) {
                    sendATCommand("AT+SGPIO=0,4,1,1", buffer);   // Set GPIO output
                    sendATCommand("AT+CGNSPWR=1", buffer);       // Enable GNSS power
                }
            break;
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
                if(data.fix == 1) {
                    snprintf(data.speed, sizeof(data.speed), "%-6.1f", atof(token));
                } else {
                    snprintf(data.speed, sizeof(data.speed), "0.0");
                }
            break;
        }

        field++;
        if (field > 6) break; // Adjust if you need more fields
    }

    return data;
}
