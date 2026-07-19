#ifndef UI_H
#define UI_H

typedef enum {
  UI_STATE_IDLE
} ui_state_t;

void UI_ShowState(ui_state_t state);

#endif