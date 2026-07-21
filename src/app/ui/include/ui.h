#ifndef UI_H
#define UI_H

#include "controller_types.h"
#include <stdint.h>

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

    UI_PAGE_QUANTITY_NOT_VALID

} ui_page_t;

void UI_SetPage(ui_page_t state);

void UI_Update_Page(volatile controller_state_t * current_state);

void UI_Update_Quantity(uint8_t quantity);

void UI_Update_Destination(const char* destination_name);

#endif