//
// Created by Thijs Hulshof
//

#include "driver/i2c.h"
#include "esp_log.h"
#include "display/PCF8574.h"

esp_err_t i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    return i2c_param_config(I2C_MASTER_NUM, &conf) == ESP_OK ? i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0) : ESP_FAIL;
}

esp_err_t i2c_write_byte(uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8574_ADDR << 1) | I2C_MASTER_WRITE, true);  // Send the I2C address with write bit
    i2c_master_write_byte(cmd, data, true);  // Send the data byte (controlling the pins)
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));  // Send the command
    i2c_cmd_link_delete(cmd);
    return ret;
}