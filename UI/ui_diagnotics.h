#ifndef __UI_DIAGNOTICS_H__
#define __UI_DIAGNOTICS_H__

#include "lvgl.h"
#include "ui_func_com.h"

typedef union
{
  struct
  {
    uint32_t sensor_data    :1;  //0
    uint32_t sensor_blink   :1;
    uint32_t sensor_arrow   :1;
    uint32_t surface_time   :1;
    uint32_t dive_time      :1;
    uint32_t max_distance   :1;  //5
    uint32_t batterry_data  :1;
    uint32_t batterry_blink :1;
    uint32_t temperature    :1;
    uint32_t gas_mix        :1;
    uint32_t set_point      :1;  //10
    uint32_t xxx            :1;
    uint32_t time_to_surface :1;
    uint32_t ppo2           :1;
    uint32_t stk_tm         :1;
    uint32_t button_L       :1;  //15
    uint32_t button_R       :1;
    uint32_t reserve        :15;
  } Bit;
  uint32_t Word;
} ui_diagnotics_flag_update_t;

typedef struct
{
  ui_sensor_t  sensor[3];     // x100
  uint16_t  current_pressure;     // feet
  uint16_t  current_temp;     // feet
  uint16_t temp_type;
  uint16_t lux_data;
  char      button_L[20];     // context string
  char      button_R[10];     // context string
  ui_diagnotics_flag_update_t update;
}ui_diagnotics_t;

void UI_diagnotics_init(void);
void UI_diagnotics_loadscreen(void);
void UI_diagnotics_update(void);

extern ui_diagnotics_t  ui_diagnotics;

#endif
