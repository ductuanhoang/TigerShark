#ifndef __UI_MAIN_SCREEN_H__
#define __UI_MAIN_SCREEN_H__

#include "lvgl.h"
#include "ui_func_com.h"

typedef struct
{
  uint16_t blink  :1;
  uint16_t data   :15;
} ui_battery_t;

typedef struct
{
  uint16_t blink  :1;
  uint16_t data   :15;
} ui_setpoint_t;

typedef struct
{
  uint16_t O2;
  uint16_t he;
} ui_gas_t;

typedef struct
{
  uint16_t blink  :1;
  uint16_t data   :15;
} ui_tts_t;

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
    uint32_t current_distance       :1;
    uint32_t deco_flag      :1;
    uint32_t reserve        :13;
  } Bit;
  uint32_t Word;
} ui_main_flag_update_t;

typedef struct
{
  ui_sensor_t  sensor[3];     // x100
  uint32_t  surface_time;     // second
  uint16_t  dive_time;        // second
  uint16_t  dive_state;        // second
  uint16_t  units_type;       // tyoe
  uint16_t  current_distance;     // feet
  uint16_t  max_distance;     // feet last deth
  ui_battery_t   batterry;       // x10
  int16_t  temperature;      // in F
  uint16_t  temp_type;      // in F
  uint16_t  switch_ui_flags;   
  uint16_t  temp_press_warning;      // in F
  ui_gas_t   gas_mix;          // ???
  ui_setpoint_t  set_point;    // x100
  uint16_t  mode;    // x100
  uint16_t  glow;          //
  uint16_t  ghight;        // ???
  ui_tts_t  time_to_surface;  // ???
  uint16_t  ppo2;             // x100
  uint16_t  stk_tm;           // ???
  uint16_t  stk_expired_warning;           // ???
  uint16_t  deco_time;             
  uint16_t  deco_depth;        
  uint16_t  deco_mode;    
  char      button_L[20];     // context string
  char      button_R[10];     // context string
  ui_main_flag_update_t update;
}ui_main_t;

void UI_main_init(void);
void UI_main_loadscreen(void);
void UI_main_update(void);
void UI_main_timer_blink(void);

extern ui_main_t  ui_main;

#endif
