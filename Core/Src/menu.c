#include "menu.h"

uint8_t cur_screen = 0, cur_pos = 0, cur_sel = 0, cur_shift = 0;
uint8_t need_update_menu = 0;
uint8_t content[10][21] = {0};

extern SCHEDULE schedule_list[10];
extern uint8_t schedule_size, schedule_pos;
extern int upcoming_schedule_pos;;
extern SCHEDULE tmp_schedule;
extern int medicine_notify, type_a_cnt, type_b_cnt;

MENU_ITEM menu_items[] = {{0, 0}, {3, 1}, {4, 1}, {3, 1}, {4, 3}, {3, 0}, {3, 1}};

RTC_Time c_time;

void menu_update() {
	int i;

	if (need_update_menu == 0)
		return;

	lcd_gotoxy(1, 1);
	lcd_send_string((char*) content[0]);
	for (i = 1; i <= 3; i++) {
		lcd_gotoxy(i + 1, 1);
		lcd_send_string((char*) content[i + cur_shift]);
	}

	need_update_menu = 0;
}

void menu_set_content() {
	int i;
	switch (cur_screen) {
	case MAIN_SCREEN:
		strcpy((char*) content[0], "MAIN SCREEN         ");
		sprintf((char*) content[1], "   %02d:%02d:%02d         ", c_time.hours,
				c_time.minutes, c_time.seconds);
		sprintf((char*) content[2], "   %02d-%02d-%04d       ", c_time.day,
				c_time.month, 2000 + c_time.year);
		if(medicine_notify == 1 && type_a_cnt == 0 && type_b_cnt == 0)
			strcpy((char*) content[3], "TIME FOR MEDICINE!!!");
		else
			strcpy((char*) content[3], "                    ");
		break;
	case MENU_SCREEN:
		strcpy((char*) content[0], "MENU                ");
		strcpy((char*) content[1], " SET TIME           ");
		strcpy((char*) content[2], " DISPLAY LIST       ");
		strcpy((char*) content[3], " UPCOMING LIST      ");
		break;
	case SETSCHEDULE_SCREEN:
		strcpy((char*) content[0], "SET TIME            ");
		sprintf((char*) content[1], " TIME: %02dH %02dM      ", tmp_schedule.hour, tmp_schedule.minute);
		sprintf((char*) content[2], " TYPE A: %2d         ", tmp_schedule.type_a);
		sprintf((char*) content[3], " TYPE B: %2d         ", tmp_schedule.type_b);
		strcpy((char*) content[4], " SAVE               ");
		break;
	case DISPLAYLIST_SCREEN:
		strcpy((char*) content[0], "DISPLAY LIST        ");
		for (i = 0; i < schedule_size; i++) {
			sprintf((char*) content[i + 1], " TIME %d: %02dH %02dM    ", i + 1,
					schedule_list[i].hour, schedule_list[i].minute);
		}
		menu_items[3].n_child = schedule_size;
		if(i < 3) {
			for(; i < 3; i++)
				strcpy((char*) content[i + 1], "                    ");
		}
		break;
	case VIEWSCHEDULE_SCREEN:
		sprintf((char*) content[0], "TIME %2d             ", schedule_pos);
		sprintf((char*) content[1], " TIME: %02dH %02dM      ",
				schedule_list[schedule_pos - 1].hour, schedule_list[schedule_pos - 1].minute);
		sprintf((char*) content[2], " TYPE A: %2d         ",
				schedule_list[schedule_pos - 1].type_a);
		sprintf((char*) content[3], " TYPE B: %2d         ",
				schedule_list[schedule_pos - 1].type_b);
		strcpy((char*) content[4], " DELETE             ");
		break;
	case UPCOMING_SCREEN:
		strcpy((char*) content[0], "UPCOMING LIST       ");
		sprintf((char*) content[1], " CURRENT: %02d:%02d:%02d  ", c_time.hours,
				c_time.minutes, c_time.seconds);
		strcpy((char*) content[2], " UPCOMING TIME:     ");
		if(upcoming_schedule_pos != -1)
			sprintf((char*) content[3], "   %02dH %02dM          ",
				schedule_list[upcoming_schedule_pos].hour, schedule_list[upcoming_schedule_pos].minute);
		else
			strcpy((char*) content[3], "                    ");
		break;
	case SETTIME_SCREEN:
		strcpy((char*) content[0], "SET TIME            ");
		sprintf((char*) content[1], " HOUR: %02d           ", tmp_schedule.hour);
		sprintf((char*) content[2], " MINUTE: %02d         ", tmp_schedule.minute);
		strcpy((char*) content[3], " OK                 ");
	}

	if (cur_pos != 0) {
		if (cur_sel == 0)
			content[cur_pos][0] = '>';
		else
			content[cur_pos][0] = '*';
	}

	need_update_menu = 1;
}
