#include "ui_main.h"
#include "ui_img_storage.h"
#include "ui_control.h"
#include "main_process.h"
#include "flash_process.h"
#include <stdio.h>
const uint16_t main_widget_posY[5] = {5, 67, 111, 157, 205};
const uint16_t hight_header = 21;

char buff[7];

extern saving_data_t config_data;

lv_obj_t *screen_main;

lv_obj_t *lb_main_header_BG[4], *lb_main_header[4];
lv_obj_t *lb_main_time[3];
lv_obj_t *lb_main_sensor_BG[3], *lb_main_sensor[3];
lv_obj_t *lb_main_config[5];
lv_obj_t *lb_main_system[4];
lv_obj_t *lb_main_button[2];
lv_obj_t *img_main_up[3], *img_main_down[3];

ui_main_t       ui_main;

uint8_t array_value_main[6];
uint8_t times_blink_main = 0;

void ui_main_header(void)
{
  /****************************** HEADER ******************************/
  // Create style
  static lv_style_t style_main_header;
  lv_style_init(&style_main_header);
  lv_style_set_text_font(&style_main_header, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_main_header, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  
  // lb_header_1 - Sensor
  lb_main_header_BG[0] = lv_obj_create(screen_main, NULL);
  lb_main_header[0] = lv_label_create(lb_main_header_BG[0], NULL);
  lv_label_set_long_mode(lb_main_header[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_main_header[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_main_header[0], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_main_header[0], "Sen 1           Sen 2          Sen 3");
  lv_obj_set_width(lb_main_header_BG[0], 298);
  lv_obj_set_height(lb_main_header_BG[0], hight_header);
  lv_obj_set_width(lb_main_header[0], 298);
  lv_obj_add_style(lb_main_header[0], LV_LABEL_PART_MAIN , &style_main_header);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[0], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  }
  else
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[0], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  }
  lv_obj_align(lb_main_header_BG[0], NULL, LV_ALIGN_IN_TOP_LEFT, 10, main_widget_posY[0]);
  lv_obj_align(lb_main_header[0], NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0); // in Parent
  
  // lb_header_2 - Time
  lb_main_header_BG[1] = lv_obj_create(screen_main, lb_main_header_BG[0]);
  lb_main_header[1] = lv_label_create(lb_main_header_BG[1], lb_main_header[0]);
  lv_label_set_text(lb_main_header[1], "S Time    |    D Time    |    Max D");
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[1], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  }
  else
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[1], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 80, 80));
  }
  lv_obj_align(lb_main_header_BG[1], NULL, LV_ALIGN_IN_TOP_LEFT, 10, main_widget_posY[1]);
  
  // lb_header_3 - Config
  lb_main_header_BG[2] = lv_obj_create(screen_main, lb_main_header_BG[0]);
  lb_main_header[2] = lv_label_create(lb_main_header_BG[2], lb_main_header[0]);
  lv_label_set_text(lb_main_header[2], " Bat  | Temp | Gas | Point | ppO2");
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[2], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  }
  else
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[2], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
  }
  lv_obj_align(lb_main_header_BG[2], NULL, LV_ALIGN_IN_TOP_LEFT, 10, main_widget_posY[2]);
  
  // lb_header_4 - System
  lb_main_header_BG[3] = lv_obj_create(screen_main, lb_main_header_BG[0]);
  lb_main_header[3] = lv_label_create(lb_main_header_BG[3], lb_main_header[0]);
   lv_label_set_align(lb_main_header[3], LV_LABEL_ALIGN_LEFT);
  lv_label_set_text(lb_main_header[3], "GF L/H|TTS|Stk tm| NOSTOP");
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[3], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  }
  else
  {
    lv_obj_set_style_local_bg_color(lb_main_header_BG[3], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 80, 0));
  }
  
  lv_obj_align(lb_main_header_BG[3], NULL, LV_ALIGN_IN_TOP_LEFT, 10, main_widget_posY[3]);
  
  /****************************** TABLE ******************************/
  // Create style
  static lv_style_t style_main_line;
  lv_style_init(&style_main_line);
  lv_style_set_line_width(&style_main_line, LV_STATE_DEFAULT, 2);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_style_set_line_color(&style_main_line, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  }
  else
  {
    lv_style_set_line_color(&style_main_line, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  }
  
  lv_style_set_line_rounded(&style_main_line, LV_STATE_DEFAULT, true);
  // Table
  static lv_point_t line_points_main[] = { {0, 0}, {294, 0}, {294, 35}, {0, 35}, {0, 0},
                                      {196, 0}, {196, 35}, {98, 35}, {98, 0},  };
  lv_obj_t * line1;
  line1 = lv_line_create(screen_main, NULL);
  lv_line_set_points(line1, line_points_main, 9);     /*Set the points*/
  lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_main_line);     /*Set the points*/
  lv_obj_align(line1, NULL, LV_ALIGN_IN_TOP_LEFT, 12, main_widget_posY[0] + (hight_header - 1));
}

void ui_main_sensor(void)
{
  /******************************* SENSOR ******************************/
  // Create style Label
  static lv_style_t style_main_sensor;
  lv_style_init(&style_main_sensor);
  lv_style_set_text_font(&style_main_sensor, LV_STATE_DEFAULT, &lv_font_montserrat_30);
  if(config_data.user_setting.settings.flags.darkmode)
  {
      lv_style_set_text_color(&style_main_sensor, LV_STATE_DEFAULT, LV_COLOR_WHITE);
      lv_style_set_text_color(&style_main_sensor, LV_STATE_FOCUSED, LV_COLOR_BLACK);
  }
  else
  {
      lv_style_set_text_color(&style_main_sensor, LV_STATE_DEFAULT, LV_COLOR_NAVY);
      lv_style_set_text_color(&style_main_sensor, LV_STATE_FOCUSED, LV_COLOR_WHITE);
  }
  
  static lv_style_t style_main_sensor_BG;
  lv_style_init(&style_main_sensor_BG);
  if(config_data.user_setting.settings.flags.darkmode)
  {
      lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
      lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_FOCUSED, LV_COLOR_WHITE);    
  }
  else
  {
      lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
      lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_FOCUSED, LV_COLOR_GRAY);
  }

  
  // Sensor 1
  lb_main_sensor_BG[0] = lv_obj_create(screen_main, NULL);
  lb_main_sensor[0] = lv_label_create(lb_main_sensor_BG[0], NULL);
  lv_label_set_long_mode(lb_main_sensor[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_main_sensor[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_main_sensor[0], LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
  lv_label_set_text(lb_main_sensor[0], "0.96");
  lv_obj_set_width(lb_main_sensor_BG[0], 96);
	lv_obj_set_height(lb_main_sensor_BG[0], 33);
  lv_obj_set_width(lb_main_sensor[0], 96);
  lv_obj_add_style(lb_main_sensor_BG[0], LV_LABEL_PART_MAIN , &style_main_sensor_BG);
  lv_obj_add_style(lb_main_sensor[0], LV_LABEL_PART_MAIN , &style_main_sensor);
  lv_obj_align(lb_main_sensor_BG[0], NULL, LV_ALIGN_IN_TOP_LEFT, 13, main_widget_posY[0] + hight_header);
  lv_obj_align(lb_main_sensor[0], NULL, LV_ALIGN_IN_TOP_LEFT, 18, 1); // in Parent

  // Sensor 2,3
  for(uint8_t iSS=1; iSS<3; iSS++)
  {
    lb_main_sensor_BG[iSS] = lv_obj_create(screen_main, lb_main_sensor_BG[0]);
    lb_main_sensor[iSS] = lv_label_create(lb_main_sensor_BG[iSS], lb_main_sensor[0]);
    lv_label_set_text(lb_main_sensor[iSS], "0.94");
    lv_obj_align(lb_main_sensor_BG[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 13 + 98 * iSS, main_widget_posY[0] + hight_header);
  }
  
  /******************************* ARROW ******************************/
  // Create image Up/Down
  static lv_img_dsc_t img_arrow_up = {
    .header.always_zero = 0,
    .header.w = 11,
    .header.h = 21,
    .data_size = 11 * 21 * LV_COLOR_DEPTH / 8,
    .header.cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,          /*Set the color format*/
    .data = img_arrow_up_11x21,
  };
  static lv_img_dsc_t img_arrow_down = {
    .header.always_zero = 0,
    .header.w = 11,
    .header.h = 21,
    .data_size = 11 * 21 * LV_COLOR_DEPTH / 8,
    .header.cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,          /*Set the color format*/
    .data = img_arrow_down_11x21,
  };
  
  // Arrow
  for(uint8_t iSS=0; iSS<3; iSS++)
  {
    // Up
    img_main_up[iSS] = lv_img_create(lb_main_sensor_BG[iSS], NULL);
    lv_img_set_src(img_main_up[iSS], &img_arrow_up);
    lv_obj_align(img_main_up[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 78, 7);
    // Down
    img_main_down[iSS] = lv_img_create(lb_main_sensor_BG[iSS], NULL);
    lv_img_set_src(img_main_down[iSS], &img_arrow_down);
    lv_obj_align(img_main_down[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 78, 7);
  }
}

void ui_main_time(void)
{
  // Create style
  static lv_style_t style_main_time;
  lv_style_init(&style_main_time);
  lv_style_set_text_font(&style_main_time, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_style_set_text_color(&style_main_time, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  }
  else
  {
     lv_style_set_text_color(&style_main_time, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 80, 80));
  }
   
  // S Time
  lb_main_time[0] = lv_label_create(screen_main, NULL);
  lv_label_set_long_mode(lb_main_time[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_main_time[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_main_time[0], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_main_time[0], "303");
  lv_obj_set_width(lb_main_time[0], 98);
  lv_obj_add_style(lb_main_time[0], LV_LABEL_PART_MAIN , &style_main_time);
  lv_obj_align(lb_main_time[0], NULL, LV_ALIGN_IN_TOP_LEFT, 10, main_widget_posY[1] + hight_header);
  
  // D Time
  lb_main_time[1] = lv_label_create(screen_main, lb_main_time[0]);
  lv_label_set_text(lb_main_time[1], "0");
  lv_obj_align(lb_main_time[1], NULL, LV_ALIGN_IN_TOP_LEFT, 10 + 100 * 1, main_widget_posY[1] + hight_header);
  
  // Max D
  lb_main_time[2] = lv_label_create(screen_main, lb_main_time[0]);
  lv_label_set_text(lb_main_time[2], "171ft");
  lv_obj_align(lb_main_time[2], NULL, LV_ALIGN_IN_TOP_LEFT, 10 + 100 * 2, main_widget_posY[1] + hight_header);
}

void ui_main_config(void)
{
  static lv_style_t style_main_sensor_BG;
  lv_style_init(&style_main_sensor_BG);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_FOCUSED, LV_COLOR_WHITE);
  }
  else
  {
    lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_color(&style_main_sensor_BG, LV_STATE_FOCUSED, LV_COLOR_GRAY);
  }
  // Create style
  static lv_style_t style_main_config;
  lv_style_init(&style_main_config);
  lv_style_set_text_font(&style_main_config, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_style_set_text_color(&style_main_config, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_color(&style_main_config, LV_STATE_FOCUSED, LV_COLOR_BLACK);
  }
  else
  {
    lv_style_set_text_color(&style_main_config, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_text_color(&style_main_config, LV_STATE_FOCUSED, LV_COLOR_RED);
  }
 
  // Battery
  lb_main_config[0] = lv_label_create(screen_main, NULL);
  lv_label_set_long_mode(lb_main_config[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_main_config[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_main_config[0], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_main_config[0], "5.7V");
  lv_obj_set_width(lb_main_config[0], 75);
  lv_obj_add_style(lb_main_config[0], LV_LABEL_PART_MAIN , &style_main_config);
  lv_obj_align(lb_main_config[0], NULL, LV_ALIGN_IN_TOP_LEFT, 5, main_widget_posY[2] + hight_header);
  
  // Temperature
  lb_main_config[1] = lv_label_create(screen_main, lb_main_config[0]);
  lv_label_set_text(lb_main_config[1], "87�F");
  lv_obj_align(lb_main_config[1], NULL, LV_ALIGN_IN_TOP_LEFT, 10 + 55 * 1, main_widget_posY[2] + hight_header);
  
  // Gas select
  lb_main_config[2] = lv_label_create(screen_main, lb_main_config[0]);
  lv_label_set_text(lb_main_config[2], "21/0"); 
  lv_obj_align(lb_main_config[2], NULL, LV_ALIGN_IN_TOP_LEFT, 10 + 55 * 2, main_widget_posY[2] + hight_header);
  
  // SetPoint // 0.40 - 0.70 - 1.00 - 1.20
  lb_main_config[3] = lv_label_create(screen_main, lb_main_config[0]);
  lv_label_set_text(lb_main_config[3], "0.40");
  lv_obj_align(lb_main_config[3], NULL, LV_ALIGN_IN_TOP_LEFT, 10 + 55 * 3, main_widget_posY[2] + hight_header);
  
  // ppo2 // 0.40 - 0.70 - 1.00 - 1.20
  lb_main_config[4] = lv_label_create(screen_main, lb_main_config[0]);
  lv_label_set_text(lb_main_config[4], "0.40");
  lv_obj_align(lb_main_config[4], NULL, LV_ALIGN_IN_TOP_LEFT,  60 * 4, main_widget_posY[2] + hight_header);
}

void ui_main_system(void)
{
  // Create style
  static lv_style_t style_main_system;
  lv_style_init(&style_main_system);
  lv_style_set_text_font(&style_main_system, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_style_set_text_color(&style_main_system, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  }
  else
  {
    lv_style_set_text_color(&style_main_system, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 80, 0));
  }
 
  // Low / High of xxx
  lb_main_system[0] = lv_label_create(screen_main, NULL);
  lv_label_set_long_mode(lb_main_system[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_main_system[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_main_system[0], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_main_system[0], "30/70");
  lv_obj_set_width(lb_main_system[0], 100);
  lv_obj_add_style(lb_main_system[0], LV_LABEL_PART_MAIN , &style_main_system);
  lv_obj_align(lb_main_system[0], NULL, LV_ALIGN_IN_TOP_LEFT, 0, main_widget_posY[3] + hight_header);
  
  // TTS value
  lb_main_system[1] = lv_label_create(screen_main, lb_main_system[0]);
  lv_label_set_text(lb_main_system[1], "0");
  lv_obj_align(lb_main_system[1], NULL, LV_ALIGN_IN_TOP_LEFT, 60, main_widget_posY[3] + hight_header);
  
  // stack value
  lb_main_system[2] = lv_label_create(screen_main, lb_main_system[0]);
  lv_label_set_text(lb_main_system[2], "0.21"); 
  lv_obj_align(lb_main_system[2], NULL, LV_ALIGN_IN_TOP_LEFT, 115, main_widget_posY[3] + hight_header);
  
  //decomode
  lb_main_system[3] = lv_label_create(screen_main, lb_main_system[0]);
  lv_label_set_text(lb_main_system[3], "310"); 
  lv_obj_align(lb_main_system[3], NULL, LV_ALIGN_IN_TOP_LEFT,190, main_widget_posY[3] + hight_header);
}

void ui_main_button(void)
{
  lv_obj_t *main_button[2];
  
  /*Create style*/
  static lv_style_t style_main_button;
  lv_style_init(&style_main_button);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    lv_style_set_bg_color(&style_main_button, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_color(&style_main_button, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  }
  else
  {
    lv_style_set_bg_color(&style_main_button, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x40, 0x00));
    lv_style_set_text_color(&style_main_button, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  }
  
  
  // button Function
  main_button[0] = lv_btn_create(screen_main, NULL);
  lv_obj_set_size(main_button[0], 180, 30);
  lv_obj_add_style(main_button[0], LV_BTN_PART_MAIN, &style_main_button);
  lv_obj_set_pos(main_button[0], 10, main_widget_posY[4]);
  lb_main_button[0] = lv_label_create(main_button[0], NULL);
  lv_obj_set_style_local_text_color(lb_main_button[0], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text(lb_main_button[0], "Function");
  
  // button Static - Bright
  main_button[1] = lv_btn_create(screen_main, NULL);
  lv_obj_set_size(main_button[1], 80, 30);
  lv_obj_add_style(main_button[1], LV_BTN_PART_MAIN, &style_main_button);
  lv_obj_set_pos(main_button[1], 230, main_widget_posY[4]);
  lb_main_button[1] = lv_label_create(main_button[1], NULL);
  lv_obj_set_style_local_text_color(lb_main_button[1], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text(lb_main_button[1], "Bright");
}

void UI_main_update(void)
{
  if(ui_main.update.Bit.sensor_data)
  {
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      memset(array_value_main,0,6);
      ui_convert_to_sensor(array_value_main, ui_main.sensor[iSS].data);
      lv_label_set_text(lb_main_sensor[iSS], (char*)&array_value_main);
    }
  }
  if(ui_main.update.Bit.sensor_blink)
  {
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      if(ui_main.sensor[iSS].blink == 0)
      {
        lv_obj_set_state(lb_main_sensor_BG[iSS], LV_STATE_DEFAULT);
        lv_obj_set_state(lb_main_sensor[iSS], LV_STATE_DEFAULT);
      }
    }
  }
  if(ui_main.update.Bit.sensor_arrow)
  {
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      if(ui_main.sensor[iSS].arrow == ARROW_UP)
      {
        lv_obj_set_hidden(img_main_up[iSS], false);
        lv_obj_set_hidden(img_main_down[iSS], true);
        lv_obj_align(lb_main_sensor[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 10, 1);
      }
      else if(ui_main.sensor[iSS].arrow == ARROW_DOWN)
      {
        lv_obj_set_hidden(img_main_up[iSS], true);
        lv_obj_set_hidden(img_main_down[iSS], false);
        lv_obj_align(lb_main_sensor[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 10, 1);
      }
      else
      {
        lv_obj_set_hidden(img_main_up[iSS], true);
        lv_obj_set_hidden(img_main_down[iSS], true);
        lv_obj_align(lb_main_sensor[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 18, 1);
      }  
    }
  }
  if(ui_main.update.Bit.surface_time)
  {
    if(ui_main.dive_state == 1)
    {
        lv_label_set_text(lb_main_header[1], "Cur D     |    D Time    |    Max D");
         if(ui_main.units_type)
         {
          lv_label_set_text_fmt(lb_main_time[0],"%d.%dm",ui_main.current_distance/10,ui_main.current_distance%10);
         }
         else
         {
           lv_label_set_text_fmt(lb_main_time[0],"%dft",ui_main.current_distance);
         }
    }
    else
    {
        lv_label_set_text(lb_main_header[1], "S Time    |    D Time    |    Max D");
        memset(array_value_main,0,6);
        ui_convert_to_dive_time(array_value_main, ui_main.surface_time);
        lv_label_set_text(lb_main_time[0],(char*)&array_value_main);
    }
  }
  if(ui_main.update.Bit.dive_time)
  {
    
    if(ui_main.dive_state == 1)
    {
        uint8_t array_time[9];
        ui_convert_to_diving_time(array_time,ui_main.dive_time);
        lv_label_set_text(lb_main_time[1], (char*)&array_time);
    }
    else
    {
       memset(array_value_main,0,6);
       ui_convert_to_dive_time(array_value_main, ui_main.dive_time);
       lv_label_set_text(lb_main_time[1], (char*)&array_value_main);
    }
  }
  if(ui_main.update.Bit.max_distance)
  {
    if(ui_main.units_type)
    {
        lv_label_set_text_fmt(lb_main_time[2],"%d.%dm",ui_main.max_distance/10,ui_main.max_distance%10);
    }
    else
    {
       lv_label_set_text_fmt(lb_main_time[2],"%dft",ui_main.max_distance);
    }
    //lv_obj_set_hidden(lb_main_time[2], false);
  }
  if(ui_main.update.Bit.batterry_data)
  {
    memset(array_value_main,0,6);
    array_value_main[0] = '0' + ui_main.batterry.data % 100 / 10;
    array_value_main[1] = '.';
    array_value_main[2] = '0' + ui_main.batterry.data % 10 / 1;
    array_value_main[3] = 'V';
    array_value_main[4] = 0;
    lv_label_set_text(lb_main_config[0], (char*)&array_value_main);
    //lv_obj_set_hidden(lb_main_config[0], false);
  }
  if(ui_main.update.Bit.temperature)
  {
    if(ui_main.temp_press_warning)
    {
        lv_label_set_text(lb_main_config[1], "****");
    }
    else
    {
        if(ui_main.temp_type)
        {
            lv_label_set_text_fmt(lb_main_config[1],"%d \xC2\xB0\x43",ui_main.temperature);
        }
        else
        {
           lv_label_set_text_fmt(lb_main_config[1],"%d \xC2\xB0\x46",ui_main.temperature); 
        }
       // lv_obj_set_hidden(lb_main_config[1], false);
    }
     
  }
  if(ui_main.update.Bit.gas_mix)
  {
    //ui_convert_to_sensor(array_value_main, ui_main.gas_mix);
    memset(buff,0,7);
    sprintf(buff,"%d/%d",ui_main.gas_mix.O2,ui_main.gas_mix.he);
    lv_label_set_text(lb_main_config[2], buff);
  }
  if(ui_main.update.Bit.set_point)
  {
    memset(array_value_main,0,6);
    ui_convert_setpoint(array_value_main, ui_main.set_point.data, ui_main.mode);
    lv_label_set_text(lb_main_config[3], (char*)&array_value_main);
   // lv_obj_set_hidden(lb_main_config[3], false);
  }
  if(ui_main.update.Bit.xxx)
  {
    //ui_convert_to_sensor(array_value_main, ui_main.set_point);
    memset(buff,0,7);
    sprintf(buff,"%d/%d",ui_main.glow,ui_main.ghight);
    lv_label_set_text(lb_main_system[0], buff);
  }
  if(ui_main.update.Bit.time_to_surface)
  {
    if(ui_main.time_to_surface.blink)
    {
        lv_label_set_text(lb_main_system[1], "!!!");
    }
    else
    {
        memset(array_value_main,0,6);
        ui_convert_to_int3(array_value_main, ui_main.time_to_surface.data);
        lv_label_set_text(lb_main_system[1], (char*)&array_value_main);
    }
    // lv_obj_set_hidden(lb_main_system[1], false);
  }
  if(ui_main.update.Bit.ppo2)
  {
     memset(array_value_main,0,6);
     ui_convert_to_sensor(array_value_main, ui_main.ppo2);
     lv_label_set_text(lb_main_config[4], (char*)&array_value_main);
  }
  if(ui_main.update.Bit.stk_tm)
  {
    memset(array_value_main,0,6);
    ui_convert_to_int3(array_value_main, ui_main.stk_tm);
    lv_label_set_text(lb_main_system[2], (char*)&array_value_main);
  }
  if(ui_main.update.Bit.button_L)
  {
    lv_label_set_text(lb_main_button[0], ui_main.button_L);
  }
  if(ui_main.update.Bit.button_R)
  {
    lv_label_set_text(lb_main_button[1], ui_main.button_R);
  }
  if(ui_main.deco_mode)
  {
     lv_label_set_text(lb_main_header[3], " GF L/H  |TTS|Stk tm|  DECO");
     if(ui_main.units_type)
     {
       lv_label_set_text_fmt(lb_main_system[3], "%d @ %dm",ui_main.deco_time,ui_main.deco_depth);  
     }
     else
     {
       lv_label_set_text_fmt(lb_main_system[3], "%d @ %dft",ui_main.deco_time,ui_main.deco_depth);
     }
  }
  else
  {
     lv_label_set_text(lb_main_header[3], " GF L/H  |TTS|Stk tm|  NOSTOP");
     lv_label_set_text(lb_main_system[3], "NDL");
  }
  // reset bit
  {
  ui_main.update.Word = 0;
  }
}

void UI_main_timer_blink(void)
{
  if(screen_active_flag.Bit.main)
  {
    // sensor background
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      if(ui_main.sensor[iSS].blink)
      {
        if(times_blink_main == 0)
        {
          lv_obj_set_state(lb_main_sensor_BG[iSS], LV_STATE_FOCUSED);
          lv_obj_set_state(lb_main_sensor[iSS], LV_STATE_FOCUSED); 
        }
        else
        {
          lv_obj_set_state(lb_main_sensor_BG[iSS], LV_STATE_DEFAULT);
          lv_obj_set_state(lb_main_sensor[iSS], LV_STATE_DEFAULT);
        }
      }
    }
    // battery
    if(ui_main.batterry.blink)
    {
      if(times_blink_main == 0)
      {
        lv_obj_set_hidden(lb_main_config[0], false);
      }
      else
      {
        lv_obj_set_hidden(lb_main_config[0], true);
      }
    }
    else
    {
        lv_obj_set_hidden(lb_main_config[0], false);
    }
    
    // OC mode
    if(ui_main.set_point.blink)
    {
      if(times_blink_main == 0)
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_FOCUSED);
        lv_obj_set_hidden(lb_main_config[3], false);
      }
      else
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_DEFAULT);
        lv_obj_set_hidden(lb_main_config[3], true);
      }
    }
    else
    {
         lv_obj_set_hidden(lb_main_config[3], false);
    }
    
   // TTS mode
    if(ui_main.time_to_surface.blink)
    {
      if(times_blink_main == 0)
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_FOCUSED);
        lv_obj_set_hidden(lb_main_system[1], false);
      }
      else
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_DEFAULT);
        lv_obj_set_hidden(lb_main_system[1], true);
      }
    }
    else
    {
       lv_obj_set_hidden(lb_main_system[1], false); 
    }
    
    // STACKTIMER
    if(ui_main.stk_expired_warning)
    {
      if(times_blink_main == 0)
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_FOCUSED);
        lv_obj_set_hidden(lb_main_system[2], false);
      }
      else
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_DEFAULT);
        lv_obj_set_hidden(lb_main_system[2], true);
      }
    }
    else
    {
       lv_obj_set_hidden(lb_main_system[2], false); 
    }
    
    // temp and pressure mode
    if(ui_main.temp_press_warning)
    {
      if(times_blink_main == 0)
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_FOCUSED);
        lv_obj_set_hidden(lb_main_config[1], false);
        lv_obj_set_hidden(lb_main_time[2], false);
      }
      else
      {
       // lv_obj_set_state(lb_main_config[3], LV_STATE_DEFAULT);
        lv_obj_set_hidden(lb_main_config[1], true);
        lv_obj_set_hidden(lb_main_time[2], true);
      }
    }
    else
    {
        lv_obj_set_hidden(lb_main_config[1], false);
        lv_obj_set_hidden(lb_main_time[2], false);
    }
    //
    //
    if(times_blink_main == 0)
      times_blink_main = 1;
    else
      times_blink_main = 0;
  }
}

void UI_main_loadscreen(void)
{
  lv_scr_load(screen_main);
  screen_active_flag.Bit.main = 1;
  screen_active_flag.Bit.menu = 0;
  screen_active_flag.Bit.dialog = 0;
  screen_active_flag.Bit.diagnotic =0;
    screen_active_flag.Bit.compass = 0;
}

void UI_main_init(void)
{
  screen_main = lv_obj_create(NULL, NULL);
  if(config_data.user_setting.settings.flags.darkmode)
  {
    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(screen_main, LV_OBJ_PART_MAIN, &style_screen);
  }
  
  ui_main_header();
  ui_main_sensor();
  ui_main_time();
  ui_main_config();
  ui_main_system();
  ui_main_button();
  //list_button_init();
  
  
  // TEST
  ui_main.sensor[0].data = 123;
  ui_main.sensor[1].data = 456;
  ui_main.sensor[2].data = 789;
  ui_main.sensor[0].blink = 1;
  ui_main.sensor[1].blink = 1;
  ui_main.sensor[2].blink = 1;
  ui_main.sensor[0].arrow = ARROW_UP;
  ui_main.sensor[1].arrow = ARROW_NONE;
  ui_main.sensor[2].arrow = ARROW_DOWN;
  //
  ui_main.surface_time = 150;
  ui_main.dive_time = 340;
  ui_main.max_distance = 120;
  //
  ui_main.batterry.data = 58;
  ui_main.temperature = 78;
  ui_main.set_point.data = 120;
  ui_main.ppo2 = 40;
  //
  memcpy(ui_main.button_L, "Function", 8);
  memcpy(ui_main.button_R, "Bright", 6);
  //
  ui_main.update.Word = 0xffffffff;
  UI_main_update();
}

