#include "schedule.h"

SCHEDULE schedule_list[10] = {};
uint8_t schedule_size = 4, schedule_pos = 0, upcoming_schedule_pos;;
SCHEDULE tmp_schedule;
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
//	uint32_t buff[12];
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
	if (schedule_size == 0)
		return;

	for (i = pos; i < schedule_size - 1; i++) {
		schedule_list[i] = schedule_list[i + 1];
	}
	schedule_size--;
}

void find_upcoming_schedule() {
	int i;
//	uint16_t max_
	for (i = 0; i < schedule_pos; i++) {
		if (schedule_list[i].hour * 60 + schedule_list[i].minute  > c_time.hours * 60 + c_time.minutes) {
			upcoming_schedule_pos = i;
			break;
		}
	}
}
