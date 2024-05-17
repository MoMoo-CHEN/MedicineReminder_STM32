#include "button.h"

extern MENU_ITEM menu_items[];
extern uint8_t cur_screen, cur_pos, cur_sel, cur_shift;
extern SCHEDULE schedule_list[];
extern uint8_t schedule_size, schedule_pos;
extern SCHEDULE tmp_schedule;
extern int medicine_notify, medicine_notify_cnt;
extern int sensor_tmout, sensor_tmout_cnt, sensor_tmout_cnt2;

int btn_cnt[4] = {0};

#if BTN_EMUL
extern uint8_t btn_emul[4];
#endif

void check_button() {
	check_button_up();
	check_button_down();
	check_button_select();
	check_button_back();
}

void check_button_up() {
#if BTN_EMUL
	if(btn_emul[0] == 1)
#else
	if(HAL_GPIO_ReadPin(BT_UP_GPIO_Port, BT_UP_Pin) == GPIO_PIN_RESET)
#endif
	{
#if BTN_EMUL
		btn_emul[0] = 0;
		btn_cnt[0] = 39;
#endif
		btn_cnt[0]++;
		if (btn_cnt[0] == 40) {
			if (cur_sel == 0) {
				if (cur_pos > 1) {
					cur_pos -= menu_items[cur_screen].step;
					if (cur_pos > 3)
						cur_shift = cur_pos - 3;
					else
						cur_shift = 0;
				}
			} else {
				if (cur_screen == SETSCHEDULE_SCREEN) {
					if (cur_pos == 2) {
						if (tmp_schedule.type_a < 10)
							tmp_schedule.type_a++;
					} else if (cur_pos == 3) {
						if (tmp_schedule.type_b < 10)
							tmp_schedule.type_b++;
					}
				}
				else if(cur_screen == SETTIME_SCREEN) {
					if(cur_pos == 1) {
						if(tmp_schedule.hour < 23)
							tmp_schedule.hour++;
						else
							tmp_schedule.hour = 0;
					}
					else if(cur_pos == 2) {
						if(tmp_schedule.minute < 59)
							tmp_schedule.minute++;
						else
							tmp_schedule.minute = 0;
					}
				}
			}

			menu_set_content();
		}
	} else {
		btn_cnt[0] = 0;
	}
}

void check_button_down() {
#if BTN_EMUL
	if (btn_emul[1] == 1)
#else
	if (HAL_GPIO_ReadPin(BT_DN_GPIO_Port, BT_DN_Pin) == GPIO_PIN_RESET)
#endif
	{
#if BTN_EMUL
		btn_emul[1] = 0;
		btn_cnt[1] = 39;
#endif
		btn_cnt[1]++;
		if (btn_cnt[1] == 40) {
			if (cur_sel == 0) {
				if (cur_pos < menu_items[cur_screen].n_child) {
					cur_pos += menu_items[cur_screen].step;
					if (cur_pos > 3)
						cur_shift = cur_pos - 3;
				}
			} else {
				if (cur_screen == SETSCHEDULE_SCREEN) {
					if (cur_pos == 2) {
						if (tmp_schedule.type_a > 0)
							tmp_schedule.type_a--;
					} else if (cur_pos == 3) {
						if (tmp_schedule.type_b > 0)
							tmp_schedule.type_b--;
					}
				}
				else if(cur_screen == SETTIME_SCREEN) {
					if(cur_pos == 1) {
						if(tmp_schedule.hour > 0)
							tmp_schedule.hour--;
						else
							tmp_schedule.hour = 23;
					}
					else if(cur_pos == 2) {
						if(tmp_schedule.minute > 0)
							tmp_schedule.minute--;
						else
							tmp_schedule.minute = 59;
					}
				}
			}

			menu_set_content();
		}
	} else {
		btn_cnt[1] = 0;
	}
}

void check_button_select() {
#if BTN_EMUL
	if(btn_emul[2] == 1)
#else
	if (HAL_GPIO_ReadPin(BT_SL_GPIO_Port, BT_SL_Pin) == GPIO_PIN_RESET)
#endif
	{
#if BTN_EMUL
		btn_emul[2] = 0;
		btn_cnt[2] = 39;
#endif
		btn_cnt[2]++;
		if (btn_cnt[2] == 40) {
			if (cur_screen == MAIN_SCREEN) {
				if(medicine_notify == 1) {
					medicine_notify = 0;
					medicine_notify_cnt = 0;
					sensor_tmout_cnt = 0;
				}
				else {
					if(sensor_tmout == 1) {
						sensor_tmout = 0;
						sensor_tmout_cnt2 = 0;
					}
					cur_screen = MENU_SCREEN;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
				}
			} else if (cur_screen == MENU_SCREEN) {
				if (cur_pos == 1) {
					cur_screen = SETSCHEDULE_SCREEN;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
					tmp_schedule.hour = 0;
					tmp_schedule.minute = 0;
					tmp_schedule.type_a = 0;
					tmp_schedule.type_b = 0;
				} else if (cur_pos == 2) {
					cur_screen = DISPLAYLIST_SCREEN;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
				} else if (cur_pos == 3) {
					cur_screen = UPCOMING_SCREEN;
					cur_pos = 0;
					cur_sel = 0;
					cur_shift = 0;
					find_upcoming_schedule();
				}
			} else if (cur_screen == SETSCHEDULE_SCREEN) {
				if(cur_pos == 1) {
					cur_screen = SETTIME_SCREEN;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
				}
				else if (cur_pos == 2 || cur_pos == 3) {
					if (cur_sel == 0)
						cur_sel = 1;
				} else {
					schedule_list[schedule_size].hour = tmp_schedule.hour;
					schedule_list[schedule_size].minute = tmp_schedule.minute;
					schedule_list[schedule_size].type_a = tmp_schedule.type_a;
					schedule_list[schedule_size].type_b = tmp_schedule.type_b;
					schedule_size++;
					store_schedule();
					find_upcoming_schedule();
					update_to_esp();
					HAL_Delay(100);
					cur_screen = MENU_SCREEN;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
					tmp_schedule.hour = 0;
					tmp_schedule.minute = 0;
					tmp_schedule.type_a = 0;
					tmp_schedule.type_b = 0;
				}
			} else if (cur_screen == DISPLAYLIST_SCREEN) {
				schedule_pos = cur_pos;
				cur_screen = VIEWSCHEDULE_SCREEN;
				cur_pos = 1;
				cur_sel = 0;
				cur_shift = 0;
			} else if (cur_screen == VIEWSCHEDULE_SCREEN) {
				if (cur_pos == 4) {
					schedule_remove(schedule_pos - 1);
					store_schedule();
					find_upcoming_schedule();
					update_to_esp();
					HAL_Delay(100);
					cur_screen = DISPLAYLIST_SCREEN;
					cur_pos = schedule_pos - 1;
					cur_sel = 0;
					cur_shift = 0;
				}
			} else if (cur_screen == SETTIME_SCREEN) {
				if(cur_pos < 3 && cur_sel == 0) {
					cur_sel = 1;
				}
				else {
					cur_screen = SETSCHEDULE_SCREEN;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
				}
			}

			menu_set_content();
		}
	} else {
		btn_cnt[2] = 0;
	}
}

void check_button_back() {
#if BTN_EMUL
	if(btn_emul[3] == 1)
#else
	if (HAL_GPIO_ReadPin(BT_BK_GPIO_Port, BT_BK_Pin) == GPIO_PIN_RESET)
#endif
	{
#if BTN_EMUL
		btn_emul[3] = 0;
		btn_cnt[3] = 39;
#endif
		btn_cnt[3]++;
		if (btn_cnt[3] == 40) {
			if (cur_sel == 1) {
				cur_sel = 0;
			} else if (cur_screen == SETSCHEDULE_SCREEN) {
				cur_screen = MENU_SCREEN;
				cur_pos = 1;
				cur_sel = 0;
				cur_shift = 0;
			} else if (cur_screen == DISPLAYLIST_SCREEN) {
				cur_screen = MENU_SCREEN;
				cur_pos = 2;
				cur_sel = 0;
				cur_shift = 0;
			} else if (cur_screen == VIEWSCHEDULE_SCREEN) {
				cur_screen = DISPLAYLIST_SCREEN;
				cur_shift = 0;
				cur_pos = schedule_pos;
				if (cur_pos > 3)
					cur_shift = cur_pos - 3;
				cur_sel = 0;
			} else if (cur_screen == UPCOMING_SCREEN) {
				cur_screen = MENU_SCREEN;
				cur_pos = 3;
				cur_sel = 0;
				cur_shift = 0;
			} else {
				cur_screen = MAIN_SCREEN;
				cur_pos = 0;
				cur_sel = 0;
				cur_shift = 0;
			}

			menu_set_content();
		}
	} else {
		btn_cnt[3] = 0;
	}
}
