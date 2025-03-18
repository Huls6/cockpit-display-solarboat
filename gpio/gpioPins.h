
#ifndef GPIO_H
#define GPIO_H

#include "driver/gpio.h"
#include <stdio.h>

int gpioSetOutputPin(gpio_num_t gpiopin);
int gpioSetInputPin(gpio_num_t gpiopin);
int gpioWriteOutput(gpio_num_t gpiopin, int value);

#endif //GPIO_H
