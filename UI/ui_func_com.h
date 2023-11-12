#ifndef __UI_FUNTION_COMMON_H__
#define __UI_FUNTION_COMMON_H__

#include "lvgl.h"

#define ARROW_NONE  0
#define ARROW_UP    1
#define ARROW_DOWN  2

typedef struct
{
  uint16_t blink  :1;
  uint16_t arrow  :2;
  uint16_t data   :13;
} ui_sensor_t;

typedef union
{
  struct
  {
    uint8_t main      :1;
    uint8_t menu      :1;
    uint8_t dialog    :1;
    uint8_t diagnotic :1;
    uint8_t compass   :1;
    uint8_t reserve   :3;
  } Bit;
  uint8_t Word;
} ui_screen_active_t;

typedef struct
{
  char _char[25];
} ui_menu_string_t;

void ui_convert_to_sensor(uint8_t *array, uint16_t value);
uint8_t ui_convert_to_int3(uint8_t *array, int16_t value);
uint8_t ui_convert_to_diving_time(uint8_t *array, uint32_t value);
uint8_t ui_convert_to_dive_time(uint8_t *array, uint32_t value);
uint8_t ui_convert_to_distance(uint8_t *array, uint16_t value);
void ui_convert_setpoint(uint8_t *array, uint16_t value,uint16_t mode);
void ui_convert_to_diagnotics_sensor(uint8_t *array, uint16_t value);
void ui_convert_to_compass(uint8_t *array, uint16_t value);

extern ui_screen_active_t screen_active_flag;

#endif
