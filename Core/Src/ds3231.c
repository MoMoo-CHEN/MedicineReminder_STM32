#include "ds3231.h"

I2C_HandleTypeDef *_ds3231_ui2c;


void DS3231_Init(I2C_HandleTypeDef *hi2c) {
	_ds3231_ui2c = hi2c;
}

void DS3231_SetRegByte(uint8_t regAddr, uint8_t val) {
	uint8_t bytes[2] = { regAddr, val };
	HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, bytes, 2, DS3231_TIMEOUT);
}

uint8_t DS3231_GetRegByte(uint8_t regAddr) {
	uint8_t val;
	HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &regAddr, 1, DS3231_TIMEOUT);
	HAL_I2C_Master_Receive(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &val, 1, DS3231_TIMEOUT);
	return val;
}

uint8_t DS3231_GetDayOfWeek(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DOW));
}

uint8_t DS3231_GetDate(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DATE));
}

uint8_t DS3231_GetMonth(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f);
}

uint16_t DS3231_GetYear(void) {
	uint8_t decYear = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_YEAR));
	uint16_t century = (DS3231_GetRegByte(DS3231_REG_MONTH) >> DS3231_CENTURY) * 100 + 2000;
	return century + decYear;
}

uint8_t DS3231_GetHour(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_HOUR));
}

uint8_t DS3231_GetMinute(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MINUTE));
}

uint8_t DS3231_GetSecond(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_SECOND));
}

void DS3231_SetDayOfWeek(uint8_t dayOfWeek) {
	DS3231_SetRegByte(DS3231_REG_DOW, DS3231_EncodeBCD(dayOfWeek));
}

void DS3231_SetDate(uint8_t date) {
	DS3231_SetRegByte(DS3231_REG_DATE, DS3231_EncodeBCD(date));
}

void DS3231_SetMonth(uint8_t month) {
	uint8_t century = DS3231_GetRegByte(DS3231_REG_MONTH) & 0x80;
	DS3231_SetRegByte(DS3231_REG_MONTH, DS3231_EncodeBCD(month) | century);
}

void DS3231_SetYear(uint16_t year) {
	uint8_t century = (year / 100) % 20;
	uint8_t monthReg = (DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f) | (century << DS3231_CENTURY);
	DS3231_SetRegByte(DS3231_REG_MONTH, monthReg);
	DS3231_SetRegByte(DS3231_REG_YEAR, DS3231_EncodeBCD(year % 100));
}

void DS3231_SetHour(uint8_t hour_24mode) {
	DS3231_SetRegByte(DS3231_REG_HOUR, DS3231_EncodeBCD(hour_24mode & 0x3f));
}

void DS3231_SetMinute(uint8_t minute) {
	DS3231_SetRegByte(DS3231_REG_MINUTE, DS3231_EncodeBCD(minute));
}

void DS3231_SetSecond(uint8_t second) {
	DS3231_SetRegByte(DS3231_REG_SECOND, DS3231_EncodeBCD(second));
}

void DS3231_SetFullTime(uint8_t  hour_24mode, uint8_t minute, uint8_t second){
	DS3231_SetHour(hour_24mode);
	DS3231_SetMinute(minute);
	DS3231_SetSecond(second);
}

void DS3231_SetFullDate(uint8_t date, uint8_t month, uint8_t dow, uint16_t year){
	DS3231_SetDate(date);
	DS3231_SetMonth(month);
	DS3231_SetDayOfWeek(dow);
	DS3231_SetYear(year);
}

void DS3231_GetFullDateTime(RTC_Time *rtc_time) {
	rtc_time->hours = DS3231_GetHour();
	rtc_time->minutes = DS3231_GetMinute();
	rtc_time->seconds = DS3231_GetSecond();
	rtc_time->day = DS3231_GetDate();
	rtc_time->month = DS3231_GetMonth();
	rtc_time->year = DS3231_GetYear();
}

uint8_t DS3231_DecodeBCD(uint8_t bin) {
	return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

uint8_t DS3231_EncodeBCD(uint8_t dec) {
	return (dec % 10 + ((dec / 10) << 4));
}

int8_t DS3231_GetTemperatureInteger(){
	return DS3231_GetRegByte(DS3231_TEMP_MSB);
}

uint8_t DS3231_GetTemperatureFraction(){
	return (DS3231_GetRegByte(DS3231_TEMP_LSB) >> 6) * 25;
}
