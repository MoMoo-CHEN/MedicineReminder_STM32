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

void load_schedule();

void store_schedule();

void schedule_remove(int pos);

int convert_to_minute(uint8_t hour, uint8_t min);

void find_upcoming_schedule();

#endif
