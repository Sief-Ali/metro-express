#include "analog.h"
#include <stddef.h>

void Analog_Init(const adc_config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    ADC_Init(config);
}

uint16_t Analog_Read(void)
{
    return ADC_Read();
}

uint16_t Analog_ReadChannel(adc_input_t input)
{
    if (ADC_SetInput(&input) == 0U)
    {
        return 0U;
    }

    return ADC_Read();
}