#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "main.h"

typedef struct schedule_tm
{
   uint8_t hour;
   uint8_t minute;
   uint8_t type_a;
   uint8_t type_b;
} SCHEDULE;

extern SCHEDULE schedule_list[10];
extern SCHEDULE current_schedule;
extern uint8_t schedule_size, schedule_pos;

void load_schedule();

void store_schedule();

void schedule_remove(int pos);

int convert_to_minute(uint8_t hour, uint8_t min);

void find_upcoming_schedule();

void update_schedulelist_to_esp();

void update_upcoming_to_esp(SCHEDULE);

void update_confirm_to_esp(SCHEDULE);

#endif
