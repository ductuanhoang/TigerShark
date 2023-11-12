#ifndef __UI_MENU_H__
#define __UI_MENU_H__

#include "lvgl.h"
#include "ui_func_com.h"

typedef union
{
  struct
  {
    uint32_t sensor_value   :1;  //0
    uint32_t sensor_blink   :1;
    uint32_t sensor_arrow   :1;
    uint32_t header_menu    :1;
    uint32_t index_listview :1;
    uint32_t update_listview_value :1;
    uint32_t button_L       :1;
    uint32_t button_R       :1;  //5
    uint32_t index_listview_value :1;
    uint32_t reserve        :23;
  } Bit;
  uint32_t Word;
} ui_menu_flag_update_t;

typedef struct
{
  ui_sensor_t       sensor[3];     // x100
  uint8_t           sum_list_menu; // 
  ui_menu_string_t  header_menu; //
  ui_menu_string_t  list_menu[17]; // 
  ui_menu_string_t  list_menu_value[17]; // 
  uint8_t           index_listview;
  char      button_L[20];     // context string
  char      button_R[10];     // context string
  ui_menu_flag_update_t    update;
}ui_menu_t;

void UI_menu_init(void);
void UI_menu_loadscreen(void);
void UI_menu_update(void);
void UI_menu_timer_blink(void);
//void UI_setpoint_control(void);
extern ui_menu_t  ui_menu;

#endif
