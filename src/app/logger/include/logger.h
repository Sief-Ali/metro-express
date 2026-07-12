#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} log_level_t;

void Logger_Init(uint32_t baud_rate);

void Logger_Log(
    log_level_t level,
    const char *message);

#endif