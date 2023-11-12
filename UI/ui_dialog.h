#ifndef __UI_DIALOG_H__
#define __UI_DIALOG_H__

#include "lvgl.h"
#include "ui_func_com.h"

typedef union
{
  struct
  {
    uint32_t header       :1;  //0
    uint32_t context      :1;
    uint32_t text_input   :1;
    uint32_t index_text_input :1;
    uint32_t text_preview :1;
    uint32_t button_L     :1;
    uint32_t button_R     :1;
    uint32_t reserve      :25;
  } Bit;
  uint32_t Word;
} ui_dialog_flag_update_t;

typedef struct
{
  ui_menu_string_t  header;     //
  char      context[80];        // context string
  char      text_input[10];
  uint8_t   index_text_input;
  uint8_t   sum_text_input;
  char      text_preview[12];
  char      button_L[20];       // context string
  char      button_R[10];       // context string
  ui_dialog_flag_update_t    update;
}ui_dialog_t;

void UI_dialog_init(void);
void UI_dialog_loadscreen(void);
void UI_dialog_update(void);
void UI_dialog_timer_blink(void);

extern ui_dialog_t  ui_dialog;

#endif
