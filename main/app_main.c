
// #region standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
// #endregion

// #region freertos_includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
// #endregion

// #region nvs_includes
#include "nvs_flash.h"
#include "nvs.h"
// #endregion

// #region audio_includes
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
// #endregion

// #region hardware_includes
#include "esp_peripherals.h"
#include "board.h"
#include "i2c-lcd1602.h"
#include "smbus.h"
// #endregion

// #region lcd_defines
#define LCD_MAX_LINES 4
#define LCD_NUM_ROWS 4
#define LCD_NUM_COLUMNS 40
#define LCD_NUM_VIS_COLUMNS 20
#define LCD_ADDRESS 0x27

typedef struct
{
	char row1[20]; /* IDs to jump to on keypress */
	char row2[20];
	char row3[20];
	char row4[20];
} LCDText;
// #endregion

#include "library_mode.h"

static const char *TAG = "MAIN";

void loadLCDDisplay(void);

LCDText lcdInfo;

//Audio
audio_pipeline_handle_t pipeline;
audio_board_handle_t board_handle_t;

// LCD
smbus_info_t *smbus_info = NULL;
i2c_lcd1602_info_t *lcd_info = NULL;

void printMenuItemOnLcd()
{
	i2c_lcd1602_clear(lcd_info);
	i2c_lcd1602_move_cursor(lcd_info, 0, 0);
	i2c_lcd1602_write_string(lcd_info, lcdInfo.row1);
}

void init_lcd(void)
{
	// smbus aanzetten
	smbus_info = smbus_malloc();
	smbus_init(smbus_info, I2C_NUM_0, LCD_ADDRESS);
	smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

	// lcd aan met backlight uit
	lcd_info = i2c_lcd1602_malloc();
	i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);

	// eerste regel
	i2c_lcd1602_clear(lcd_info);
}

//screenAlarm on
void alarmOn()
{
	strcpy(lcdInfo.row1, "SHHHHHHHHHH........");
	strcpy(lcdInfo.row2, "%%%%%%%%%%%%%%%%%%%");
	strcpy(lcdInfo.row3, "%%%%%%%%%%%%%%%%%%%");
	strcpy(lcdInfo.row4, "%%%%%%%%%%%%%%%%%%%");
	printMenuItemOnLcd();
	vTaskDelay(100);
}

//screenAlarm off
void alarmOff()
{
	strcpy(lcdInfo.row1, "                   ");
	strcpy(lcdInfo.row2, "                   ");
	strcpy(lcdInfo.row3, "                   ");
	strcpy(lcdInfo.row4, "                   ");
	printMenuItemOnLcd();
	vTaskDelay(100);
}

//flashes the screen for a couple seconss
void loud_alarm()
{
	for (short i = 0; i < 6; i++)
	{
		switch (i % 2)
		{
		case 0:
			alarmOn();
			break;
		case 1:
			alarmOff();
			break;
		}
	}
}

int app_main()
{

	// #region standard inits
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}

	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set(TAG, ESP_LOG_INFO);

	ESP_LOGI(TAG, "[ 2 ] Start codec chip");
	board_handle_t = audio_board_init();
	audio_hal_ctrl_codec(board_handle_t->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);
	// #endregion

	// #region my custom inits
	init_lcd();
	printMenuItemOnLcd();
	library_mode_init();
	// #endregion


	while (1)
	{	
		//when library mode loop returns true that means the sound was too loud
		if (library_mode_loop())
		{
			printf("alarm triggering \n");
			loud_alarm();
		}
		vTaskDelay(10);
	}
	printf("Goodbye\n");
	return 1;
}
