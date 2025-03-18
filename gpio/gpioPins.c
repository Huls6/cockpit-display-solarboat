#include "gpio/gpioPins.h"
#include "driver/gpio.h"
#include <stdio.h>

int gpioSetOutputPin(gpio_num_t gpiopin) {
    esp_err_t err = gpio_set_direction(gpiopin, GPIO_MODE_OUTPUT);
    if (err != ESP_OK) {
        printf("Could not set GPIO %d as output.\n", gpiopin);
        return 1;
    }
    return 0;
}

int gpioSetInputPin(gpio_num_t gpiopin) {
    esp_err_t err = gpio_set_direction(gpiopin, GPIO_MODE_INPUT);
    if (err != ESP_OK) {
        printf("Could not set GPIO %d as input.\n", gpiopin);
        return 1;
    }
    return 0;
}

int gpioWriteOutput(gpio_num_t gpiopin, int value) {
    esp_err_t err = gpio_set_level(gpiopin, value);
    if (err != ESP_OK) {
        printf("Could not set GPIO %d to value %d.\n", gpiopin, value);
        return 1;
    }
    return 0;
}
