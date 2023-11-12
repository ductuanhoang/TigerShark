#ifndef _POWER_MANAGEMENT_H_
#define _POWER_MANAGEMENT_H_

#include "main.h"

void power_board_on(void);
void power_board_off(void);
void power_lcd_on(void);
void change_brightness_lcd(uint32_t bright);
#endif