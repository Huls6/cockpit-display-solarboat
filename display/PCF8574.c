//
// Created by Thijs Hulshof
//


#include "driver/i2c.h"
#include "esp_log.h"
#include "display/PCF8574.h"
#include "driver/i2c_master.h"

i2c_master_dev_handle_t dev_handle = NULL;

void i2c_master_init(void) {
    // 1. Configure and install the I2C master bus
    i2c_master_bus_handle_t bus_handle = NULL;
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags.enable_internal_pullup = 1,
        .flags.allow_pd = 0,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    // 2. Configure the slave device
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCF8574_ADDR,
        .scl_speed_hz = 100000,
        .scl_wait_us = 0,
        .flags.disable_ack_check = 0,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));
}

void i2c_write_byte(uint8_t data) {
    i2c_master_transmit(dev_handle, &data, sizeof(data), 1000);
}