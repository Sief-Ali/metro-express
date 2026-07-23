#include "ui.h"

#include "board.h"
#include "lcd.h"
#include "lcd_config.h"
#include "controller_types.h"
#include "led.h"
#include "passenger.h"
#include "str.h"
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

/* UI redraw state used to avoid repainting unchanged pages. */
static controller_state_t last_state;
static ui_page_t current_page;
static bool force_redraw;

/* -------------------------------------------------------------------------- */
/* LED Helpers                                                                 */
/* -------------------------------------------------------------------------- */

static led_t * const board_led_list[] = {
  &led.ready,
  &led.processing,
  &led.ticket,
  &led.error
};

/* Turns one status LED on and turns the rest off. */
static void setLedOn(led_t * led){
  int led_length = (sizeof(board_led_list) / sizeof(board_led_list[0]));
  for (int index = 0; index < led_length ; index++) {
    LED_Off(board_led_list[index]);
  }
  
  LED_On(led);
}

/* -------------------------------------------------------------------------- */
/* LCD Field Writers                                                           */
/* -------------------------------------------------------------------------- */

/* Writes the quantity field on the second LCD row. */
static void UI_Set_Quantity(uint8_t qty) {

  /* Center of a 16x2 LCD:
   * "Qty. :" = 7 chars
   * (16 - 7) = 9 column left
   * number takes one column at 7
   */
  LCD_SetCursor(&lcd_display, 1, 0);
  LCD_PrintString(&lcd_display, "Qty. :");

  LCD_SetCursor(&lcd_display, 1, 7);
  LCD_PrintString(&lcd_display, Str_U8(qty));
}

/* Writes the selected destination field on the second LCD row. */
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

/* Writes the confirmation summary: destination on row 0, quantity on row 1. */
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
  LCD_PrintString(&lcd_display, Str_U8(qty));
}

/* -------------------------------------------------------------------------- */
/* Field Update Helpers                                                        */
/* -------------------------------------------------------------------------- */

/* Refreshes the quantity field only while the quantity page is visible. */
void UI_Update_Quantity(void) {
  if (current_page != UI_PAGE_SELECT_QUANTITY) return;

  uint8_t qty = Passenger_GetQuantity();

  if (qty <= 5U) {
    UI_Set_Quantity(qty);
  } else {
    UI_Set_Quantity(0);
    setLedOn(&led.ready);
  }
}

/* Refreshes the destination field only while the destination page is visible. */
void UI_Update_Destination(void) {
  if (current_page != UI_PAGE_SELECT_DESTINATION) return;

  const char* destination_name = Passenger_GetSelectedDestinationName();

  UI_Set_Destination(destination_name);
}

/* Refreshes the confirmation summary from passenger transaction data. */
void UI_Update_Summarize(void) { 
  const char* destination = Passenger_GetSelectedDestinationName();
  uint8_t qty = Passenger_GetQuantity();

  UI_Set_Summarize(qty, destination);
}

/* -------------------------------------------------------------------------- */
/* Page Renderers                                                              */
/* -------------------------------------------------------------------------- */

/* Draws the idle screen and shows the ready status LED. */
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

/* Draws the destination selection screen. */
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

/* Draws the quantity selection screen. */
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

/* Draws the final confirmation summary screen. */
static void UI_Confirmation(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  UI_Update_Summarize();

  setLedOn(&led.ready);
}

/* Draws a temporary error/cancel popup, then requests a redraw. */
static void UI_Popup(ui_page_t popup_page) {
  LCD_Init(&lcd_display);

  setLedOn(&led.error);
  
  LCD_Clear(&lcd_display);

  switch (popup_page) {
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
        LCD_PrintString(&lcd_display, "Of Stock");
      break;
    case UI_PAGE_CANCELLED:
        LCD_SetCursor(&lcd_display, 0, 0);
        LCD_PrintString(&lcd_display, "Purchase");
        LCD_SetCursor(&lcd_display, 1, 0);
        LCD_PrintString(&lcd_display, "Cancelled");
      break;
    default:
        LCD_SetCursor(&lcd_display, 0, 0);
        LCD_PrintString(&lcd_display, "Error Occurred");
      break;
  }
  force_redraw = true;

  _delay_ms(1500);
}

/* Draws the issued ticket page with the generated ticket code. */
static void UI_Ticket(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  const char *ticket_code = Passenger_GetTicketCode();

  LCD_SetCursor(&lcd_display, 0, 0);
  LCD_PrintString(&lcd_display, "Ticket Issued:");

  LCD_SetCursor(&lcd_display, 1, 1);
  LCD_PrintString(&lcd_display, ticket_code);

  setLedOn(&led.ticket);
}

/* Routes a logical page enum to its LCD renderer. */
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
        UI_Popup(UI_PAGE_QUANTITY_NOT_VALID);
      break;
    case UI_PAGE_OUT_OF_STOCK:
        UI_Popup(UI_PAGE_OUT_OF_STOCK);
      break;
    case UI_PAGE_CANCELLED:
        UI_Popup(UI_PAGE_CANCELLED);
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

/* Maps controller states to UI pages and redraws only when needed. */
void UI_Update_Page(volatile controller_state_t * current_state) {

  if (!force_redraw && last_state == *current_state) return;
  force_redraw = false;
  
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

/* Exposes the shared status LED helper to the controller. */
void UI_SetLed(led_t * led) {
  setLedOn(led);
}
