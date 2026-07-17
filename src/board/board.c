#include "board.h"

board_buttons_t btn = {
    .next = {
        .pin = BTN_NEXT,
        .pullup = PULLUP_STATE_ON
    },

    .confirm = {
        .pin = BTN_CONFIRM,
        .pullup = PULLUP_STATE_ON
    },

    .cancel = {
        .pin = BTN_CANCEL,
        .pullup = PULLUP_STATE_ON
    }
};

board_leds_t led = {
    .ready = {
        .pin = LED_READY,
    },
    .processing = {
      .pin = LED_PROCESSING,
    },
    .ticket = {
      .pin = LED_TICKET,
    },
    .error = {
      .pin = LED_ERROR,
    }
};

adc_config_t pot_adc_config =
{
    .reference = ADC_REFERENCE_AREF,

    .alignment = ADC_ALIGNMENT_RIGHT,

    .prescaler = ADC_PRESCALER_128,

    .input =
    {
        .mode = ADC_INPUT_SINGLE_ENDED,
        .positive = ADC_CHANNEL_0
    },

    .auto_trigger = ADC_AUTO_TRIGGER_DISABLE,

    .interrupt = ADC_INTERRUPT_DISABLE,

    .trigger = ADC_TRIGGER_FREE_RUNNING
};