#include <util/delay.h>

#include "analog.h"
#include "app.h"
#include "btn.h"
#include "led.h"
#include "logger.h"
#include "board.h"
#include "twi.h"


static led_t * const board_led_list[] = {
    &led.ready,
    &led.processing,
    &led.ticket,
    &led.error
};

static btn_t * const board_btn_list[] = {
    &btn.next,
    &btn.confirm,
    &btn.cancel
};

int main(void) {

  int led_length = sizeof(board_led_list) / sizeof(board_led_list[0]);
  for (int index = 0; index < led_length ; index++) {
    LED_Init(board_led_list[index]);
  }

  int btn_length = sizeof(board_btn_list) / sizeof(board_btn_list[0]);
  for (int index = 0; index < btn_length ; index++) {
    BTN_Init(board_btn_list[index]);
  }
  
  Analog_Init(&pot_adc_config);

  TWI_Init(&lcd_twi_config);
  
  Logger_Init();

  APP_Init();
}