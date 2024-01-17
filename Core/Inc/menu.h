#ifndef _MENU_H_
#define _MENU_H_

#include "main.h"

typedef struct menu_tm
{
   uint8_t n_child;
   uint8_t step;
} MENU_ITEM;

enum {
	MAIN_SCREEN = 0,
	MENU_SCREEN,
	SETSCHEDULE_SCREEN,
	DISPLAYLIST_SCREEN,
	VIEWSCHEDULE_SCREEN,
	UPCOMING_SCREEN,
	SETTIME_SCREEN
};

void menu_update();
void menu_set_content();

#endif
