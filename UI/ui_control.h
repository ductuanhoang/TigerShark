#ifndef __UI_CONTROL_H__
#define __UI_CONTROL_H__

#include "main.h"
#include "ui_dialog.h"
#include "ui_main.h"
#include "ui_menu.h"
#include "ui_diagnotics.h"
#include "ui_compass.h"
#include "ui_func_com.h"
#include "main_process.h"
#include "adc_process.h"

typedef struct
{
  void (*f_main)(void);
  void (*f_tap)(void);
  uint16_t value;
}s_buton_callback_t;

typedef struct
{
  s_buton_callback_t buton_cb[20];
  uint8_t current_index;
  uint8_t max_index;
}s_screen_control_t;

typedef struct
{
    uint8_t value;
    uint8_t edit;
}s_setpoint;
typedef struct
{
    uint8_t O2;
    uint8_t He;
    char  name[10];
}s_gas_mix;
typedef struct
{
    uint8_t gLow;
    uint8_t gHigh;
}s_gradient;
typedef enum
{
    SETPOINT_CONFIRM = 0,
    GAS_MIX_CONFIRM,
    CIRCULT_CONFIRM,
    INPUT_CHAR_CONFIRM,
    GAS_DEFINE_CONFIRM,
    GRADIENT_CONFIRM,
    GRADIENT_CUSTOM_CONFIRM,
    SETPOINT_EDIT_CONFIRM,
    CALIB_SUCCESS_CONFIRM,
    CALIB_FAIL_CONFIRM
}e_seting_point;

typedef enum
{
    TBD_SELECT,
    GAS_MIX_SELECT,
    GAS_DEFINE_SELECT,
    GAS_DEFINE_CHANGE,
    GAS_CONFIG_VALUE,
    GRADIENT_SELECT,
    GRADIENT_CHANGE,
    GRADIENT_CUSTOM_SELECT,
    GRADIENT_CUSTOM_EDIT,
    SETPOINT_SELECT,
    SETPOINT_EDIT_SELECT,
    SETPOINT_EDITED,
    CALIBRATION_SELECT,
    CALIBRATION_EDITED
    
}e_menu_select;

typedef struct 
{
    s_setpoint setpoint_value[6];
    s_gas_mix  gas_define[10];
    s_gradient gradient[6];
}s_ui_value;


void list_button_init(void);
void dialog_open_confirm(char *header, char *text);
void menu_gas_list(void);
void menu_option_list(void);
void gas_define_list(void);
void gas_define_index(void);
void update_setname(char *name);
void gas_define_confirm(void);
void return_to_main_screen(void);
void gradient_list(void);
void gradient_custom_list(void);
void gradient_custom_confirm(void);
void setpoint_list(void);
void setpoint_edit_list(void);
void setpoint_edit(uint8_t id);
void setpoint_confirm(void);
void calibration_o2_list(void);
void calibration_o2(uint8_t id);
void std_calib_confirm(uint8_t type);
void alt_calib_confirm(uint8_t type);
void auto_exit_setting_mode(void);
void control_bright_screen(void);
void screen_update(void);
void config_diva_menu(void);
void func_change_time(void);
void set_date_time(void);
void diva_timeout_update();
void rotate_screen(uint32_t screen_rotate);

void config_setting_menu_list(void);


void main_screen_init(void);

extern s_ui_value ui_value;
#endif
