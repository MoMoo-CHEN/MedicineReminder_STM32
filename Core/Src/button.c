#include "button.h"

extern MENU_ITEM menu_items[];
extern uint8_t cur_screen, cur_pos, cur_sel, cur_shift;
extern SCHEDULE schedule_list[];
extern uint8_t schedule_size, schedule_pos, upcoming_pos;
extern SCHEDULE tmp_schedule;

int btn_cnt[4] = {0};

void check_button() {
	check_button_up();
	check_button_down();
	check_button_select();
	check_button_back();
}

void check_button_up() {
	if(HAL_GPIO_ReadPin(BT_UP_GPIO_Port, BT_UP_Pin) == GPIO_PIN_RESET) {
		btn_cnt[0]++;
		if (btn_cnt[0] == 20) {
			if (cur_sel == 0) {
				if (cur_pos > 1) {
					cur_pos -= menu_items[cur_screen].step;
					if (cur_pos > 3)
						cur_shift = cur_pos - 3;
					else
						cur_shift = 0;
				}
			} else {
				if (cur_screen == 2) {
					if (cur_pos == 2) {
						if (tmp_schedule.type_a < 10)
							tmp_schedule.type_a++;
					} else if (cur_pos == 3) {
						if (tmp_schedule.type_b < 10)
							tmp_schedule.type_b++;
					}
				}
				else if(cur_screen == 6) {
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
	if (HAL_GPIO_ReadPin(BT_DN_GPIO_Port, BT_DN_Pin) == GPIO_PIN_RESET) {
		btn_cnt[1]++;
		if (btn_cnt[1] == 20) {
			if (cur_sel == 0) {
				if (cur_pos < menu_items[cur_screen].n_child) {
					cur_pos += menu_items[cur_screen].step;
					if (cur_pos > 3)
						cur_shift = cur_pos - 3;
				}
			} else {
				if (cur_screen == 2) {
					if (cur_pos == 2) {
						if (tmp_schedule.type_a > 0)
							tmp_schedule.type_a--;
					} else if (cur_pos == 3) {
						if (tmp_schedule.type_b > 0)
							tmp_schedule.type_b--;
					}
				}
				else if(cur_screen == 6) {
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
	if (HAL_GPIO_ReadPin(BT_SL_GPIO_Port, BT_SL_Pin) == GPIO_PIN_RESET) {
		btn_cnt[2]++;
		if (btn_cnt[2] == 20) {
			if (cur_screen == 0) {
				cur_screen = 1;
				cur_pos = 1;
				cur_sel = 0;
				cur_shift = 0;
			} else if (cur_screen == 1) {
				if (cur_pos == 1) {
					cur_screen = 2;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
					tmp_schedule.hour = 0;
					tmp_schedule.type_a = 0;
					tmp_schedule.type_b = 0;
				} else if (cur_pos == 2) {
					cur_screen = 3;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
				} else if (cur_pos == 3) {
					cur_screen = 5;
					cur_pos = 0;
					cur_sel = 0;
					cur_shift = 0;
					find_upcoming_schedule();
				}
			} else if (cur_screen == 2) {
				if(cur_pos == 1) {
					cur_screen = 6;
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
					HAL_Delay(100);
					cur_screen = 1;
					cur_pos = 1;
					cur_sel = 0;
					cur_shift = 0;
				}
			} else if (cur_screen == 3) {
				schedule_pos = cur_pos;
				cur_screen = 4;
				cur_pos = 1;
				cur_sel = 0;
				cur_shift = 0;
			} else if (cur_screen == 4) {
				if (cur_pos == 4) {
					schedule_remove(schedule_pos - 1);
					store_schedule();
					HAL_Delay(100);
					cur_screen = 3;
					cur_pos = schedule_pos - 1;
					cur_sel = 0;
					cur_shift = 0;
				}
			} else if (cur_screen == 6) {
				if(cur_pos < 3 && cur_sel == 0) {
					cur_sel = 1;
				}
				else {
					cur_screen = 2;
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
	if (HAL_GPIO_ReadPin(BT_BK_GPIO_Port, BT_BK_Pin) == GPIO_PIN_RESET) {
		btn_cnt[3]++;
		if (btn_cnt[3] == 20) {
			if (cur_sel == 1) {
				cur_sel = 0;
			} else if (cur_screen == 2) {
				cur_screen = 1;
				cur_pos = 1;
				cur_sel = 0;
				cur_shift = 0;
			} else if (cur_screen == 3) {
				cur_screen = 1;
				cur_pos = 2;
				cur_sel = 0;
				cur_shift = 0;
			} else if (cur_screen == 4) {
				cur_screen = 3;
				cur_shift = 0;
				cur_pos = schedule_pos;
				if (cur_pos > 3)
					cur_shift = cur_pos - 3;
				cur_sel = 0;
			} else if (cur_screen == 5) {
				cur_screen = 1;
				cur_pos = 3;
				cur_sel = 0;
				cur_shift = 0;
			} else {
				cur_screen = 0;
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
