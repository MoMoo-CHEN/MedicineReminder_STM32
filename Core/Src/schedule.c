#include "schedule.h"

SCHEDULE schedule_list[10] = {};
uint8_t schedule_size = 0, schedule_pos = 0;
uint8_t is_next_day = 0;
int upcoming_schedule_pos;
int upcoming_time = 0;
SCHEDULE tmp_schedule, current_schedule;
extern RTC_Time c_time;
uint32_t buff[12];
uint32_t signature;

void load_schedule() {
	int i;
//	uint32_t buff[12];
	Flash_Read_Data(ADDR_FLASH_SECTOR_5, buff, 1);
	signature = buff[0];
	if(buff[0] != 0x11223344) {		// signature failed, no data
		schedule_size = 0;
		buff[0] = 0x11223344;
		buff[1] = 0;
		Flash_Write_Data(ADDR_FLASH_SECTOR_5, buff, 2);
		HAL_Delay(100);
	}
	else {
		Flash_Read_Data(ADDR_FLASH_SECTOR_5 + 4, buff, 11);
		schedule_size = buff[0];
		for(i = 0; i < schedule_size; i++) {
			schedule_list[i].hour = (buff[i + 1] & 0xFF000000) >> 24;
			schedule_list[i].minute = (buff[i + 1] & 0x00FF0000) >> 16;
			schedule_list[i].type_a = (buff[i + 1] & 0x0000FF00) >> 8;
			schedule_list[i].type_b = (buff[i + 1] & 0x000000FF);
		}
	}
}

void store_schedule() {
	int i;
	buff[0] = 0x11223344;
	buff[1] = schedule_size;
	for(i = 0; i < schedule_size; i++) {
		buff[i + 2] = ((uint32_t) schedule_list[i].hour << 24);
		buff[i + 2] |= ((uint32_t) schedule_list[i].minute << 16);
		buff[i + 2] |= ((uint32_t) schedule_list[i].type_a) << 8;
		buff[i + 2] |= schedule_list[i].type_b;
	}
	Flash_Write_Data(ADDR_FLASH_SECTOR_5, buff, schedule_size + 2);
}

void schedule_remove(int pos) {
	int i;

	if(pos < 0)
		return;

	if (schedule_size == 0)
		return;

	for (i = pos; i < schedule_size - 1; i++) {
		schedule_list[i] = schedule_list[i + 1];
	}
	schedule_size--;
}

int convert_to_minute(uint8_t hour, uint8_t min) {
	return hour * 60 + min;
}

void find_upcoming_schedule() {
	int i;
	upcoming_time = 999999;
	upcoming_schedule_pos = -1;
	is_next_day = 0;
	for (i = 0; i < schedule_size; i++) {
		int cur_sch = convert_to_minute(schedule_list[i].hour, schedule_list[i].minute);
		if (cur_sch > convert_to_minute(c_time.hours, c_time.minutes) && cur_sch < upcoming_time) {
			upcoming_time = cur_sch;
			upcoming_schedule_pos = i;
		}
	}
	// upcoming schedule is in the next day?
	if(upcoming_schedule_pos == -1) {
		upcoming_time = 999999;
		for(i = 0; i < schedule_size; i++) {
			int cur_sch = convert_to_minute(schedule_list[i].hour, schedule_list[i].minute);
			if(cur_sch < upcoming_time) {
				upcoming_time = cur_sch;
				upcoming_schedule_pos = i;
				is_next_day = 1;
			}
		}
	}
}

extern UART_HandleTypeDef huart2;
void update_schedulelist_to_esp() {
	uint8_t buff[50];
	buff[0] = 0x85;
	buff[1] = 0xF0;
	buff[2] = 0x82;
	buff[3] = schedule_size;
	for(int i = 0; i < schedule_size; i++) {
		buff[3 * i + 4] = schedule_list[i].hour;
		buff[3 * i + 5] = schedule_list[i].minute;
		buff[3 * i + 6] = (schedule_list[i].type_b << 4) | schedule_list[i].type_a;
	}
	buff[3 * schedule_size + 4] = 0x85;
	buff[3 * schedule_size + 5] = 0xF1;
	HAL_UART_Transmit(&huart2, (uint8_t *) buff, 3 * schedule_size + 6, 1000);
}

void update_upcoming_to_esp(SCHEDULE schedule) {
	uint8_t buff[50];
	buff[0] = 0x85;
	buff[1] = 0xF0;
	buff[2] = 0x83;
	buff[3] = schedule.hour;
	buff[4] = schedule.minute;
	buff[5] = (schedule.type_b << 4) | schedule.type_a;
	buff[6] = 0x85;
	buff[7] = 0xF1;
	HAL_UART_Transmit(&huart2, (uint8_t *) buff, 8, 1000);
}

void update_confirm_to_esp(SCHEDULE schedule) {
	uint8_t buff[50];
	buff[0] = 0x85;
	buff[1] = 0xF0;
	buff[2] = 0x84;
	buff[3] = schedule.hour;
	buff[4] = schedule.minute;
	buff[5] = (schedule.type_b << 4) | schedule.type_a;
	buff[6] = 0x85;
	buff[7] = 0xF1;
	HAL_UART_Transmit(&huart2, (uint8_t *) buff, 8, 1000);
}
