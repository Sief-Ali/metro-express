#include "logger.h"

#include <stddef.h>

#include "serial.h"

/* UART prefixes used by the project logger for each severity level. */
static const char *log_level_prefix[] =
{
    [LOG_DEBUG]   = "[D]",
    [LOG_INFO]    = "[I]",
    [LOG_WARNING] = "[W]",
    [LOG_ERROR]   = "[E]"
};

/* Initializes the serial layer used by the logger. */
void Logger_Init(void)
{
    Serial_Init(9600);
}

/* 
  logging style:
    [timestamp]:[level/type]:[error_code]:[message]
  example: 
    [00000000]:[I]:[0x00]:SYS_INIT_OK
    [00000000]:[E]:[0x12]:ADC_TIMEOUT

*/
/* Writes one formatted log line to the serial maintenance terminal. */
void Logger_Log(
    log_level_t level,
    const char *message)
{
    if ((level > LOG_ERROR) || (message == NULL))
    {
        return;
    }

    // timestamp placeholder
    Serial_Write("[00000000]:");
    // logs level / type D:Debug I:Info W:Warning E:Error
    Serial_Write(log_level_prefix[level]);

    Serial_Write(":");
    // finally the message contains code:message
    Serial_Write(message);

    Serial_Write("\r\n");
}
