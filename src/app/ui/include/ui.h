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

void UI_SetPage(ui_page_t state);

void UI_Update_Page(volatile controller_state_t * current_state);

void UI_Update_Quantity(void);

void UI_Update_Destination(void);

void UI_Update_Summarize(void);

void UI_SetLed(led_t * led);

#endif