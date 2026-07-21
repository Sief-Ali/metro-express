#ifndef HAL_ANALOG_H
#define HAL_ANALOG_H

#include <stdint.h>

#include "adc.h"
#include "adc_types.h"

void Analog_Init(const adc_config_t *config);

uint16_t Analog_Read(void);

uint16_t Analog_ReadChannel(adc_input_t input);

uint8_t Analog_Get_Quantity(void);

#endif