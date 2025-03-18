
#ifndef PCF8574_H
#define PCF8574_H

#define I2C_MASTER_SCL_IO    22    // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO    21    // GPIO number for I2C master data
#define I2C_MASTER_NUM        I2C_NUM_0
#define I2C_MASTER_FREQ_HZ    1000000 // I2C frequency
#define PCF8574_ADDR          0x20   // PCF8574 I2C address (0x20 or 0x27)

esp_err_t i2c_master_init();
esp_err_t i2c_write_byte(uint8_t data);

#endif //PCF8574_H
