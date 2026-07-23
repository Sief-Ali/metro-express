#ifndef LOGGER_H
#define LOGGER_H

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} log_level_t;

/* Initializes the serial logger at the project baud rate. */
void Logger_Init(void);

/* Writes one formatted log line. Invalid levels or NULL messages are ignored. */
void Logger_Log(
    log_level_t level,
    const char *message);

#endif
