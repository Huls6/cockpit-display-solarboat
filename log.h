//
// Created by Thijs Willemsen
//

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_COLOR_WHITE   "\x1b[0m"
#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"

#define FORMAT(color, level, fmt, ...) \
    printf(color level " (%ld) [%s] %s:%d: " fmt LOG_COLOR_WHITE "\n", \
           esp_log_timestamp(), pcTaskGetName(NULL), __FILE_NAME__, __LINE__, ##__VA_ARGS__)

// Define different log levels
#define LOGD(fmt, ...) FORMAT(LOG_COLOR(LOG_COLOR_WHITE), "D", fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) FORMAT(LOG_COLOR(LOG_COLOR_GREEN), "I", fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) FORMAT(LOG_COLOR(LOG_COLOR_BROWN), "W", fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) FORMAT(LOG_COLOR(LOG_COLOR_RED), "E", fmt, ##__VA_ARGS__)

//Different logs for debugging
#define DEVLOG(fmt, ...) FORMAT(LOG_COLOR(LOG_COLOR_PURPLE), "D", fmt, ##__VA_ARGS__)

//Highlight logs with different colors
#define LOGC(color, fmt, ...) FORMAT(LOG_COLOR(color), "C", fmt, ##__VA_ARGS__)


// Wrappers for existing log functions
#define LOG_HEXDUMP(buffer, len) ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), buffer, len, ESP_LOG_DEBUG)
#define LOG_CHARDUMP(buffer, len) ESP_LOG_BUFFER_CHAR_LEVEL(pcTaskGetName(NULL), buffer, len, ESP_LOG_DEBUG)

#endif //LOG_H
