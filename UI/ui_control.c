#include "ui_control.h"
#include <stdio.h>
#include "flash_process.h"
#include "main_process.h"
#include "ms5837.h"
#include "deco.h"
#include "rtc.h"
#include "sensors_calibration.h"
#include "power_management.h"

extern SystemStatus_Deco_t   decoInfo;
extern RTC_HandleTypeDef hrtc;
extern uint16_t north_magnetic_degree;
extern uint16_t lux_level;

static const float feetPerDeciMeter = 0.32808399;
static const float barPerAtm = 1.01325;

#define MAX_SETPOINT   160
#define MIN_SETPOINT   40

#define NUM_ROW   18
#define NUM_COL   25


RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

const char menu_option_context[NUM_ROW][NUM_COL] = \
{
  "Gas Define",       "Conservatism",     "Custom Gradient",  "Define Setpoints",
  "Calibrate 02",       "Config Setting",   "Diva Config",      "Diagnostic Menu",
  "Enter Feature Key",  "Enable Bluetooth", "Power Off Unit",   "Set Time/Date",
  "Reset Stack Timer",  "Reset Dive Logs",  "About / BS Meter", "Compass Screen",
  "Wifi Screen", "Exit Menu"
};

const char control_mode[3][25] = {"Sol - Manual <OFF!> ", "Sol - Juergensen Mode ", "Sol - Standard Mode "};
s_screen_control_t menu_button;
s_screen_control_t dialog_button;
s_screen_control_t main_button;
s_screen_control_t diag_button;
s_screen_control_t compass_button;

const char alphabet[37] = {' ','0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
uint8_t alphabet_id = 0;
uint8_t num_space = 0 ;
uint8_t change_He,change_O2,change_Name,change_gLow,change_gHigh;
uint8_t gradient_high,gradient_low;
extern uint8_t index_left_menu;
extern uint8_t index_right_menu;
uint8_t current_id = 0;
uint8_t button_offset =0;
uint8_t value_offset = 0;
extern uint8_t menu_max_index;
char button_name[15][20];
char gas_name[10][25];
char header_name[50];
uint8_t O2_value,He_value;
e_seting_point type_confirm;
e_menu_select  type_select;
uint8_t last_id_select =0;
uint8_t is_in_menu_flag =0;
uint8_t setpoint_edit_value,change_setpoint,setpoint_flag;
uint8_t array[6];
uint8_t save_config_flag  = 0; 
uint8_t confirm_flag  = 0; 
uint8_t timeout = 0;
uint16_t temp_value;
uint8_t bright[5] = {0,4,8,12,16};
uint8_t brightness = 16;
uint8_t flag_setting_time = 0;
uint8_t flag_setting_date = 0;
uint8_t hour,minute,second ;
uint8_t date,month,year ;
uint8_t id_switch_count;
uint8_t id_switch_flag;
bool blink = true;


#define timeout_min_sec             0
#define timeout_max_sec             360 // 6 minutes will be always on
#define timeout_max_turnOff_sec     300
#define timeout_stepSizeJump_sec    60
#define timeout_stepSize_sec        5
#define timeout_largeStepSize_sec   60
#define number_repeat_min           0
#define number_repeat_max           5

/* init when start*/
void main_screen_init(void)
{
   list_button_init();
}

static void enter_setting_mode(void)
{
    main_button.current_index = 1;
    memset(ui_main.button_L, 0, 20);
    memset(ui_main.button_R, 0, 10);
    memcpy(ui_main.button_L, &button_name[main_button.current_index][0], 20);
    memcpy(ui_main.button_R, "Select", 6);
    ui_main.update.Bit.button_L = 1;
    ui_main.update.Bit.button_R = 1;
    UI_main_update();
    
}

static void enable_deco_leds(void)
{
    if(config_data.user_setting.settings.flags.enableDecoLeds == 1)
    {
        config_data.user_setting.settings.flags.enableDecoLeds = 0;
    }
    else
    {
        config_data.user_setting.settings.flags.enableDecoLeds = 1;
    }
    return_to_main_screen();
    save_config_flag = 1;
}

static void enable_flash_light(void)
{
    if(config_data.user_setting.settings.flags.enableFlashlight == 1)
    {
        config_data.user_setting.settings.flags.enableFlashlight = 0;
    }
    else
    {
        config_data.user_setting.settings.flags.enableFlashlight = 1;
    }
    return_to_main_screen();
    save_config_flag = 1;
}
static void enable_signal_leds(void)
{
    if(config_data.user_setting.settings.flags.enableSignalLeds == 1)
    {
        config_data.user_setting.settings.flags.enableSignalLeds = 0;
    }
    else
    {
        config_data.user_setting.settings.flags.enableSignalLeds = 1;
    }
    return_to_main_screen();
    save_config_flag = 1;
}

static void func_dialog_circuit_confirm(void)
{
    if(config_data.user_setting.circuitMode == OPENED_CIRCULT)
    {
        config_data.user_setting.circuitMode = CLOSED_CIRCULT;
        systemStatus.circuitMode = CLOSED_CIRCULT;
    }
    else
    {
        config_data.user_setting.circuitMode = OPENED_CIRCULT;
        systemStatus.circuitMode = OPENED_CIRCULT;
    }
    save_config_flag = 1;
    return_to_main_screen();
}

static void func_dialog_circuit_cancel(void)
{
    return_to_main_screen();
}

static void func_dialog_gas_select_confirm(void)
{
    systemStatus.gasMixHe_percent = config_data.ui_value.gas_define[menu_button.buton_cb[menu_button.current_index].value].He;
    systemStatus.gasMixO2_percent = config_data.ui_value.gas_define[menu_button.buton_cb[menu_button.current_index].value].O2;
    config_data.user_setting.gasMixO2_percent = config_data.ui_value.gas_define[menu_button.buton_cb[menu_button.current_index].value].O2;
    config_data.user_setting.gasMixHe_percent = config_data.ui_value.gas_define[menu_button.buton_cb[menu_button.current_index].value].He;  
    save_config_flag = 1;
    return_to_main_screen();
}

static void func_dialog_gas_select_cancel(void)
{
    return_to_main_screen();
}

static void func_dialog_setpoit_confirm(void)
{
    systemStatus.setpoint_ata_percent = config_data.ui_value.setpoint_value[main_button.buton_cb[main_button.current_index].value].value;
    config_data.user_setting.setPoint_centiAta = config_data.ui_value.setpoint_value[main_button.buton_cb[main_button.current_index].value].value;
    config_data.user_setting.circuitMode = CLOSED_CIRCULT;
    systemStatus.circuitMode = CLOSED_CIRCULT;
    save_config_flag = 1;
    return_to_main_screen();
}

static void func_gas_select(void)
{
    memset(header_name,0,50);
    sprintf(header_name,"CHANGE CURRENT GAS MIX TO [%d/%d]",config_data.ui_value.gas_define[menu_button.buton_cb[menu_button.current_index].value].O2,config_data.ui_value.gas_define[menu_button.buton_cb[menu_button.current_index].value].He);
    dialog_open_confirm("CONFIRM CHANGE",header_name);
    dialog_button.buton_cb[0].f_main = func_dialog_gas_select_confirm;
    dialog_button.buton_cb[0].f_tap = func_dialog_gas_select_cancel;
  
}


static void func_dialog_setpoint_cancel(void)
{
    return_to_main_screen();
}

static void change_circuit_mode(void)
{
   memset(header_name,0,50);
   if(config_data.user_setting.circuitMode == CLOSED_CIRCULT)
   {
         memcpy(header_name,"Switch To Opened Circult Mode",50);
   }
   else
   {
         memcpy(header_name,"Switch To Closed Circult Mode",50);
   }
   dialog_open_confirm("CONFIRM CHANGE",header_name);
   dialog_button.buton_cb[0].f_main = func_dialog_circuit_confirm;
   dialog_button.buton_cb[0].f_tap = func_dialog_circuit_cancel;
   //save_config_flag = 1;
}

static void func_setpoint(void)
{
    memset(header_name,0,50);
    ui_convert_to_sensor(array,config_data.ui_value.setpoint_value[main_button.buton_cb[main_button.current_index].value].value);
    snprintf(header_name,50,"Configure Setpoint to %s",array);
    dialog_open_confirm("CONFIRM CHANGE",header_name);
    dialog_button.buton_cb[0].f_main = func_dialog_setpoit_confirm;
    dialog_button.buton_cb[0].f_tap = func_dialog_setpoint_cancel;
    

}
static void func_main(void)
{
    main_button.current_index++;
    if(main_button.current_index > main_button.max_index)
    {
        main_button.current_index = 1;
    }
    memset(ui_main.button_L, 0, 20);
    memset(ui_main.button_R, 0, 10);
    memcpy(ui_main.button_L, &button_name[main_button.current_index][0], 20);
    memcpy(ui_main.button_R, "Select", 6);
    ui_main.update.Bit.button_L = 1;
    ui_main.update.Bit.button_R = 1;
    UI_main_update();
}
static void func_menu(void)
{
    menu_button.current_index++;
    if(menu_button.current_index > menu_button.max_index)
    {
        menu_button.current_index = 0;
    }
    ui_menu.index_listview = menu_button.current_index;
    ui_menu.update.Bit.index_listview = 1;
    UI_menu_update();

}

void list_button_init(void)  
{
    main_button.current_index = 0;
    memset(&button_name,0,300);
    uint8_t current_id = 0;
    main_button.buton_cb[current_id].f_tap = control_bright_screen;
    main_button.buton_cb[current_id].f_main =  enter_setting_mode; 
    current_id++;
    for(uint8_t i = 0; i < 6; i++)
    {
        if(config_data.ui_value.setpoint_value[i].edit != 1)
        {    
            ui_convert_to_sensor(array,config_data.ui_value.setpoint_value[i].value);
            snprintf(&button_name[current_id][0],20,"Setpoint %s",array);
            main_button.buton_cb[current_id].f_tap = func_setpoint;
            main_button.buton_cb[current_id].f_main = func_main;
            main_button.buton_cb[current_id].value = i;
            current_id++;
        }
    }
    memcpy(&button_name[current_id][0],"Gas Select",10);
    main_button.buton_cb[current_id].f_tap = menu_gas_list;
    main_button.buton_cb[current_id].f_main = func_main;
    //main_button.buton_cb[current_id].value = i;
    current_id++;
    if(config_data.user_setting.circuitMode == CLOSED_CIRCULT)
    {
       memcpy(&button_name[current_id][0],"SET OC",6);
    }
    else
    {
        memcpy(&button_name[current_id][0],"SET CC",6);
    }
    main_button.buton_cb[current_id].f_tap = change_circuit_mode;
    main_button.buton_cb[current_id].f_main = func_main;
    //main_button.buton_cb[current_id].value = i;
    current_id++;
    memcpy(&button_name[current_id][0],"Option Menu",11);
    main_button.buton_cb[current_id].f_tap = menu_option_list;
    main_button.buton_cb[current_id].f_main = func_main;
    current_id++;
    if(config_data.user_setting.settings.flags.enableDecoLeds == 1)
    {
       memcpy(&button_name[current_id][0],"DvDeco Off",10);
    }
    else
    {
        memcpy(&button_name[current_id][0],"DvDeco On",9);
    }
    main_button.buton_cb[current_id].f_tap = enable_deco_leds;
    main_button.buton_cb[current_id].f_main = func_main;
    current_id++;
    if(config_data.user_setting.settings.flags.enableFlashlight == 1)
    {
       memcpy(&button_name[current_id][0],"Light Off",9);
    }
    else
    {
        memcpy(&button_name[current_id][0],"Light On",8);
    }
    main_button.buton_cb[current_id].f_tap = enable_flash_light;
    main_button.buton_cb[current_id].f_main = func_main;
    current_id++;
    if(config_data.user_setting.settings.flags.enableSignalLeds == 1)
    {
       memcpy(&button_name[current_id][0],"Sig Off",7);
    }
    else
    {
        memcpy(&button_name[current_id][0],"Sig On",6);
    }
    main_button.buton_cb[current_id].f_tap = enable_signal_leds;
    main_button.buton_cb[current_id].f_main = func_main;
    main_button.max_index = current_id;
}

void dialog_open_confirm(char *header, char *text)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memcpy(ui_dialog.header._char, header, 25);
  memcpy(ui_dialog.context, text, 50);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_L, "Confirm", 7);
  memcpy(ui_dialog.button_R, "Cancel", 6);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
}

static void func_change_text_input(void)
{
    if(alphabet[alphabet_id] != ' ')
    {
       if(ui_dialog.index_text_input < 9)
       {
        num_space =0;
        ui_dialog.text_input[ui_dialog.index_text_input] = alphabet[alphabet_id];
        alphabet_id =0;
        memset(ui_dialog.text_preview,0, 12);
        memcpy(ui_dialog.text_preview,&alphabet[alphabet_id], 6);
        ui_dialog.update.Bit.text_preview = 1;
        ui_dialog.update.Bit.text_input = 1;
        UI_dialog_update();
        ui_dialog.index_text_input++;
       }
       else
        {
          update_setname(ui_dialog.text_input);  
        }
    }
    else
    {
       if(ui_dialog.index_text_input < 9)
       {
        ui_dialog.text_input[ui_dialog.index_text_input] = alphabet[alphabet_id];
        alphabet_id =0;
        memset(ui_dialog.text_preview,0, 12);
        memcpy(ui_dialog.text_preview,&alphabet[alphabet_id], 6);
        ui_dialog.update.Bit.text_preview = 1;
        ui_dialog.update.Bit.text_input = 1;
        UI_dialog_update();
        num_space++;
        if(num_space == 3)
        {
            update_setname(ui_dialog.text_input);
        }
        ui_dialog.index_text_input++;
       }
       else
        {
          update_setname(ui_dialog.text_input);  
        }
    }
}

static void func_select_text_input(void)
{
    alphabet_id++;
    if(alphabet_id > 36)
    {
        alphabet_id =0;
    }
    memset(ui_dialog.text_preview,0, 12);
    memcpy(ui_dialog.text_preview,&alphabet[alphabet_id], 6);
    ui_dialog.update.Bit.text_preview = 1;
    UI_dialog_update(); 
}

void dialog_gas_set_name(void)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memset(ui_dialog.text_input,0, 10);
  memset(ui_dialog.text_preview,0, 12);
  memcpy(ui_dialog.header._char, "INPUT NAME", 25);
  memcpy(ui_dialog.context,"Press Left Button To Select And right button to confirm", 55);
  ui_dialog.sum_text_input = 10;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_L, "Next Char", 9);
  memcpy(ui_dialog.button_R, "Select", 6);
  memcpy(ui_dialog.text_preview,&alphabet[alphabet_id], 6);
  ui_dialog.update.Word = 0xffffffff;
  dialog_button.buton_cb[0].f_main = func_select_text_input;
  dialog_button.buton_cb[0].f_tap = func_change_text_input;
 
  UI_dialog_update(); 
  UI_dialog_loadscreen();
}

static void func_gas_define_confirm(void)
{
    uint8_t id = menu_button.buton_cb[menu_button.current_index].value;
    if(change_Name)
    {
        memcpy(config_data.ui_value.gas_define[id].name,ui_dialog.text_input,10);
    }
    if(change_O2)
    {
       config_data.ui_value.gas_define[id].O2 = O2_value;
    }
    if(change_He)
    {
        config_data.ui_value.gas_define[id].He = He_value;
    }
    save_config_flag = 1;
    gas_define_list();
    UI_menu_loadscreen();
}

static void func_gas_define_cancel(void)
{
    gas_define_list();
    UI_menu_loadscreen();
}



void gas_define_confirm(void)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memcpy(ui_dialog.header._char, "CONFIRM CHANGES", 25);
  if((change_He == 1) | (change_O2 == 1)  | (change_Name == 1))
  {
    memset(header_name,0,50);
    uint8_t id =0;
    if(change_Name)
    {
      sprintf(&header_name[id],"NAME : %s ", ui_dialog.text_input);
      id+=19;
      header_name[id] = '\n';
      id++;
    }
    if(change_O2)
    {
      sprintf(&header_name[id],"O2 : %d ", O2_value);
      id+=9;
      header_name[id] = '\n';
      id++;
    }
    for(uint8_t i = 0; i < id; i++)
    {
        if(header_name[i] == '\0')
        {
            header_name[i] = ' ';
        }
    }
    if(change_He)
    {
      sprintf(&header_name[id],"He : %d ", He_value);
    }
    memcpy(ui_dialog.context,header_name, 50);
  }
  else
  {
    memcpy(ui_dialog.context,"NOTHING CHANGE", 55);
  }
  
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_L, "Confirm", 7);
  memcpy(ui_dialog.button_R, "Cancel", 6);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
  dialog_button.buton_cb[0].f_main = func_gas_define_confirm;
  dialog_button.buton_cb[0].f_tap = func_gas_define_cancel; 
  //type_confirm = GAS_DEFINE_CONFIRM;
}

static void open_diagnostic_menu(void)
{
 
  UI_diagnotics_loadscreen();
  diag_button.buton_cb[0].f_tap = return_to_main_screen;
  diag_button.current_index = 0;
}

static void setpoint_compass(void)
{
    ui_compass.setpoint_flag = 1;
    ui_compass.compass_setpoint = north_magnetic_degree;
    memset(ui_compass.button_L, 0, 20);
    memset(ui_compass.button_R, 0, 10);
    
    memcpy(ui_compass.button_L, "Function", 8);
    memcpy(ui_compass.button_R, "Select", 5);
    
    ui_compass.update.Bit.button_L = 1;
    ui_compass.update.Bit.button_R = 1;
    UI_compass_update();
    
}

static void clear_setpoint_compass(void)
{
    ui_compass.setpoint_flag = 0;
    //ui_compass.compass_setpoint = north_magnetic_degree;
    memcpy(ui_compass.button_L, "Function", 8);
    memcpy(ui_compass.button_R, "Select", 5);
    
    ui_compass.update.Bit.button_L = 1;
    ui_compass.update.Bit.button_R = 1;
    UI_compass_update();
}

static void func_compass(void)
{
    compass_button.current_index++;
    if(compass_button.current_index > compass_button.max_index)
    {
        compass_button.current_index = 1;
    }
    memset(ui_compass.button_L, 0, 20);
    memset(ui_compass.button_R, 0, 10);
    if(compass_button.current_index == 1)
    {
        memcpy(ui_compass.button_L, "SetPoint", 20);
        compass_button.buton_cb[1].f_tap = setpoint_compass;
        compass_button.buton_cb[1].f_main = func_compass;
    }
    else if(compass_button.current_index == 2)
    {
        memcpy(ui_compass.button_L, "Clear", 20);
        compass_button.buton_cb[2].f_tap = clear_setpoint_compass;
        compass_button.buton_cb[2].f_main = func_compass;
    }
    else
    {
        memcpy(ui_compass.button_L, "Exit", 20);
        compass_button.buton_cb[3].f_tap = return_to_main_screen;
        compass_button.buton_cb[3].f_main = func_compass;
    }
    
    memcpy(ui_compass.button_R, "Select", 6);
    ui_compass.update.Bit.button_L = 1;
    ui_compass.update.Bit.button_R = 1;
    UI_compass_update();
}

static void open_compass_menu(void)
{
    UI_compass_loadscreen();
    compass_button.buton_cb[0].f_main = func_compass;
    compass_button.current_index = 0;
    compass_button.max_index = 3;
    ui_compass.setpoint_flag = 0;
    ui_compass.compass_setpoint = 0;
}

static void open_wifi_menu(void)
{
  uint8_t index = 0;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "CONFIG WIFI MENU", 16);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  
  memcpy(&ui_menu.list_menu[index]._char[0],"Scan Wifi",9);
  menu_button.buton_cb[index].f_main = func_menu;
  //menu_button.buton_cb[index].f_tap = func_change_unit;
  //menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.units;
  index++;
  
  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Cancel",6);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = return_to_main_screen;
  
  menu_button.current_index = 0;
  menu_button.max_index = index;
  
  index++;

  ui_menu.sum_list_menu = index;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();    
}

static void open_bs_menu(void)
{
  uint8_t index = 0;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "ABOUT SCREEN", 12);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  /*max deth*/
  if(config_data.user_setting.settings.flags.units)
  {
    sprintf(&ui_menu.list_menu[index]._char[0],"Max Depth %d.%d m",systemStatus.bsMeter_maxDepth_deciMeters/10,systemStatus.bsMeter_maxDepth_deciMeters%10);
  }
  else
  {
    sprintf(&ui_menu.list_menu[index]._char[0],"Max Depth %d ft",(uint16_t)((float)systemStatus.bsMeter_maxDepth_deciMeters * feetPerDeciMeter + 0.5));
  }
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = NULL;
  index++;
  /*dive time*/
  uint8_t array_value_main[6];
  ui_convert_to_dive_time(array_value_main, systemStatus.diveTime_sec);
  sprintf(&ui_menu.list_menu[index]._char[0],"Dive Time %s",array_value_main);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = NULL;
  index++;
  /*total dive time*/
  sprintf(&ui_menu.list_menu[index]._char[0],"Total Dive Time %d m",systemStatus.bsMeter_totalDiveTime_sec/60);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = NULL;
  index++;
  /*average dive time*/
  sprintf(&ui_menu.list_menu[index]._char[0],"Average Dive Time %d m",systemStatus.bsMeter_diveTime_average/60);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = NULL;
  index++;
  
  /*average depth*/
  if(config_data.user_setting.settings.flags.units)
  {
    sprintf(&ui_menu.list_menu[index]._char[0],"Average Depth %d.%d m",systemStatus.bsMeter_maxDepth_average/10,systemStatus.bsMeter_maxDepth_average%10);
  }
  else
  {
    sprintf(&ui_menu.list_menu[index]._char[0],"Average Depth %d ft",(uint16_t)((float)systemStatus.bsMeter_maxDepth_average * feetPerDeciMeter + 0.5));
  }
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = NULL;
  index++;
  
  /*serial numbers*/
  sprintf(&ui_menu.list_menu[index]._char[0],"Serial Number %s","0.0.0.0");
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = NULL;
  index++;

  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"EXIT",4);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = return_to_main_screen;
  
  menu_button.current_index = 0;
  menu_button.max_index = index;
  
  index++;

  ui_menu.sum_list_menu = index;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();      
}

static void enter_feature_key(void)
{
    
}

static void power_off_confirm(void)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memcpy(ui_dialog.context,"CONFIRM POWER OFF", 17);
  dialog_button.buton_cb[0].f_tap = return_to_main_screen;
  dialog_button.buton_cb[0].f_main = power_board_off;
  
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_R, "EXIT", 4);
  memcpy(ui_dialog.button_L, "CONFIRM", 7);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
}

static void func_delete_all_dive_logs(void)
{
  sd_card_clear();
  power_board_off();
}

static void delete_all_dive_logs_confirm(void)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memcpy(ui_dialog.context,"CONFIRM DELETE ALL DIVE LOGS", 28);
  dialog_button.buton_cb[0].f_tap = return_to_main_screen;
  dialog_button.buton_cb[0].f_main = func_delete_all_dive_logs;
  
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_R, "EXIT", 4);
  memcpy(ui_dialog.button_L, "CONFIRM", 7);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
}

static void func_reset_stack_time(void)
{
  reseting_stack_timer();
  save_config_flag = 1;

  return_to_main_screen();
}

static void reset_stack_time(void)
{
  
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  sprintf(ui_dialog.context,"Confirm reset stack timer to %d minute", get_reset_stack_time_value());
  dialog_button.buton_cb[0].f_tap = return_to_main_screen;
  dialog_button.buton_cb[0].f_main = func_reset_stack_time;
  dialog_button.buton_cb[0].value = config_data.user_setting.stackTime;
  
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_R, "EXIT", 4);
  memcpy(ui_dialog.button_L, "CONFIRM", 7);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen(); 
}


void menu_option_list(void)
{
    uint8_t index = 0;
    memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
    memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
    memset(ui_menu.button_L, 0, 20);
    memset(ui_menu.button_R, 0, 10);
    memset(ui_menu.header_menu._char, 0, 25);
    ui_menu.sum_list_menu = 17;
    memcpy(ui_menu.list_menu, menu_option_context, NUM_ROW*NUM_COL);
    ui_menu.index_listview = 0;
    memcpy(ui_menu.header_menu._char, "OPT MENU SETTING", 16);
    memcpy(ui_menu.button_L, "Next", 4);
    memcpy(ui_menu.button_R, "Select", 6);
    ui_menu.update.Word = 0xff;
    UI_menu_update();
    UI_menu_loadscreen();
    
    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = gas_define_list;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = gradient_list;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = gradient_custom_list;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = setpoint_list;
    index++;
    
    menu_button.buton_cb[index].f_main = func_menu;
    if(ui_main.dive_state)
    {
        menu_button.buton_cb[index].f_tap = NULL;
    }
    else
    {
        menu_button.buton_cb[index].f_tap = calibration_o2_list;
    }
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = config_setting_menu_list;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = config_diva_menu;
    index++;
    
    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = open_diagnostic_menu;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = enter_feature_key;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = NULL;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = power_off_confirm;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = set_date_time;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = reset_stack_time;
    index++;
    
    if(ui_main.dive_state)
    {
       menu_button.buton_cb[index].f_tap = NULL;  
    }
    else
    {
       menu_button.buton_cb[index].f_tap = delete_all_dive_logs_confirm;
    }
    menu_button.buton_cb[index].f_main = func_menu;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = open_bs_menu;
    index++;
    
    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = open_compass_menu;
    index++;
    
    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = open_wifi_menu;
    index++;

    menu_button.buton_cb[index].f_main = func_menu;
    menu_button.buton_cb[index].f_tap = return_to_main_screen;


    menu_button.max_index = index-1;
    menu_button.current_index = 0;

}

void setpoint_list(void)
{
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "SELECT SETPOINT", 15);
  for(uint8_t i = 0; i < 6; i++)
  {       
     ui_convert_to_sensor(array,config_data.ui_value.setpoint_value[i].value);
     
     sprintf(&ui_menu.list_menu[i]._char[0],"SetPoint  %d",i+1);
     sprintf(&ui_menu.list_menu_value[i]._char[0],"[%s]",array);
     menu_button.buton_cb[i].f_main = func_menu; 
     menu_button.buton_cb[i].f_tap = setpoint_edit_list; 
     menu_button.buton_cb[i].value = i;
  }
  memcpy(&ui_menu.list_menu[6]._char[0],"Exit Menu",9);
  menu_button.buton_cb[6].f_main = func_menu; 
  menu_button.buton_cb[6].f_tap = return_to_main_screen; 
  menu_button.current_index = 0;
  menu_button.max_index = 6;
  ui_menu.sum_list_menu = 7;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();
  //type_select = SETPOINT_EDIT_SELECT;
}

static void func_setting_setpoint(void)
{
    change_setpoint = 1;
    setpoint_edit_value += 5;
    if(setpoint_edit_value > MAX_SETPOINT)
    {
        setpoint_edit_value = MIN_SETPOINT;
    }
     ui_convert_to_sensor(array,setpoint_edit_value);
     memset(&ui_menu.list_menu_value[0]._char[0],0,25);
     sprintf(&ui_menu.list_menu_value[0]._char[0],"[%s]",array); 
     ui_menu.update.Bit.index_listview = 1;
     //ui_menu.update.Bit.update_listview_value = 1;
     UI_menu_update();
}

static void func_enable_setpoint(void)
{
    change_setpoint = 1;
    setpoint_edit_value += 5;
    if(setpoint_edit_value > MAX_SETPOINT)
    {
        setpoint_edit_value = MIN_SETPOINT;
    }
     ui_convert_to_sensor(array,setpoint_edit_value);
     memset(&ui_menu.list_menu_value[0]._char[0],0,25);
     sprintf(&ui_menu.list_menu_value[0]._char[0],"[%s]",array); 
     ui_menu.update.Bit.index_listview = 1;
     //ui_menu.update.Bit.update_listview_value = 1;
     UI_menu_update();
}

void setpoint_edit_list(void)
{
  uint8_t id = menu_button.buton_cb[menu_button.current_index].value;
  setpoint_edit_value = config_data.ui_value.setpoint_value[id].value;
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  sprintf(ui_menu.header_menu._char, "DEFINE SETPOINT %d", id+1);
  
  ui_convert_to_sensor(array,setpoint_edit_value);
  sprintf(&ui_menu.list_menu[0]._char[0],"SetPoint  %d",id+1);
  sprintf(&ui_menu.list_menu_value[0]._char[0],"[%s]",array); 

  menu_button.buton_cb[0].f_main = func_menu;
  menu_button.buton_cb[0].f_tap = func_setting_setpoint;
  menu_button.buton_cb[0].value = id;
  menu_button.max_index++;

  if(config_data.ui_value.setpoint_value[id].edit != 0)
  {
    if(config_data.ui_value.setpoint_value[id].edit == 1)
    {
      memcpy(&ui_menu.list_menu[1]._char[0],"Enabled",7);
    }
    else
    {
      memcpy(&ui_menu.list_menu[1]._char[0],"Disable",7); 
    }
    menu_button.buton_cb[1].f_main = func_menu;
    menu_button.buton_cb[1].f_tap = func_enable_setpoint;
    menu_button.max_index++;
     memcpy(&ui_menu.list_menu[2]._char[0],"Save Changes",12);
     memcpy(&ui_menu.list_menu[3]._char[0],"Cancel",12);
    menu_button.buton_cb[2].f_main = func_menu;
    menu_button.buton_cb[2].f_tap = setpoint_confirm;
    menu_button.buton_cb[2].value = id;
    menu_button.max_index++;
    menu_button.buton_cb[3].f_main = func_menu;
    menu_button.buton_cb[3].f_tap = return_to_main_screen;
    ui_menu.sum_list_menu = 4;
  }
  else
  {
    memcpy(&ui_menu.list_menu[1]._char[0],"Save Changes",12);
    memcpy(&ui_menu.list_menu[2]._char[0],"Cancel",12);
    menu_button.buton_cb[1].f_main = func_menu;
    menu_button.buton_cb[1].f_tap = setpoint_confirm;
    menu_button.max_index++;
    menu_button.buton_cb[2].f_main = func_menu;
    menu_button.buton_cb[2].f_tap = return_to_main_screen;
    ui_menu.sum_list_menu = 3;
  }
  
  
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();
  type_select = SETPOINT_EDITED;
}

static void func_setting_setpoint_dialog_confirm(void)
{
    uint8_t id = menu_button.buton_cb[menu_button.current_index].value;
    config_data.ui_value.setpoint_value[id].value = setpoint_edit_value;
    if(setpoint_flag != 0)
    {
         config_data.ui_value.setpoint_value[id].edit = setpoint_flag;
    }
    save_config_flag = 1;
    return_to_main_screen();
}

void setpoint_confirm(void)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memcpy(ui_dialog.header._char, "CONFIRM CHANGES", 25);
  if(change_setpoint)
  {
    memset(header_name,0,50);
    uint8_t id =0;
    if((change_setpoint == 1) || setpoint_flag != 0)
    {
      ui_convert_to_sensor(array,setpoint_edit_value);
      sprintf(&header_name[id],"Change Setpoint To %s", array);
    }
    memcpy(ui_dialog.context,header_name, 50);
  }
  else
  {
    memcpy(ui_dialog.context,"NOTHING CHANGE", 55);
  }
  
  dialog_button.buton_cb[0].f_main = func_setting_setpoint_dialog_confirm;
  //dialog_button.buton_cb[0].f_tap

  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_L, "Confirm", 7);
  memcpy(ui_dialog.button_R, "Cancel", 6);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
}

void menu_gas_list(void)
{
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "GAS SELECT MENU", 15);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  
  for(uint8_t i  = 0; i < 10; i++)
  {
      sprintf(&ui_menu.list_menu[i]._char[0],"%s",config_data.ui_value.gas_define[i].name);
      sprintf(&ui_menu.list_menu_value[i]._char[0],"[%d/%d]",config_data.ui_value.gas_define[i].O2,config_data.ui_value.gas_define[i].He);
      menu_button.buton_cb[i].f_main = func_menu;
      menu_button.buton_cb[i].f_tap = func_gas_select;
      menu_button.buton_cb[i].value = i;
      menu_button.max_index++;
  }
  memcpy(&ui_menu.list_menu[10]._char[0],"Exit",4);
  menu_button.buton_cb[ menu_button.max_index].f_main = func_menu;
  menu_button.buton_cb[ menu_button.max_index].f_tap = return_to_main_screen;
  ui_menu.sum_list_menu = 11;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();
}
void gas_define_list(void)
{
  menu_button.current_index = 0;
  menu_button.max_index = 10;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(ui_menu.button_L, 0, 20);
  memset(ui_menu.button_R, 0, 10);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "SELECT GAS", 15);
  
  for(uint8_t i  = 0; i < 10; i++)
  {
      sprintf(&ui_menu.list_menu[i]._char[0],"%s",config_data.ui_value.gas_define[i].name);
      sprintf(&ui_menu.list_menu_value[i]._char[0],"[%d/%d]",config_data.ui_value.gas_define[i].O2,config_data.ui_value.gas_define[i].He);
      menu_button.buton_cb[i].f_main = func_menu;
      menu_button.buton_cb[i].f_tap = gas_define_index;
      menu_button.buton_cb[i].value = i;
  }
  memcpy(&ui_menu.list_menu[10]._char[0],"Exit Menu",9);
  menu_button.buton_cb[10].f_main = func_menu;
  menu_button.buton_cb[10].f_tap = return_to_main_screen;
  ui_menu.sum_list_menu = 11;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu = 1;
  UI_menu_update();
  change_He = change_O2 = change_Name = 0;
 // UI_menu_loadscreen();
  //type_select = GAS_DEFINE_CHANGE;
}

static void func_gradient_dialog_confirm(void)
{
    systemStatus.gradientHi = config_data.ui_value.gradient[menu_button.buton_cb[menu_button.current_index].value].gHigh;
    systemStatus.gradientLo = config_data.ui_value.gradient[menu_button.buton_cb[menu_button.current_index].value].gLow;
    config_data.user_setting.gradientHigh = systemStatus.gradientHi;
    config_data.user_setting.gradientLow = systemStatus.gradientLo;
    save_config_flag = 1;
    return_to_main_screen();
}

static void func_gradient_dialog_cancel(void)
{
    return_to_main_screen();
}

static void func_gradient_confirm(void)
{
    memset(header_name,0,50);
    sprintf(header_name,"CHANGE GRADIENT TO [%d/%d]",config_data.ui_value.gradient[menu_button.buton_cb[menu_button.current_index].value].gLow,\
                                                     config_data.ui_value.gradient[menu_button.buton_cb[menu_button.current_index].value].gHigh);
    dialog_open_confirm("CONFIRM CHANGE",header_name);
    dialog_button.buton_cb[0].f_main = func_gradient_dialog_confirm;
    dialog_button.buton_cb[0].f_tap = func_gradient_dialog_cancel;
  //  type_confirm = GRADIENT_CONFIRM;
}

void gradient_list(void)
{
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(ui_menu.button_L, 0, 20);
  memset(ui_menu.button_R, 0, 10);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "SELECT GRADIENT", 15);
  
  for(uint8_t i  = 0; i < 5; i++)
  {
      sprintf(&ui_menu.list_menu[i]._char[0],"GF  [%d]",i+1);
      sprintf(&ui_menu.list_menu_value[i]._char[0],"[%d/%d]",config_data.ui_value.gradient[i].gLow,config_data.ui_value.gradient[i].gHigh);
      menu_button.buton_cb[i].f_main = func_menu;
      menu_button.buton_cb[i].f_tap = func_gradient_confirm;
      menu_button.buton_cb[i].value = i;
  }
  sprintf(&ui_menu.list_menu[5]._char[0],"GF  USER");
  sprintf(&ui_menu.list_menu_value[5]._char[0],"[%d/%d]",config_data.ui_value.gradient[5].gLow,config_data.ui_value.gradient[5].gHigh);
  menu_button.buton_cb[5].f_main = func_menu;
  menu_button.buton_cb[5].f_tap = func_gradient_confirm;
  menu_button.buton_cb[5].value = 5;
  
  memcpy(&ui_menu.list_menu[6]._char[0],"Exit Menu",9);
  menu_button.buton_cb[6].f_main = func_menu;
  menu_button.buton_cb[6].f_tap = return_to_main_screen;

  menu_button.current_index = 0;
  menu_button.max_index = 6;
  ui_menu.sum_list_menu = 7;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu = 1;
  UI_menu_update();
 // UI_menu_loadscreen();
  //type_select = GRADIENT_CHANGE;
}

static void func_change_ghigh(void)
{
    change_gHigh = 1;
    gradient_high++;
    if(gradient_high > 100)
    {
        gradient_high = 0;
    }
    memset(&ui_menu.list_menu_value[0]._char[0],0,25);
    sprintf(&ui_menu.list_menu_value[0]._char[0],"[%d]",gradient_high);
    ui_menu.update.Bit.index_listview = 1;
    //ui_menu.update.Bit.update_listview_value = 1;
    UI_menu_update();

}

static void func_change_glow(void)
{
    change_gLow = 1;
    gradient_low++;
    if(gradient_low > 100)
    {
        gradient_low = 0;
    }
    memset(&ui_menu.list_menu_value[1]._char[0],0,25);
    sprintf(&ui_menu.list_menu_value[1]._char[0],"[%d]",gradient_low);
    ui_menu.update.Bit.index_listview = 1;
    //ui_menu.update.Bit.update_listview_value = 1;
    UI_menu_update();
    
}

void gradient_custom_list(void)
{
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(ui_menu.button_L, 0, 20);
  memset(ui_menu.button_R, 0, 10);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "EDIT CUSTOM GRADIENT", 15);
  
  sprintf(&ui_menu.list_menu[0]._char[0],"GF  HIGH");
  sprintf(&ui_menu.list_menu_value[0]._char[0],"[%d]",config_data.ui_value.gradient[5].gHigh);
  sprintf(&ui_menu.list_menu[1]._char[0],"GF  LOW");
  sprintf(&ui_menu.list_menu_value[1]._char[0],"[%d]",config_data.ui_value.gradient[5].gLow);
  sprintf(&ui_menu.list_menu[2]._char[0],"Save Changes");
  sprintf(&ui_menu.list_menu[3]._char[0],"Cancel");

  menu_button.buton_cb[0].f_main = func_menu;
  menu_button.buton_cb[0].f_tap = func_change_ghigh;
  //menu_button.buton_cb[0].value = i

  menu_button.buton_cb[1].f_main = func_menu;
  menu_button.buton_cb[1].f_tap = func_change_glow;
 // menu_button.buton_cb[1].value = i

  menu_button.buton_cb[2].f_main = func_menu;
  menu_button.buton_cb[2].f_tap = gradient_custom_confirm;
  //menu_button.buton_cb[2].value = i

  menu_button.buton_cb[3].f_main = func_menu;
  menu_button.buton_cb[3].f_tap = return_to_main_screen;
  
  gradient_high = config_data.ui_value.gradient[5].gHigh;
  gradient_low = config_data.ui_value.gradient[5].gLow;
  change_gHigh = change_gLow = 0;

  menu_button.current_index = 0;
  menu_button.max_index = 3;
  ui_menu.sum_list_menu = 5;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu = 1;
  UI_menu_update();
 // UI_menu_loadscreen();
  //type_select = GRADIENT_CUSTOM_EDIT;
}

static void func_gradient_custom_dialog_confirm(void)
{
    config_data.ui_value.gradient[5].gHigh = gradient_high;
    config_data.ui_value.gradient[5].gLow = gradient_low;
    save_config_flag = 1;
    return_to_main_screen();

}

void gradient_custom_confirm(void)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memcpy(ui_dialog.header._char, "CONFIRM CHANGES", 25);
  if((change_gLow == 1) | (change_gHigh == 1))
  {
    memset(header_name,0,50);
    uint8_t id =0;
    if(change_gLow)
    {
      sprintf(&header_name[id],"GRADIENT LOW : %d ", gradient_low);
      id+=19;
      header_name[id] = '\n';
      id++;
    }
    for(uint8_t i = 0; i < id; i++)
    {
        if(header_name[i] == '\0')
        {
            header_name[i] = ' ';
        }
    }
    if(change_gHigh)
    {
      sprintf(&header_name[id],"GRADIENT LOW : %d ", gradient_high);
      id+=19;
      header_name[id] = '\n';
      id++;
    }
    memcpy(ui_dialog.context,header_name, 50);
  }
  else
  {
    memcpy(ui_dialog.context,"NOTHING CHANGE", 55);
  }
  
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_L, "Confirm", 7);
  memcpy(ui_dialog.button_R, "Cancel", 6);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
  dialog_button.buton_cb[0].f_main = func_gradient_custom_dialog_confirm;
  dialog_button.buton_cb[0].f_tap = return_to_main_screen;

}

static void func_std_calib(void)
{
  ok1 = ok2 = ok3 = false;
  if(adc_std_calib())
  {
    std_calib_confirm(1);
  }
  else
  {
    std_calib_confirm(0);
  }
}

static void func_alt_calib(void)
{
  ok1 = ok2 = ok3 = false;
  if(adc_alt_calib())
  {
    alt_calib_confirm(1);
  }
  else
  {
    alt_calib_confirm(0);
  }
}

void calibration_o2_list(void)
{
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(ui_menu.button_L, 0, 20);
  memset(ui_menu.button_R, 0, 10);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "FILL LOOP WITH O2", 17);
  
  sprintf(&ui_menu.list_menu[0]._char[0],"Start Std Calibration");
  sprintf(&ui_menu.list_menu[1]._char[0],"Start Alt Calibration");
  sprintf(&ui_menu.list_menu[2]._char[0],"Exit");
  
  menu_button.buton_cb[0].f_main = func_menu;
  menu_button.buton_cb[0].f_tap = func_std_calib;
  
  menu_button.buton_cb[1].f_main = func_menu;
  menu_button.buton_cb[1].f_tap = func_alt_calib;
    
  menu_button.buton_cb[2].f_main = func_menu;
  menu_button.buton_cb[2].f_tap = return_to_main_screen;
  
  menu_button.current_index = 0;
  menu_button.max_index = 2;

  ui_menu.sum_list_menu = 3;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu = 1;
  UI_menu_update();
  type_select = CALIBRATION_EDITED;
}

static void func_calib_dialog_success(void)
{
   save_config_flag = 1;
   return_to_main_screen();
}

void std_calib_confirm(uint8_t type)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memcpy(ui_dialog.header._char, "CALIBRATION", 25);
  if(type == 0)
  {
    if(ok1 || ok2 || ok3)
    {
      memset(header_name,0,50);
      uint8_t id =0;
      memcpy(header_name,"ALT CALIBRATION FAIL AT\n", 25);
      id+=24;
      if(ok1 == false)
      {
        memcpy(&header_name[id],"SENSOR 1 ",9);
        id+=9;
      }
      if(ok2 == false)
      {
        memcpy(&header_name[id],"SENSOR 2 ",9);
        id+=9;
      }
      if(ok3 == false)
      {
        memcpy(&header_name[id],"SENSOR 3 ",9);
        id+=9;
      }
      memcpy(ui_dialog.context,header_name, 50);
    }
    else
    {
        memcpy(ui_dialog.context,"ALT CALIBRATION FAIL AT ALL SENSOR", 55);
    }
    
    dialog_button.buton_cb[0].f_tap = UI_menu_loadscreen;
    dialog_button.buton_cb[0].f_main = NULL;
   // type_confirm = CALIB_FAIL_CONFIRM;
  }
  else
  {
    memcpy(ui_dialog.context,"CALIBRATION SUCCESS" ,55);
    dialog_button.buton_cb[0].f_tap = func_calib_dialog_success;
    dialog_button.buton_cb[0].f_main = NULL;
  }
  
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_R, "EXIT", 4);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
}
void alt_calib_confirm(uint8_t type)
{
  memset(ui_dialog.header._char,0, 25);
  memset(ui_dialog.context, 0, 80);
  memset(ui_dialog.button_L, 0, 20);
  memset(ui_dialog.button_R, 0, 10);
  memcpy(ui_dialog.header._char, "CALIBRATION", 25);
  if(type == 0)
  {
    if(ok1 || ok2 || ok3)
    {
      memset(header_name,0,50);
      uint8_t id =0;
      memcpy(header_name,"ALT CALIBRATION FAIL AT\n", 25);
      id+=24;
      if(ok1 == false)
      {
        memcpy(&header_name[id],"SENSOR 1 ",9);
        id+=9;
      }
      if(ok2 == false)
      {
        memcpy(&header_name[id],"SENSOR 2 ",9);
        id+=9;
      }
      if(ok3 == false)
      {
        memcpy(&header_name[id],"SENSOR 3 ",9);
        id+=9;
      }
      memcpy(ui_dialog.context,header_name, 50);
    }
    else
    {
      memcpy(ui_dialog.context,"ALT CALIBRATION FAIL AT ALL SENSOR", 55);
    }
    
    dialog_button.buton_cb[0].f_tap = UI_menu_loadscreen;
    dialog_button.buton_cb[0].f_main = NULL;
  }
  else
  {
    sprintf(ui_dialog.context,"STD CALIBRATION SUCCESSULL AT %d mBar" ,systemStatus.rawPressure_mBar);
    dialog_button.buton_cb[0].f_tap = func_calib_dialog_success;
    dialog_button.buton_cb[0].f_main = NULL;
  }
  
  ui_dialog.sum_text_input = 0;
  ui_dialog.index_text_input = 0;
  memcpy(ui_dialog.button_R, "EXIT", 4);
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update(); 
  UI_dialog_loadscreen();
}
static void change_o2_value(void)
{
    change_O2 =1;
    O2_value++;
    if(O2_value > 100)
    {
        O2_value =0;
    }
    memset(&ui_menu.list_menu_value[0]._char[0],0,25);
    sprintf(&ui_menu.list_menu_value[0]._char[0],"%d",O2_value);
    ui_menu.update.Bit.index_listview = 1;
    //ui_menu.update.Bit.update_listview_value = 1;
    UI_menu_update();
}
static void change_he_value(void)
{
    change_He = 1;
    He_value++;
    if(He_value > 100)
    {
       He_value =0;
    }
    memset(&ui_menu.list_menu_value[1]._char[0],0,25);
    sprintf(&ui_menu.list_menu_value[1]._char[0],"%d",He_value);
    ui_menu.update.Bit.index_listview = 1;
    //ui_menu.update.Bit.update_listview_value = 1;
    UI_menu_update();  
}

static void change_name_value(void)
{
   change_Name = 1;
   dialog_gas_set_name();
}


void gas_define_index(void)
{
  uint8_t id = menu_button.buton_cb[menu_button.current_index].value;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(ui_menu.button_L, 0, 20);
  memset(ui_menu.button_R, 0, 10);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  sprintf(ui_menu.header_menu._char, "Define Gas Mix %d",id +1);
  memcpy(ui_menu.button_L, "Next", 4);
  memcpy(ui_menu.button_R, "Select", 6);
  
  sprintf(&ui_menu.list_menu[0]._char[0],"Change O2");
  sprintf(&ui_menu.list_menu[1]._char[0],"Change He");
  sprintf(&ui_menu.list_menu_value[0]._char[0],"%d",config_data.ui_value.gas_define[id].O2);
  sprintf(&ui_menu.list_menu_value[1]._char[0],"%d",config_data.ui_value.gas_define[id].He);
  O2_value =  config_data.ui_value.gas_define[id].O2;
  He_value =  config_data.ui_value.gas_define[id].He;  
  snprintf(&ui_menu.list_menu[2]._char[0],21,"Edit Name  %s",config_data.ui_value.gas_define[id].name);
  sprintf(&ui_menu.list_menu[3]._char[0],"Save Change");
  sprintf(&ui_menu.list_menu[4]._char[0],"Cancel");

  

  menu_button.buton_cb[0].f_main = func_menu;
  menu_button.buton_cb[0].f_tap = change_o2_value;

  menu_button.buton_cb[1].f_main = func_menu;
  menu_button.buton_cb[1].f_tap = change_he_value;

  menu_button.buton_cb[2].f_main = func_menu;
  menu_button.buton_cb[2].f_tap = change_name_value;

  menu_button.buton_cb[3].f_main = func_menu;
  menu_button.buton_cb[3].f_tap = gas_define_confirm;
  menu_button.buton_cb[3].value = id;

  menu_button.buton_cb[4].f_main = func_menu;
  menu_button.buton_cb[4].f_tap = return_to_main_screen;

  menu_button.current_index = 0;
  menu_button.max_index = 4;

  ui_menu.sum_list_menu = 5;
  ui_menu.update.Bit.button_L = 1;
  ui_menu.update.Bit.button_R = 1;
  ui_menu.update.Bit.index_listview = 1;
  //ui_menu.update.Bit.index_listview_value = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
 // type_select = GAS_CONFIG_VALUE;
}

void update_setname(char *name)
{
     memset(&ui_menu.list_menu[2]._char[0],0,25);
     snprintf(&ui_menu.list_menu[2]._char[0],25,"Edit Name  %s",name);
     ui_menu.update.Bit.index_listview = 1;
     UI_menu_update();
     UI_menu_loadscreen();
    // type_select = GAS_CONFIG_VALUE;
}
void return_to_main_screen(void)
{
    //confirm_flag = 0;
    timeout = 0;
    memset(ui_main.button_L, 0, 20);
    memset(ui_main.button_R, 0, 10);
    memcpy(ui_main.button_L, "Function", 8);
    memcpy(ui_main.button_R, "Bright", 6);
    ui_main.update.Bit.button_R = 1;
    ui_main.update.Bit.button_L = 1;
    UI_main_update();
    UI_main_loadscreen();
    main_screen_init();
    main_button.current_index = 0;
}

static uint32_t  updateFracO2(void)
{
    uint32_t  gasMixO2_percent;
    uint32_t  diluentPPO2_percent;
    float    atmospheres;

    atmospheres = ((14.7 + ( systemStatus.depth_decimeters / 10.0) * 1.461505) / 14.7 );
    
    gasMixO2_percent = systemStatus.gasMixO2_percent;

    diluentPPO2_percent = (uint32_t)((float)gasMixO2_percent * atmospheres);

    return diluentPPO2_percent;
}

void screen_update(void)
{
      UI_main_timer_blink();
      UI_menu_timer_blink();
      UI_dialog_timer_blink();
      if(config_data.user_setting.settings.flags.ppO2units)
      {
          ui_main.sensor[0].data = (uint16_t)((float)(systemStatus.ppO2_atm_percent[0]) * barPerAtm + 0.5);
          ui_main.sensor[1].data = (uint16_t)((float)(systemStatus.ppO2_atm_percent[1]) * barPerAtm + 0.5);
          ui_main.sensor[2].data = (uint16_t)((float)(systemStatus.ppO2_atm_percent[2]) * barPerAtm + 0.5);
          ui_menu.sensor[0].data = (uint16_t)((float)(systemStatus.ppO2_atm_percent[0]) * barPerAtm + 0.5);
          ui_menu.sensor[1].data = (uint16_t)((float)(systemStatus.ppO2_atm_percent[1]) * barPerAtm + 0.5);
          ui_menu.sensor[2].data = (uint16_t)((float)(systemStatus.ppO2_atm_percent[2]) * barPerAtm + 0.5);
      }
      else
      {
          ui_main.sensor[0].data = systemStatus.ppO2_atm_percent[0];
          ui_main.sensor[1].data = systemStatus.ppO2_atm_percent[1];
          ui_main.sensor[2].data = systemStatus.ppO2_atm_percent[2];
          ui_menu.sensor[0].data = systemStatus.ppO2_atm_percent[0];
          ui_menu.sensor[1].data = systemStatus.ppO2_atm_percent[1];
          ui_menu.sensor[2].data = systemStatus.ppO2_atm_percent[2];
      }
      ui_diagnotics.sensor[0].data = adc_raw_value[0];
      ui_diagnotics.sensor[1].data = adc_raw_value[1];
      ui_diagnotics.sensor[2].data = adc_raw_value[2];
      
      ui_compass.sensor[0].data =  ui_main.sensor[0].data;
      ui_compass.sensor[1].data =  ui_main.sensor[1].data;
      ui_compass.sensor[2].data =  ui_main.sensor[2].data;
      
      if(systemStatus.alerts.flags.unableToControl)
      {
          blink = !blink;
          for(uint8_t i = 0; i < 3; i++)
          {
             if(blink)
             {
                 ui_main.sensor[i].arrow = 1;
                 ui_menu.sensor[i].arrow = 1;
             }
             else
             {
                 ui_main.sensor[i].arrow = 2;
                 ui_menu.sensor[i].arrow = 2;
             }
             ui_main.sensor[i].blink = 1; 
             ui_menu.sensor[i].blink = 1;
          }
          ui_main.time_to_surface.blink = 1;
      }
      else
      {
        ui_main.time_to_surface.blink = 0;
        ui_main.sensor[0].blink = systemStatus.alerts.flags.badPpO2Sensor1;
        ui_main.sensor[1].blink = systemStatus.alerts.flags.badPpO2Sensor2;
        ui_main.sensor[2].blink = systemStatus.alerts.flags.badPpO2Sensor3;
        ui_menu.sensor[0].blink = systemStatus.alerts.flags.badPpO2Sensor1;
        ui_menu.sensor[1].blink = systemStatus.alerts.flags.badPpO2Sensor2;
        ui_menu.sensor[2].blink = systemStatus.alerts.flags.badPpO2Sensor3;
        for(uint8_t i = 0; i < 3; i++)
        {
            if(systemStatus.warnings.highPpO2Sensor[i])
            {
                ui_main.sensor[i].arrow = 1;
                ui_menu.sensor[i].arrow = 1;
            }
            else if(systemStatus.warnings.lowPpO2Sensor[i])
            {
                ui_main.sensor[i].arrow = 2;
                ui_menu.sensor[i].arrow = 2;
            }
            else
            {
                ui_main.sensor[i].arrow = 0;
                ui_menu.sensor[i].arrow = 0;  
            }
        }
      }

      ui_main.surface_time = systemStatus.surfaceTime_sec;
      ui_main.dive_time = systemStatus.diveTime_sec + systemStatus.current_diveTime_sec;
      if(systemStatus.alerts.flags.unknownDepthAndTemperature == 1)
      {
        ui_main.temp_press_warning = 1;
      }
      else
      {
        ui_main.temp_press_warning = 0;
      }
      
      if(config_data.user_setting.settings.flags.units == 1)
      {
         ui_main.units_type = config_data.user_setting.settings.flags.units;
         ui_main.current_distance = systemStatus.depth_decimeters;
         ui_main.max_distance = systemStatus.maxDepth_decimeters;
         
      }
      else
      {
        ui_main.units_type = config_data.user_setting.settings.flags.units;
        ui_main.current_distance = (uint16_t)((float)systemStatus.depth_decimeters * feetPerDeciMeter + 0.5);
        ui_main.max_distance = (uint16_t)((float)systemStatus.maxDepth_decimeters * feetPerDeciMeter + 0.5);
      }
     
      
      if(config_data.user_setting.settings.flags.units == 1)
      {
        ui_main.temp_type= 1;
        ui_diagnotics.temp_type =1;
        ui_main.temperature = get_temperate(TEMP_C);
        ui_diagnotics.current_temp = get_temperate(TEMP_C);
      }
      else
      {
        ui_main.temp_type = 0;
        ui_main.temperature =  get_temperate(TEMP_F);
        ui_diagnotics.temp_type = 0;
        ui_diagnotics.current_temp = get_temperate(TEMP_F);
      }
      
      ui_main.gas_mix.he = systemStatus.gasMixHe_percent;
      ui_main.gas_mix.O2 = systemStatus.gasMixO2_percent;
      ui_main.mode = systemStatus.circuitMode;
      
      ui_main.ghight = systemStatus.gradientHi;
      ui_main.glow = systemStatus.gradientLo;
      //ui_main.
      ui_main.set_point.blink = systemStatus.circuitMode;
      ui_main.time_to_surface.data = decoInfo.timeToSurface_min;
     ui_main.batterry.data = volt_calculate()/100;
     if( ui_main.batterry.data < config_data.user_setting.batteryWarning_deciVolt)
     {
       ui_main.batterry.blink =1;
     }
     else
     {
       ui_main.batterry.blink =0;
     }
     
     ui_main.set_point.data = systemStatus.setpoint_ata_percent;
     ui_main.stk_tm = systemStatus.stackTimeRemaining_sec/60;
     if(ui_main.stk_tm == 0)
     {
        ui_main.stk_expired_warning = 1;
     }
     else
     {
        ui_main.stk_expired_warning = 0;
     }
     ui_diagnotics.current_pressure = get_pressure_mbar();
     ui_main.ppo2 = updateFracO2();
     
     id_switch_count++;
     if(id_switch_count >=4)
     {
       if(id_switch_flag)
       {
        ui_main.switch_ui_flags = 1;
        id_switch_flag = 0;
       }
       else
       {
         ui_main.switch_ui_flags = 0;
         id_switch_flag = 1;
       }
        id_switch_count = 0;
     }
     if(decoInfo.firstStop_meters == 0)
     {
        ui_main.deco_mode = 0;
     }
     else
     {
        ui_main.deco_mode = 1;
     }
     ui_main.deco_time = decoInfo.firstTime_min;
     if(config_data.user_setting.settings.flags.units == 1)
     {
        ui_main.deco_depth = decoInfo.firstStop_meters;
     }
     else
     {
         ui_main.deco_depth = (decoInfo.firstStop_meters/3)*10;
     }
     
     ui_diagnotics.lux_data = lux_level;
     
     if(screen_active_flag.Bit.main)
     {
        ui_main.update.Word = 0xffffffff;
        UI_main_update();
     }
     else if(screen_active_flag.Bit.menu)
     {
       ui_menu.update.Bit.sensor_arrow = 1;
       ui_menu.update.Bit.sensor_blink = 1;
       ui_menu.update.Bit.sensor_value = 1; 
       UI_menu_update();
     }
     else if(screen_active_flag.Bit.diagnotic)
     {
        ui_diagnotics.update.Bit.sensor_data = 1; 
        UI_diagnotics_update();
     }
     else if(screen_active_flag.Bit.compass)
     {
        ui_compass.update.Bit.sensor_data = 1; 
        UI_compass_update();
     }
}

void auto_exit_setting_mode(void)
{
    if(save_config_flag)
    {
        save_config_flag = 0;
        at_config_set_info();
    }
    if((screen_active_flag.Bit.dialog != 1) && (timeout > 0) && (screen_active_flag.Bit.diagnotic != 1) && (screen_active_flag.Bit.compass != 1))
    {
        timeout--;
        if(timeout == 0)
        {
          if(((screen_active_flag.Bit.main == 1) && (main_button.current_index != 0)) || ((screen_active_flag.Bit.main != 1)))
          {
           return_to_main_screen();  
          }
        }
    }
}
extern uint16_t lux_level;
void control_bright_screen(void)
{
  static uint32_t i =0;
//  i+=50;
//  if(i>500)
//    i = 50;
  if(lux_level > 1000)
  {
    i = 1000;
  }
  else if (lux_level <100)
  {
    i =100;
  }
  else
  {
    i = lux_level;
  }
  i=i/2;
  change_brightness_lcd(i-1);
}

static void func_change_unit(void)
{
    if(menu_button.buton_cb[0].value)
    {
        menu_button.buton_cb[0].value = 0; 
        memset(&ui_menu.list_menu[0]._char[0],0,25);
        memcpy(&ui_menu.list_menu[0]._char[0],"Imperial Units",14);
        ui_menu.update.Bit.index_listview = 1;
    }
    else
    {
        menu_button.buton_cb[0].value = 1; 
        memset(&ui_menu.list_menu[0]._char[0],0,25);
        memcpy(&ui_menu.list_menu[0]._char[0],"Metric Units",12);
        ui_menu.update.Bit.index_listview = 1;
    }
}

static void func_change_PPO2_unit(void)
{
    if(menu_button.buton_cb[1].value)
    {
        menu_button.buton_cb[1].value = 0; 
        memset(&ui_menu.list_menu[1]._char[0],0,25);
        memcpy(&ui_menu.list_menu[1]._char[0],"PPO2 Units - atm",16 );
        ui_menu.update.Bit.index_listview = 1;
    }
    else
    {
        menu_button.buton_cb[1].value = 1; 
        memset(&ui_menu.list_menu[1]._char[0],0,25);
        memcpy(&ui_menu.list_menu[1]._char[0],"PPO2 Units - bars",17);
        ui_menu.update.Bit.index_listview = 1;
    }
}

static void func_change_screenRotation(void)
{
    menu_button.buton_cb[2].value++;
    if(menu_button.buton_cb[2].value >=2)
    {
        menu_button.buton_cb[2].value = 0;
    }
    if(menu_button.buton_cb[2].value == 3)
    {
        memset(&ui_menu.list_menu[2]._char[0],0,25);
        memcpy(&ui_menu.list_menu[2]._char[0],"Portrait Mode",13);
        ui_menu.update.Bit.index_listview = 1;
    }
    else if(menu_button.buton_cb[2].value == 2)
    {
        memset(&ui_menu.list_menu[2]._char[0],0,25);
        memcpy(&ui_menu.list_menu[2]._char[0],"Portrait 180 Mode",17);
        ui_menu.update.Bit.index_listview = 1;
    }
    else if(menu_button.buton_cb[2].value == 1)
    {
        memset(&ui_menu.list_menu[2]._char[0],0,25);
        memcpy(&ui_menu.list_menu[2]._char[0],"Landscape 180 Mode",18);
        ui_menu.update.Bit.index_listview = 1;
    }
    else
    {
        memset(&ui_menu.list_menu[2]._char[0],0,25);
        memcpy(&ui_menu.list_menu[2]._char[0],"Landscape Mode",14);
        ui_menu.update.Bit.index_listview = 1;
    }
}

static void func_change_stacktime(void)
{
    menu_button.buton_cb[3].value += 30;
    if(menu_button.buton_cb[3].value > 360)
    {
        menu_button.buton_cb[3].value = 30;
    }
    if(menu_button.buton_cb[3].value != 360)
    {
      sprintf(&ui_menu.list_menu[3]._char[0],"Max Stac Tm m %d",menu_button.buton_cb[3].value);
    }
    else
    {
      memcpy(&ui_menu.list_menu[3]._char[0],"Max Stac Tm m max",17); 
    }
    ui_menu.update.Bit.index_listview = 1;

}

static void func_change_control_mode(void)
{
    menu_button.buton_cb[4].value ++;
    if(menu_button.buton_cb[4].value > 2)
    {
        menu_button.buton_cb[4].value = 0;
    } 
    memcpy(&ui_menu.list_menu[4]._char[0],control_mode[menu_button.buton_cb[4].value],25);
    ui_menu.update.Bit.index_listview = 1;
}

static void func_change_batt_warning(void)
{
    menu_button.buton_cb[5].value ++;
    if(menu_button.buton_cb[5].value > 42)
    {
        menu_button.buton_cb[5].value = 12;
    }
    
    sprintf(&ui_menu.list_menu[5]._char[0],"Batt Warning Volt %d.%d",menu_button.buton_cb[5].value/10,menu_button.buton_cb[5].value%10);
    ui_menu.update.Bit.index_listview  = 1;
}

static void func_change_auto_shutdown_time(void)
{
    menu_button.buton_cb[6].value ++;
    if(menu_button.buton_cb[6].value > 30)
    {
        menu_button.buton_cb[6].value = 1;
    } 
    sprintf(&ui_menu.list_menu[6]._char[0],"Auto Shutdown m %d",menu_button.buton_cb[6].value);
    ui_menu.update.Bit.index_listview  = 1;
}

static void func_change_darkmode(void)
{
   menu_button.buton_cb[8].value ++;
   if(menu_button.buton_cb[8].value >= 2)
   {
     menu_button.buton_cb[8].value = 0;
   }
   if(menu_button.buton_cb[8].value)
   {
        memset(&ui_menu.list_menu[8]._char[0],0,25);
        memcpy(&ui_menu.list_menu[8]._char[0],"DarkMode On",11);
        ui_menu.update.Bit.index_listview = 1;    
   }
   else
   {
        memset(&ui_menu.list_menu[8]._char[0],0,25);
        memcpy(&ui_menu.list_menu[8]._char[0],"DarkMode Off",12);
        ui_menu.update.Bit.index_listview = 1;
   }
}

static void func_change_backlight_timeout(void)
{
    menu_button.buton_cb[7].value ++;
    if(menu_button.buton_cb[7].value > 30)
    {
        menu_button.buton_cb[7].value = 0;
    } 
     sprintf(&ui_menu.list_menu[7]._char[0],"BackLight timeout s %d",menu_button.buton_cb[7].value);
    ui_menu.update.Bit.index_listview  = 1;
}

static void func_save_config(void)
{
    config_data.user_setting.settings.flags.divaMode = menu_button.buton_cb[0].value;
    config_data.user_setting.settings.flags.ppO2units = menu_button.buton_cb[1].value;
    config_data.user_setting.settings.flags.screenRotation = menu_button.buton_cb[2].value;
    config_data.user_setting.stackTime = menu_button.buton_cb[3].value;
    config_data.user_setting.solMode = menu_button.buton_cb[4].value;
    systemStatus.solMode = menu_button.buton_cb[4].value;
    config_data.user_setting.batteryWarning_deciVolt = menu_button.buton_cb[5].value;
    config_data.user_setting.autoShutdownTime_min = menu_button.buton_cb[6].value;
    config_data.user_setting.backlightTimeout_sec = menu_button.buton_cb[7].value;
    config_data.user_setting.settings.flags.darkmode = menu_button.buton_cb[8].value;
    config_data.user_setting.settings.flags.divaSetpointBlinkRepeat_sec = menu_button.buton_cb[9].value;
    reseting_stack_timer();
    rotate_screen(config_data.user_setting.settings.flags.screenRotation);
    return_to_main_screen();
    save_config_flag = 1;
}

void config_setting_menu_list(void)
{
  uint8_t index = 0;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "CONFIG SETTING MENU", 19);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  /*setting Metric units*/
  if(config_data.user_setting.settings.flags.units)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Metric Units",12);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Imperial Units",14);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_unit;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.units;
  index++;

  /*setting PPO2 units*/
  if(config_data.user_setting.settings.flags.ppO2units)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"PPO2 Units - bars",17);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"PPO2 Units - atm",16);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_PPO2_unit;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.ppO2units;
  index++;

  /*setting screen rotation*/
  if(config_data.user_setting.settings.flags.screenRotation == 3)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Portrait Mode",13);
  }
  else  if(config_data.user_setting.settings.flags.screenRotation == 2)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Portrait 180 Mode",17);
  }
  else if(config_data.user_setting.settings.flags.screenRotation == 1)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Landscape 180 Mode",18);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Landscape Mode",14);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_screenRotation;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.screenRotation;
  index++;

  /*setting stacktime*/
  if(config_data.user_setting.stackTime != 360)
  {
    sprintf(&ui_menu.list_menu[index]._char[0],"Max Stac Tm m %d",config_data.user_setting.stackTime);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Max Stac Tm m max",17);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_stacktime;
  menu_button.buton_cb[index].value = config_data.user_setting.stackTime;
  index++;

  /*setting mode*/
  memcpy(&ui_menu.list_menu[index]._char[0],control_mode[config_data.user_setting.solMode],25);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_control_mode;
  menu_button.buton_cb[index].value = config_data.user_setting.solMode;
  index++;

  /*setting Batt Warning*/
  sprintf(&ui_menu.list_menu[index]._char[0],"Batt Warning Volt %d.%d",config_data.user_setting.batteryWarning_deciVolt/10,config_data.user_setting.batteryWarning_deciVolt%10);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_batt_warning;
  menu_button.buton_cb[index].value = config_data.user_setting.batteryWarning_deciVolt;
  index++;

  /*setting auto shutdow*/
  sprintf(&ui_menu.list_menu[index]._char[0],"Auto Shutdown m %d",config_data.user_setting.autoShutdownTime_min);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_auto_shutdown_time;
  menu_button.buton_cb[index].value = config_data.user_setting.autoShutdownTime_min;
  index++;

  /*setting backlight timeout*/
  sprintf(&ui_menu.list_menu[index]._char[0],"BackLight timeout s %d",config_data.user_setting.backlightTimeout_sec);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_backlight_timeout;
  menu_button.buton_cb[index].value = config_data.user_setting.backlightTimeout_sec;
  index++;

    /*setting darkmode*/
  if(config_data.user_setting.settings.flags.darkmode)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"DarkMode On",11);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"DarkMode Off",12);
  }
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_darkmode;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.darkmode;
  index++;
  
  /*setting saveconfig*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Save Config",11);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_save_config;
  index++;

  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Cancel",6);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = return_to_main_screen;
  
  menu_button.current_index = 0;
  menu_button.max_index = index;
  
  index++;

  ui_menu.sum_list_menu = index;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();    
}

static void func_change_diva_mode(void)
{
    if(menu_button.buton_cb[0].value)
    {
        menu_button.buton_cb[0].value = 0; 
        memset(&ui_menu.list_menu[0]._char[0],0,25);
        memcpy(&ui_menu.list_menu[0]._char[0],"PPO2 Mode",9);
        ui_menu.update.Bit.index_listview = 1;
    }
    else
    {
        menu_button.buton_cb[0].value = 1; 
        memset(&ui_menu.list_menu[0]._char[0],0,25);
        memcpy(&ui_menu.list_menu[0]._char[0],"SetPoint Mode",13);
        ui_menu.update.Bit.index_listview = 1;
    }
}

static void func_change_buddy_leds(void)
{
    if(menu_button.buton_cb[1].value)
    {
        menu_button.buton_cb[1].value = 0; 
        memset(&ui_menu.list_menu[1]._char[0],0,25);
        memcpy(&ui_menu.list_menu[1]._char[0],"Disable Buddy Leds",18);
        ui_menu.update.Bit.index_listview = 1;
    }
    else
    {
        menu_button.buton_cb[1].value = 1; 
        memset(&ui_menu.list_menu[1]._char[0],0,25);
        memcpy(&ui_menu.list_menu[1]._char[0],"Enabled Buddy Leds",18);
        ui_menu.update.Bit.index_listview = 1;
    }
}

static void func_change_light_opt(void)
{
    if(menu_button.buton_cb[2].value)
    {
        menu_button.buton_cb[2].value = 0; 
        memset(&ui_menu.list_menu[2]._char[0],0,25);
        memcpy(&ui_menu.list_menu[2]._char[0],"Hide Light OPT",14);
        ui_menu.update.Bit.index_listview = 1;
    }
    else
    {
        menu_button.buton_cb[2].value = 1; 
        memset(&ui_menu.list_menu[2]._char[0],0,25);
        memcpy(&ui_menu.list_menu[2]._char[0],"Show Light OPT",14);
        ui_menu.update.Bit.index_listview = 1;
    }
}

static void func_change_signal_opt(void)
{
    if(menu_button.buton_cb[3].value)
    {
        menu_button.buton_cb[3].value = 0; 
        memset(&ui_menu.list_menu[3]._char[0],0,25);
        memcpy(&ui_menu.list_menu[3]._char[0],"Hide Signal OPT",17);
        ui_menu.update.Bit.index_listview = 1;
    }
    else
    {
        menu_button.buton_cb[3].value = 1; 
        memset(&ui_menu.list_menu[3]._char[0],0,25);
        memcpy(&ui_menu.list_menu[3]._char[0],"Show Signal OPT",17);
        ui_menu.update.Bit.index_listview = 1;
    }
}

static void func_change_diver_led(void)
{
    menu_button.buton_cb[4].value += 10;
    if(menu_button.buton_cb[4].value > 100)
    {
        menu_button.buton_cb[4].value = 0;
    }
    
    sprintf(&ui_menu.list_menu_value[4]._char[0],"%d",menu_button.buton_cb[4].value);
    ui_menu.update.Bit.index_listview  = 1;
}

static void func_change_deco_led(void)
{
    menu_button.buton_cb[5].value +=10;
    if(menu_button.buton_cb[5].value > 100)
    {
        menu_button.buton_cb[5].value = 0;
    }
    
    sprintf(&ui_menu.list_menu_value[5]._char[0],"%d",menu_button.buton_cb[5].value);
    ui_menu.update.Bit.index_listview  = 1;
}

static void func_change_light_led(void)
{
    menu_button.buton_cb[6].value += 10;
    if(menu_button.buton_cb[6].value > 100)
    {
        menu_button.buton_cb[6].value = 0;
    }
    
    sprintf(&ui_menu.list_menu_value[6]._char[0],"%d",menu_button.buton_cb[6].value);
    ui_menu.update.Bit.index_listview  = 1;
}

static void func_change_time_out(void)
{
      menu_button.buton_cb[7].value =  menu_button.buton_cb[7].value >= timeout_stepSizeJump_sec ?
                                         menu_button.buton_cb[7].value + timeout_largeStepSize_sec :
                                         menu_button.buton_cb[7].value + timeout_stepSize_sec
                                       ;
  
     menu_button.buton_cb[8].value  = menu_button.buton_cb[8].value >= timeout_stepSizeJump_sec ?
                                        menu_button.buton_cb[8].value + timeout_largeStepSize_sec :
                                        menu_button.buton_cb[8].value + timeout_stepSize_sec
                                       ;
    if( menu_button.buton_cb[7].value > timeout_max_sec )
                    menu_button.buton_cb[7].value = timeout_min_sec;
    if( menu_button.buton_cb[8].value > timeout_max_sec )
                    menu_button.buton_cb[8].value = timeout_min_sec;

    sprintf(&ui_menu.list_menu_value[7]._char[0],"%d",menu_button.buton_cb[7].value);
    
    if(menu_button.buton_cb[8].value > timeout_max_turnOff_sec)
    {
       memcpy(&ui_menu.list_menu_value[8]._char[0],"inf",3);
    }
    else
    {
        sprintf(&ui_menu.list_menu_value[8]._char[0],"%d",menu_button.buton_cb[8].value);
    }
    
    ui_menu.update.Bit.index_listview_value = 1;
}
static void func_change_setpoit_blink_repeat(void)
{
    menu_button.buton_cb[9].value++;
    
    if( menu_button.buton_cb[9].value > number_repeat_max)
    {
        menu_button.buton_cb[9].value = number_repeat_min;
    }
    
    sprintf(&ui_menu.list_menu_value[9]._char[0],"%d",menu_button.buton_cb[9].value);
}

void config_diva_menu(void)
{
  uint8_t index = 0;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "DIVA CONFIG MENU", 16);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  /*setting Metric units*/
  if(config_data.user_setting.settings.flags.divaMode)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"SetPoint Mode",13);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"PPO2 Mode",9);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_diva_mode;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.divaMode;
  index++;

  /*setting Buddy Leds*/
  if(config_data.user_setting.settings.flags.enableBuddyLeds)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Enabled Buddy Leds",18);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Disable Buddy Leds",18);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_buddy_leds;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.enableBuddyLeds;
  index++;

  /*setting Light Opt*/
  if(config_data.user_setting.settings.flags.flashlightOptShow)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Hide Light OPT",14);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Show Light OPT",14);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_light_opt;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.flashlightOptShow;
  index++;

  /*setting signal Opt*/
  if(config_data.user_setting.settings.flags.signalOptShow)
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Hide Signal OPT",15);
  }
  else
  {
    memcpy(&ui_menu.list_menu[index]._char[0],"Show Signal OPT",15);
  }
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_signal_opt;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.signalOptShow;
  index++;

  /*setting led brightness*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Diver Led %",11);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",config_data.user_setting.diveLedBrightness_percent);
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_diver_led;
  menu_button.buton_cb[index].value = config_data.user_setting.diveLedBrightness_percent;
  index++;

  /*setting deco led brightness*/
  //sprintf(&ui_menu.list_menu[index]._char[0],"Deco Led '%' %d",config_data.user_setting.decoLedBrightness_percent);
  memcpy(&ui_menu.list_menu[index]._char[0],"Deco Led %",11);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",config_data.user_setting.decoLedBrightness_percent);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_deco_led;
  menu_button.buton_cb[index].value = config_data.user_setting.decoLedBrightness_percent;
  index++;

/*setting light brightness*/
  //sprintf(&ui_menu.list_menu[index]._char[0],"Light '%' %d",config_data.user_setting.flashlightBrightness_percent);
  memcpy(&ui_menu.list_menu[index]._char[0],"Light %",11);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",config_data.user_setting.flashlightBrightness_percent);
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_light_led;
  menu_button.buton_cb[index].value = config_data.user_setting.flashlightBrightness_percent;
  index++;

  /*setting light timeout*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Light Time s",11);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",config_data.user_setting.flashlightTimeout_sec);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = NULL;
  menu_button.buton_cb[index].value = config_data.user_setting.flashlightTimeout_sec;
  index++;
  /*setting light timeout*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Signal Leds Time s",25);
  if(config_data.user_setting.divaSigLedTimeout_sec > timeout_max_turnOff_sec)
  {
    memcpy(&ui_menu.list_menu_value[index]._char[0],"inf",3);
  }
  else
  {
    sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",config_data.user_setting.divaSigLedTimeout_sec);
  }
  
 
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_time_out;
  menu_button.buton_cb[index].value = config_data.user_setting.divaSigLedTimeout_sec;
  index++;
  
  /*setting setpoint time repeat*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Sp Time Repeat s",16);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",config_data.user_setting.settings.flags.divaSetpointBlinkRepeat_sec);
  
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_setpoit_blink_repeat;
  menu_button.buton_cb[index].value = config_data.user_setting.settings.flags.divaSetpointBlinkRepeat_sec;
  index++;

  /*setting saveconfig*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Save Config",11);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_save_config;
  index++;

  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Cancel",6);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = return_to_main_screen;
  
  menu_button.current_index = 0;
  menu_button.max_index = index;
  
  index++;

  ui_menu.sum_list_menu = index;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();    
}

static uint32_t flashlightOnTime_s;
static uint32_t signalLedsOnTime_s;
void diva_timeout_update()
{
    if(config_data.user_setting.settings.flags.enableFlashlight)
    {
        flashlightOnTime_s++;

        if(config_data.user_setting.flashlightTimeout_sec != timeout_max_sec
           && flashlightOnTime_s >= (config_data.user_setting.flashlightTimeout_sec)
           )
        {
            config_data.user_setting.settings.flags.enableFlashlight = 0;
        }
    }
    else
    {
      flashlightOnTime_s = 0;
    }

    if(config_data.user_setting.settings.flags.enableSignalLeds)
    {
        signalLedsOnTime_s++;

        if(config_data.user_setting.divaSigLedTimeout_sec != timeout_max_sec
           && signalLedsOnTime_s >= (config_data.user_setting.divaSigLedTimeout_sec)
           )
        {
            config_data.user_setting.settings.flags.enableSignalLeds = 0;
        }
    }
    else
    {
      signalLedsOnTime_s = 0;
    }
}
static void func_change_hour(void)
{
    hour++;
    if(hour > 23)
    {
        hour = 0;
    }
    sprintf(&ui_menu.list_menu_value[0]._char[0],"%d",hour);
    ui_menu.update.Bit.index_listview = 1;
}

static void func_change_minute(void)
{
    minute++;
    if(minute > 59)
    {
        minute = 0;
    }
    sprintf(&ui_menu.list_menu_value[1]._char[0],"%d",minute);
    ui_menu.update.Bit.index_listview = 1;
}

static void func_change_second(void)
{
    second++;
    if(second > 59)
    {
        second = 0;
    }
    sprintf(&ui_menu.list_menu_value[2]._char[0],"%d",second);
    ui_menu.update.Bit.index_listview = 1;
}

static void func_change_day(void)
{
    date++;
    if(date > 31)
    {
        date = 1;
    }
    sprintf(&ui_menu.list_menu_value[1]._char[0],"%d",date);
    ui_menu.update.Bit.index_listview = 1;
}

static void func_change_month(void)
{
    month++;
    if(month > 12)
    {
        month = 1;
    }
    sprintf(&ui_menu.list_menu_value[0]._char[0],"%d",month);
    ui_menu.update.Bit.index_listview = 1;
}

static void func_change_year(void)
{
    year++;
    if(year > 99)
    {
        year = 0;
    }
    sprintf(&ui_menu.list_menu_value[2]._char[0],"%d",year);
    ui_menu.update.Bit.index_listview = 1;
}

static void func_confirm_time(void)
{
    flag_setting_time = 1;
    set_date_time();
}

void func_change_time(void)
{
  uint8_t index = 0;
  ui_menu.index_listview = 0;
  flag_setting_time = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "SETTING TIME", 13);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  /*setting time*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Hour",4);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",hour);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_hour;
  menu_button.buton_cb[index].value = hour;
  index++;
  /*setting time*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Minute",6);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",minute);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_minute;
  menu_button.buton_cb[index].value = minute;
  index++;
  /*setting time*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Second",6);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",second);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_second;
  menu_button.buton_cb[index].value = second;
  index++;
  
  /*setting ok*/
  memcpy(&ui_menu.list_menu[index]._char[0],"OK",2);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_confirm_time;
  index++;

  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Cancel",6);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = set_date_time;
  
  menu_button.current_index = 0;
  menu_button.max_index = index;
  
  index++;

  ui_menu.sum_list_menu = index;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();    
}

static void func_save_date_time(void)
{
    flag_setting_time = 0;
    flag_setting_date = 0;
    
    sTime.Hours = hour;
    sTime.Minutes = minute;
    sTime.Seconds = second;
    sDate.Date = date;
    sDate.Month = month;
    sDate.Year = year;
    HAL_RTC_SetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
    
    return_to_main_screen();
     
}

static void func_cancel_date_time(void)
{
    flag_setting_time = 0;
    flag_setting_date = 0;
    return_to_main_screen();
}

static void func_confirm_date(void)
{
    flag_setting_date = 1;
    set_date_time();
}

static void func_change_date(void)
{
  uint8_t index = 0;
  ui_menu.index_listview = 0;
  flag_setting_date = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "SETTING TIME", 13);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  
  /*setting time*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Month",5);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",month);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_month;
  menu_button.buton_cb[index].value = month;
  index++;
  
  /*setting time*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Day",4);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",date);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_day;
  menu_button.buton_cb[index].value = date;
  index++;
  
  /*setting time*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Year",4);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%d",year);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_year;
  menu_button.buton_cb[index].value = year;
  index++;

  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"OK",2);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_confirm_date;
  index++;
     
  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Cancel",6);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = set_date_time;
  
  menu_button.current_index = 0;
  menu_button.max_index = index;
  
  index++;

  ui_menu.sum_list_menu = index;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();    
}

void set_date_time(void)
{
 if(flag_setting_time == 0)
  {
//    RTC_TimeTypeDef sTime;
//    RTC_DateTypeDef sDate;
   // flag_setting_time = 1;
    HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
   // HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
    hour = sTime.Hours;
    minute = sTime.Minutes;
    second = sTime.Seconds;
   // date = sDate.Date;
   // month = sDate.Month;
   // year = sDate.Year;    
  }
  if(flag_setting_date == 0)
  {
//    RTC_TimeTypeDef sTime;
//    RTC_DateTypeDef sDate;
   // flag_setting_date = 1;
  //  HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
  //  hour = sTime.Hours;
  //  minute = sTime.Minutes;
  //  second = sTime.Seconds;
    date = sDate.Date;
    month = sDate.Month;
    year = sDate.Year;    
  }
  uint8_t index = 0;
  ui_menu.index_listview = 0;
  memset(ui_menu.header_menu._char, 0, 25);
  memset(&ui_menu.list_menu[0],0,NUM_ROW*NUM_COL);
  memset(&ui_menu.list_menu_value[0],0,NUM_ROW*NUM_COL);
  memcpy(ui_menu.header_menu._char, "SET DATE TIME", 13);
  menu_button.current_index = 0;
  menu_button.max_index = 0;
  /*setting time*/
  memcpy(&ui_menu.list_menu[index]._char[0],"TIME (hh/mm/ss)",15);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%02d:%02d:%02d",hour,minute,second);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_time;
 // menu_button.buton_cb[index].value = hour;
  index++;
  /*setting date*/
  memcpy(&ui_menu.list_menu[index]._char[0],"DATE (mm/dd/yy)",15);
  sprintf(&ui_menu.list_menu_value[index]._char[0],"%02d/%02d/%d",month,date,year);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_change_date;
  //menu_button.buton_cb[index].value = ;
  index++;



  /*setting saveconfig*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Save Config",11);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_save_date_time;
  index++;

  /*setting cancel*/
  memcpy(&ui_menu.list_menu[index]._char[0],"Cancel",6);
  menu_button.buton_cb[index].f_main = func_menu;
  menu_button.buton_cb[index].f_tap = func_cancel_date_time;
  
  menu_button.current_index = 0;
  menu_button.max_index = index;
  
  index++;

  ui_menu.sum_list_menu = index;
  ui_menu.update.Bit.index_listview = 1;
  ui_menu.update.Bit.header_menu =1;
  UI_menu_update();
  UI_menu_loadscreen();      
}

void rotate_screen(uint32_t screen_rotate)
{
  if(screen_rotate == 1)
  {
    lv_disp_set_rotation(NULL,LV_DISP_ROT_90);
  }
  else if(screen_rotate == 0)
  {
    lv_disp_set_rotation(NULL,LV_DISP_ROT_270);
  }
}