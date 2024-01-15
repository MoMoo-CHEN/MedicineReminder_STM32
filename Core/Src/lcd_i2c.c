#include "lcd_i2c.h"

#define SLAVE_ADDRESS_LCD 0x4E

extern I2C_HandleTypeDef hi2c1;

void lcd_send_cmd(char cmd) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C;  //en=1, rs=0
	data_t[1] = data_u | 0x08;  //en=0, rs=0
	data_t[2] = data_l | 0x0C;  //en=1, rs=0
	data_t[3] = data_l | 0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t*) data_t, 4,
			100);
}

void lcd_send_data(char data) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D;  //en=1, rs=0
	data_t[1] = data_u | 0x09;  //en=0, rs=0
	data_t[2] = data_l | 0x0D;  //en=1, rs=0
	data_t[3] = data_l | 0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t*) data_t, 4,
			100);
}

void lcd_init(void) {
	lcd_send_cmd(0x33); /* set 4-bits interface */
	lcd_send_cmd(0x32);
	HAL_Delay(50);
	lcd_send_cmd(0x28); /* start to set LCD function */
	HAL_Delay(50);
	lcd_send_cmd(0x01); /* clear display */
	HAL_Delay(50);
	lcd_send_cmd(0x06); /* set entry mode */
	HAL_Delay(50);
	lcd_send_cmd(0x0c); /* set display to on */
	HAL_Delay(50);
	lcd_send_cmd(0x02); /* move cursor to home and set data address to 0 */
	HAL_Delay(50);
	lcd_send_cmd(0x80);
}

void lcd_send_string(char *str) {
	while (*str)
		lcd_send_data(*str++);
}

void lcd_clear_display(void) {
	lcd_send_cmd(0x01); //clear display
}

void lcd_gotoxy(int row, int col) {
	uint8_t pos_addr = 0x80;
	if (row == 2)
		pos_addr |= 0x40;
	else if (row == 3)
		pos_addr |= 0x14;
	else if (row == 4)
		pos_addr |= 0x54;
	pos_addr += col - 1;
	lcd_send_cmd(pos_addr);
}
