#include "ui.h"

#include "board.h"
#include "lcd.h"
#include "lcd_config.h"
#include "controller_types.h"
#include "led.h"
#include "passenger.h"
#include "str.h"
#include <stdint.h>
#include <util/delay.h>

static controller_state_t last_state;
static ui_page_t current_page;

/*
  TODO: Refactor to adhere to DRY principles and use a component-based design with dedicated page title fields, rather than hardcoding UI text
*/

// i don't no why i made it so complex but i like it this way

// LEDs utility functions

static led_t * const board_led_list[] = {
  &led.ready,
  &led.processing,
  &led.ticket,
  &led.error
};

static void setLedOn(led_t * led){
  int led_length = (sizeof(board_led_list) / sizeof(board_led_list[0]));
  for (int index = 0; index < led_length ; index++) {
    LED_Off(board_led_list[index]);
  }
  
  LED_On(led);
}

// UI Components

static void UI_Set_Quantity(uint8_t qty) {

  /* Center of a 16x2 LCD:
   * "Qty. :" = 7 chars
   * (16 - 7) = 9 column left
   * number takes one column at 7
   */
  LCD_SetCursor(&lcd_display, 1, 0);
  LCD_PrintString(&lcd_display, "Qty. :");

  LCD_SetCursor(&lcd_display, 1, 7);
  LCD_PrintString(&lcd_display, Str(qty));
}

static void UI_Set_Destination(const char* destination_name) {

  /* Center of a 16x2 LCD:
   * "Dest.:" = 7 chars
   * (16 - 7) = 9 column left
   * destination name start column at 7 and clear the field first
   */
  LCD_SetCursor(&lcd_display, 1, 0);
  LCD_PrintString(&lcd_display, "Dest.:");
  
  //clear
  LCD_SetCursor(&lcd_display, 1, 7);
  LCD_PrintString(&lcd_display, "        ");
  // print destination
  LCD_SetCursor(&lcd_display, 1, 7);
  LCD_PrintString(&lcd_display, destination_name);
}

static void UI_Set_Summarize(uint8_t qty, const char* destination_name) {

  // destination
  LCD_SetCursor(&lcd_display, 0, 0);
  LCD_PrintString(&lcd_display, "Dest.:");
  
  //clear
  LCD_SetCursor(&lcd_display, 0, 7);
  LCD_PrintString(&lcd_display, "        ");
  // print destination
  LCD_SetCursor(&lcd_display, 0, 7);
  LCD_PrintString(&lcd_display, destination_name);
  
  // qty
  LCD_SetCursor(&lcd_display, 1, 0);
  LCD_PrintString(&lcd_display, "Qty. :");

  LCD_SetCursor(&lcd_display, 1, 7);
  LCD_PrintString(&lcd_display, Str(qty));
}

// UI components updater

void UI_Update_Quantity(void) {
  if (current_page != UI_PAGE_SELECT_QUANTITY) return;

  uint8_t qty = Passenger_GetQuantity();

  if (!(qty > 0U) || !(qty > 5U)) {    
    UI_Set_Quantity(qty);
  } else {
    UI_Set_Quantity(0);
    setLedOn(&led.ready);
  }
}

void UI_Update_Destination(void) {
  if (current_page != UI_PAGE_SELECT_DESTINATION) return;

  const char* destination_name = Passenger_GetSelectedDestinationName();

  UI_Set_Destination(destination_name);
}

void UI_Update_Summarize(void) { 
  // if (current_page != UI_PAGE_CONFIRMATION) return;

  const char* destination = Passenger_GetSelectedDestinationName();
  uint8_t qty = Passenger_GetQuantity();

  UI_Set_Summarize(qty, destination);
}

// state handlers

static void UI_Idle(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Metro Express" = 14 chars
   * (16 - 14) / 2 = column 1
   */
  LCD_SetCursor(&lcd_display, 0, 1);
  LCD_PrintString(&lcd_display, "Metro Express");

  setLedOn(&led.ready);
}

static void UI_Select_Destination(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Select Destinat" = 16 chars
   * (16 - 16) / 2 = error //just keep it 0
   */
  LCD_SetCursor(&lcd_display, 0, 0);
  LCD_PrintString(&lcd_display, "Select Destinat");

  const char* dest = Passenger_GetSelectedDestinationName();

  UI_Set_Destination(dest);

  setLedOn(&led.ready);
}

static void UI_Select_Quantity(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Select Quantity" = 16 chars
   * (16 - 16) / 2 = error //just keep it 0
   */
  LCD_SetCursor(&lcd_display, 0, 0);
  LCD_PrintString(&lcd_display, "Select Quantity");

  UI_Set_Quantity(0);

  setLedOn(&led.ready);
}

static void UI_Confirmation(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  UI_Update_Summarize();

  setLedOn(&led.ready);
}

static void UI_Error(ui_page_t error_page) {
  LCD_Init(&lcd_display);

  setLedOn(&led.error);
  
  LCD_Clear(&lcd_display);

  switch (error_page) {
    case UI_PAGE_QUANTITY_NOT_VALID:
        LCD_SetCursor(&lcd_display, 0, 0);
        LCD_PrintString(&lcd_display, "Error: Select");
        LCD_SetCursor(&lcd_display, 1, 1);
        LCD_PrintString(&lcd_display, "Valid Quantity");
      break;
    case UI_PAGE_OUT_OF_STOCK:
        LCD_SetCursor(&lcd_display, 0, 0);
        LCD_PrintString(&lcd_display, "Ticket Out");
        LCD_SetCursor(&lcd_display, 1, 1);
        LCD_PrintString(&lcd_display, "Of Stuck");
      break;
    default:
        LCD_SetCursor(&lcd_display, 0, 0);
        LCD_PrintString(&lcd_display, "Error Accord");
      break;
  }
  last_state = STATE_ERROR;

  _delay_ms(1500);
}

static void UI_Ticket(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  const char *ticket_code = Passenger_GetTicketCode();

  LCD_SetCursor(&lcd_display, 0, 0);
  LCD_PrintString(&lcd_display, "Ticket Issued:");

  LCD_SetCursor(&lcd_display, 1, 2);
  LCD_PrintString(&lcd_display, ticket_code);

  setLedOn(&led.ticket);
}

void UI_SetPage(ui_page_t page) {
  switch (page) {
    case UI_PAGE_IDLE:
        UI_Idle();
      break;
    case UI_PAGE_SELECT_DESTINATION:
        UI_Select_Destination();
      break;
    case UI_PAGE_SELECT_QUANTITY:
        UI_Select_Quantity();
      break;
    case UI_PAGE_QUANTITY_NOT_VALID:
        UI_Error(UI_PAGE_QUANTITY_NOT_VALID);
      break;
    case UI_PAGE_OUT_OF_STOCK:
        UI_Error(UI_PAGE_OUT_OF_STOCK);
      break;
    case UI_PAGE_CONFIRMATION:
        UI_Confirmation();
      break;
    case UI_PAGE_TICKET:
        UI_Ticket();
      break;
    default:
        UI_Idle();
      break;
  }
  current_page = page;
}

void UI_Update_Page(volatile controller_state_t * current_state) {

  if (last_state == *current_state) return;
  
  last_state = *current_state;

  ui_page_t target_page;
  
  switch (*current_state) {
    case STATE_IDLE:
        target_page = UI_PAGE_IDLE;
      break;
    case STATE_SELECT_DESTINATION:
        target_page = UI_PAGE_SELECT_DESTINATION;
      break;
    case STATE_SELECT_QUANTITY:
        target_page = UI_PAGE_SELECT_QUANTITY;
      break;
    case STATE_CONFIRMATION:
        target_page = UI_PAGE_CONFIRMATION;
      break;
    case STATE_TICKET_ISSUED:
        target_page = UI_PAGE_TICKET;
      break;
    default:
        target_page = UI_PAGE_IDLE;
      break;
  }

  UI_SetPage(target_page);
}

void UI_SetLed(led_t * led) {
  setLedOn(led);
}