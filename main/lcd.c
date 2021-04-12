#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "i2c-lcd1602.h"
#include "smbus.h"
#include "lcd.h"
#include <time.h>


#define LCD_MAX_LINES 4

#define LCD_NUM_ROWS 4
#define LCD_NUM_COLUMNS 40
#define LCD_NUM_VIS_COLUMNS 20
#define LCD_ADDRESS 0x27

// LCD
smbus_info_t *smbus_info = NULL;
i2c_lcd1602_info_t *lcd_info = NULL;



void init_lcd(void)
{
	//smbus aanzetten
	smbus_info = smbus_malloc();
	smbus_init(smbus_info, I2C_NUM_0, LCD_ADDRESS);
	smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

	// lcd aan met backlight uit
	lcd_info = i2c_lcd1602_malloc();
	i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);

	// een lege regels schrijven
	i2c_lcd1602_clear(lcd_info);
}

