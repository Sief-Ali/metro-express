#ifndef UI_H
#define UI_H

#include "controller_types.h"
#include "led.h"

typedef enum
{
    UI_PAGE_IDLE,

    UI_PAGE_SELECT_DESTINATION,

    UI_PAGE_SELECT_QUANTITY,

    UI_PAGE_CONFIRMATION,

    UI_PAGE_PROCESSING,

    UI_PAGE_TICKET,

    UI_PAGE_CANCELLED,

    UI_PAGE_ERROR,

    UI_PAGE_QUANTITY_NOT_VALID,

    UI_PAGE_OUT_OF_STOCK

} ui_page_t;

/* Draws the requested page immediately on the LCD. */
void UI_SetPage(ui_page_t state);

/* Redraws the LCD only when the controller state has changed. */
void UI_Update_Page(volatile controller_state_t * current_state);

/* Refreshes the quantity field on the quantity selection page. */
void UI_Update_Quantity(void);

/* Refreshes the selected destination field on the destination page. */
void UI_Update_Destination(void);

/* Refreshes the confirmation summary using the current passenger data. */
void UI_Update_Summarize(void);

/* Turns on the requested status LED and turns off the other status LEDs. */
void UI_SetLed(led_t * led);

#endif