#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "main.h"

#if BTN_EMUL
uint8_t btn_emul[4] = {0, 0, 0, 0};
#endif

void check_button();
void check_button_up();
void check_button_down();
void check_button_select();
void check_button_back();

#endif
