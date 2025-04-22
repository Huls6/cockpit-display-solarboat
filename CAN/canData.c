//
// Created by thijs on 15-4-2025.
//

#include "canData.h"

//#include <log.h>
#include <math.h>

#include "esp_log.h"

#include "driver/twai.h"


#define TAG "CAN_RX"

void initCAN() {

    // Configure TWAI (CAN) general settings: RX = GPIO25, TX = GPIO26
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_23, GPIO_NUM_19, TWAI_MODE_LISTEN_ONLY);

    // Set up 250 kbps timing config (adjust as needed)
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();

    // Accept all messages
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install and start the TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG,"TWAI driver installed");
    } else {
        ESP_LOGI(TAG,"Failed to install TWAI driver");
        return;
    }

    if (twai_start() == ESP_OK) {
        ESP_LOGI(TAG,"TWAI driver started and ready to receive");
    } else {
        ESP_LOGI(TAG,"Failed to start TWAI driver");
        return;
    }
}

struct canData getCANData(void) {
    // Wait for the message to be received
    struct canData data = {0};
    twai_message_t message;

    if (twai_receive(&message, pdMS_TO_TICKS(500)) != ESP_OK) {
        return data;
    }
    data.id = message.identifier;
    data.extd = message.extd;
    data.dlc = message.data_length_code;

    if (!(message.rtr)) {
        for (uint16_t i = 0; i < message.data_length_code; i++) {
            data.data[i] = message.data[i];
        }
    }
    return data;
}

struct displayVariables temp={0};

struct displayVariables CANtoDisplayParser(enum CANID ID, uint8_t Length, uint8_t Message[]) {
    switch(ID) {
        case BMS1:
            if (Message[3] == 0x1) {
                temp.Voltage = ((float)(int16_t)(((uint16_t)Message[5] << 8) | Message[4])) / 1000.0f;
            } else if (Message[3] == 0x2) {
                temp.Ampere = ((float)(int16_t)(((uint16_t)Message[5] << 8) | Message[4])) / 100.0f;
            } else if (Message[3] == 0x5) {
                temp.Lading = Message[4];
            }
        break;
        case Foil:
            temp.FoilHoek = Message[1];
        break;
        case MotorController:
            int16_t TP =((uint16_t)Message[0] << 8) | (uint16_t)Message[1];
            printf("TP: %d\n", TP);
            temp.MotorTemp = -178.4+(249*sqrt((float)3416/(4095-TP)-1));
            printf("Temperature: %f\n", temp.MotorTemp);
        break;
        default:
    }
    return temp;
}