#ifndef _MENU_H_
#define _MENU_H_

#include "main.h"

typedef struct menu_tm
{
   uint8_t n_child;
   uint8_t step;
} MENU_ITEM;

void menu_update();
void menu_set_content();

#endif
