//
// Created by Thijs Hulshof
//

#ifndef PCF8574_H
#define PCF8574_H


#define I2C_PORT I2C_NUM_0
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_MASTER_FREQ_HZ    1000000       // I2C frequency
#define PCF8574_ADDR          0x20          // PCF8574 I2C address (0x20 or 0x27)

void i2c_master_init(void);
void i2c_write_byte(uint8_t data);

#endif //PCF8574_H
