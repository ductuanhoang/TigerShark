#include "ui_diagnotics.h"
#include <stdio.h>

const uint16_t diag_widget_posY[6] = {5, 45, 111, 132, 153, 205};
const uint16_t hight_header1 = 21;

//char buff[7];


lv_obj_t *screen_diagnotics;

lv_obj_t *lb_diagnotics_header_BG[5], *lb_diagnotics_header[5];
//lv_obj_t *lb_diagnotics_time[3];
lv_obj_t *lb_diagnotics_sensor_BG[6], *lb_diagnotics_sensor[6];
//lv_obj_t *lb_diagnotics_config[4];
//lv_obj_t *lb_diagnotics_system[4];
lv_obj_t *lb_diagnotics_button[2];
//lv_obj_t *img_diagnotics_up[3], *img_diagnotics_down[3];

ui_diagnotics_t       ui_diagnotics;

uint8_t array_value_diagnotics[6];
uint8_t times_blink_diagnotics = 0;

void ui_diagnotics_header(void)
{
  /****************************** HEADER ******************************/
  // Create style
  static lv_style_t style_diagnotics_header;
  lv_style_init(&style_diagnotics_header);
  lv_style_set_text_font(&style_diagnotics_header, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_diagnotics_header, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  
  //lb header
  lb_diagnotics_header_BG[1] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_header[1] = lv_label_create(lb_diagnotics_header_BG[1], NULL);
  lv_label_set_long_mode(lb_diagnotics_header[1], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_header[1], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_header[1], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_header[1], "DIAGNOSTICS SCREEN");
  lv_obj_set_width(lb_diagnotics_header_BG[1], 298);
	lv_obj_set_height(lb_diagnotics_header_BG[1], hight_header1);
  lv_obj_set_width(lb_diagnotics_header[1], 298);
  lv_obj_add_style(lb_diagnotics_header[1], LV_LABEL_PART_MAIN , &style_diagnotics_header);
  lv_obj_set_style_local_bg_color(lb_diagnotics_header_BG[1], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_obj_align(lb_diagnotics_header_BG[1], NULL, LV_ALIGN_IN_TOP_LEFT, 10, diag_widget_posY[0]);
  lv_obj_align(lb_diagnotics_header[1], NULL, LV_ALIGN_IN_TOP_LEFT, 1, 1); // in Parent
  
  
  // lb_header_1 - Sensor
  lb_diagnotics_header_BG[0] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_header[0] = lv_label_create(lb_diagnotics_header_BG[0], NULL);
  lv_label_set_long_mode(lb_diagnotics_header[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_header[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_header[0], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_header[0], "Sen 1 Mv    Sen 2 Mv    Sen 3 Mv");
  lv_obj_set_width(lb_diagnotics_header_BG[0], 298);
  lv_obj_set_height(lb_diagnotics_header_BG[0], hight_header1);
  lv_obj_set_width(lb_diagnotics_header[0], 298);
  lv_obj_add_style(lb_diagnotics_header[0], LV_LABEL_PART_MAIN , &style_diagnotics_header);
  lv_obj_set_style_local_bg_color(lb_diagnotics_header_BG[0], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_obj_align(lb_diagnotics_header_BG[0], NULL, LV_ALIGN_IN_TOP_LEFT, 10, diag_widget_posY[1]);
  lv_obj_align(lb_diagnotics_header[0], NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0); // in Parent
  
    // lb_header_2 - Sensor
  lb_diagnotics_header_BG[2] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_header[2] = lv_label_create(lb_diagnotics_header_BG[2], NULL);
  lv_label_set_long_mode(lb_diagnotics_header[2], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_header[2], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_header[2], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_header[2], "Barometer");
  lv_obj_set_width(lb_diagnotics_header_BG[2], 150);
  lv_obj_set_height(lb_diagnotics_header_BG[2], hight_header1);
  lv_obj_set_width(lb_diagnotics_header[2], 150);
  lv_obj_add_style(lb_diagnotics_header[2], LV_LABEL_PART_MAIN , &style_diagnotics_header);
  lv_obj_set_style_local_bg_color(lb_diagnotics_header_BG[2], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_obj_align(lb_diagnotics_header_BG[2], NULL, LV_ALIGN_IN_TOP_LEFT, 10, diag_widget_posY[2]);
  lv_obj_align(lb_diagnotics_header[2], NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0); // in Parent
    // lb_header_2 - Sensor
  lb_diagnotics_header_BG[3] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_header[3] = lv_label_create(lb_diagnotics_header_BG[3], NULL);
  lv_label_set_long_mode(lb_diagnotics_header[3], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_header[3], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_header[3], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_header[3], "Temperature");
  lv_obj_set_width(lb_diagnotics_header_BG[3], 150);
  lv_obj_set_height(lb_diagnotics_header_BG[3], hight_header1);
  lv_obj_set_width(lb_diagnotics_header[3], 150);
  lv_obj_add_style(lb_diagnotics_header[3], LV_LABEL_PART_MAIN , &style_diagnotics_header);
  lv_obj_set_style_local_bg_color(lb_diagnotics_header_BG[3], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_obj_align(lb_diagnotics_header_BG[3], NULL, LV_ALIGN_IN_TOP_LEFT, 10, diag_widget_posY[3]);
  lv_obj_align(lb_diagnotics_header[3], NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0); // in Parent
  
  lb_diagnotics_header_BG[4] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_header[4] = lv_label_create(lb_diagnotics_header_BG[4], NULL);
  lv_label_set_long_mode(lb_diagnotics_header[4], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_header[4], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_header[4], LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_header[4], "Lux Level");
  lv_obj_set_width(lb_diagnotics_header_BG[4], 150);
  lv_obj_set_height(lb_diagnotics_header_BG[4], hight_header1);
  lv_obj_set_width(lb_diagnotics_header[4], 150);
  lv_obj_add_style(lb_diagnotics_header[4], LV_LABEL_PART_MAIN , &style_diagnotics_header);
  lv_obj_set_style_local_bg_color(lb_diagnotics_header_BG[4], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_obj_align(lb_diagnotics_header_BG[4], NULL, LV_ALIGN_IN_TOP_LEFT, 10, diag_widget_posY[4]);
  lv_obj_align(lb_diagnotics_header[4], NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0); // in Parent
}

void ui_diagnotics_sensor(void)
{
  /******************************* SENSOR ******************************/
  // Create style Label
  static lv_style_t style_diagnotics_sensor;
  lv_style_init(&style_diagnotics_sensor);
  lv_style_set_text_font(&style_diagnotics_sensor, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_diagnotics_sensor, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_style_set_text_color(&style_diagnotics_sensor, LV_STATE_FOCUSED, LV_COLOR_WHITE);
  
  static lv_style_t style_diagnotics_sensor_BG;
  lv_style_init(&style_diagnotics_sensor_BG);
  lv_style_set_bg_color(&style_diagnotics_sensor_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_bg_color(&style_diagnotics_sensor_BG, LV_STATE_FOCUSED, LV_COLOR_GRAY);
  
  // Sensor 1
  lb_diagnotics_sensor_BG[0] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_sensor[0] = lv_label_create(lb_diagnotics_sensor_BG[0], NULL);
  lv_label_set_long_mode(lb_diagnotics_sensor[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_sensor[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_sensor[0], LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_sensor[0], "0.96");
  lv_obj_set_width(lb_diagnotics_sensor_BG[0], 96);
	lv_obj_set_height(lb_diagnotics_sensor_BG[0], 33);
  lv_obj_set_width(lb_diagnotics_sensor[0], 96);
  lv_obj_add_style(lb_diagnotics_sensor_BG[0], LV_LABEL_PART_MAIN , &style_diagnotics_sensor_BG);
  lv_obj_add_style(lb_diagnotics_sensor[0], LV_LABEL_PART_MAIN , &style_diagnotics_sensor);
  lv_obj_align(lb_diagnotics_sensor_BG[0], NULL, LV_ALIGN_IN_TOP_LEFT, 13, diag_widget_posY[1] + hight_header1);
  lv_obj_align(lb_diagnotics_sensor[0], NULL, LV_ALIGN_IN_TOP_LEFT, 18, 1); // in Parent

  // Sensor 2,3
  for(uint8_t iSS=1; iSS<3; iSS++)
  {
    lb_diagnotics_sensor_BG[iSS] = lv_obj_create(screen_diagnotics, lb_diagnotics_sensor_BG[0]);
    lb_diagnotics_sensor[iSS] = lv_label_create(lb_diagnotics_sensor_BG[iSS], lb_diagnotics_sensor[0]);
    lv_label_set_text(lb_diagnotics_sensor[iSS], "0.94");
    lv_obj_align(lb_diagnotics_sensor_BG[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 13 + 98 * iSS, diag_widget_posY[1] + hight_header1);
  }
  
  // Sensor 1
  lb_diagnotics_sensor_BG[3] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_sensor[3] = lv_label_create(lb_diagnotics_sensor_BG[3], NULL);
  lv_label_set_long_mode(lb_diagnotics_sensor[3], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_sensor[3], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_sensor[3], LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_sensor[3], "0.96b");
  lv_obj_set_width(lb_diagnotics_sensor_BG[3], 120);
	lv_obj_set_height(lb_diagnotics_sensor_BG[3], hight_header1);
  lv_obj_set_width(lb_diagnotics_sensor[3], 120);
  lv_obj_add_style(lb_diagnotics_sensor_BG[3], LV_LABEL_PART_MAIN , &style_diagnotics_sensor_BG);
  lv_obj_add_style(lb_diagnotics_sensor[3], LV_LABEL_PART_MAIN , &style_diagnotics_sensor);
  lv_obj_align(lb_diagnotics_sensor_BG[3], NULL, LV_ALIGN_IN_TOP_LEFT, 190, diag_widget_posY[2]);
  lv_obj_align(lb_diagnotics_sensor[3], NULL, LV_ALIGN_IN_TOP_LEFT, 18, 1); // in Parent
  
  // Sensor 1
  lb_diagnotics_sensor_BG[4] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_sensor[4] = lv_label_create(lb_diagnotics_sensor_BG[4], NULL);
  lv_label_set_long_mode(lb_diagnotics_sensor[4], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_sensor[4], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_sensor[4], LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_sensor[4], "0.96t");
  lv_obj_set_width(lb_diagnotics_sensor_BG[4], 120);
  lv_obj_set_height(lb_diagnotics_sensor_BG[4], hight_header1);
  lv_obj_set_width(lb_diagnotics_sensor[4], 120);
  lv_obj_add_style(lb_diagnotics_sensor_BG[4], LV_LABEL_PART_MAIN , &style_diagnotics_sensor_BG);
  lv_obj_add_style(lb_diagnotics_sensor[4], LV_LABEL_PART_MAIN , &style_diagnotics_sensor);
  lv_obj_align(lb_diagnotics_sensor_BG[4], NULL, LV_ALIGN_IN_TOP_LEFT, 190, diag_widget_posY[3]);
  lv_obj_align(lb_diagnotics_sensor[4], NULL, LV_ALIGN_IN_TOP_LEFT, 18, 1); // in Parent
  
  lb_diagnotics_sensor_BG[5] = lv_obj_create(screen_diagnotics, NULL);
  lb_diagnotics_sensor[5] = lv_label_create(lb_diagnotics_sensor_BG[5], NULL);
  lv_label_set_long_mode(lb_diagnotics_sensor[5], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_diagnotics_sensor[5], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_diagnotics_sensor[5], LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
  lv_label_set_text(lb_diagnotics_sensor[5], "0.96t");
  lv_obj_set_width(lb_diagnotics_sensor_BG[5], 120);
  lv_obj_set_height(lb_diagnotics_sensor_BG[5], hight_header1);
  lv_obj_set_width(lb_diagnotics_sensor[5], 120);
  lv_obj_add_style(lb_diagnotics_sensor_BG[5], LV_LABEL_PART_MAIN , &style_diagnotics_sensor_BG);
  lv_obj_add_style(lb_diagnotics_sensor[5], LV_LABEL_PART_MAIN , &style_diagnotics_sensor);
  lv_obj_align(lb_diagnotics_sensor_BG[5], NULL, LV_ALIGN_IN_TOP_LEFT, 190, diag_widget_posY[4]);
  lv_obj_align(lb_diagnotics_sensor[5], NULL, LV_ALIGN_IN_TOP_LEFT, 18, 1); // in Parent
}

void ui_diagnotics_time(void)
{

}

void ui_diagnotics_config(void)
{

}

void ui_diagnotics_system(void)
{

}

void ui_diagnotics_button(void)
{
  lv_obj_t *main_button[2];
  
  /*Create style*/
  static lv_style_t style_diagnotics_button;
  lv_style_init(&style_diagnotics_button);
  lv_style_set_bg_color(&style_diagnotics_button, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x40, 0x00));
  lv_style_set_text_color(&style_diagnotics_button, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  
  // button Static - Bright
  main_button[1] = lv_btn_create(screen_diagnotics, NULL);
  lv_obj_set_size(main_button[1], 80, 30);
  lv_obj_add_style(main_button[1], LV_BTN_PART_MAIN, &style_diagnotics_button);
  lv_obj_set_pos(main_button[1], 230, diag_widget_posY[5]);
  lb_diagnotics_button[1] = lv_label_create(main_button[1], NULL);
  lv_obj_set_style_local_text_color(lb_diagnotics_button[1], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
  lv_label_set_text(lb_diagnotics_button[1], "EXIT");
}

void UI_diagnotics_update(void)
{
  if(ui_diagnotics.update.Bit.sensor_data)
  {
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      memset(array_value_diagnotics,0,6);
      ui_convert_to_diagnotics_sensor(array_value_diagnotics, ui_diagnotics.sensor[iSS].data);
      lv_label_set_text(lb_diagnotics_sensor[iSS], (char*)&array_value_diagnotics);
    }
    if(ui_diagnotics.temp_type)
    {
       lv_label_set_text_fmt(lb_diagnotics_sensor[3],"%d mBar",ui_diagnotics.current_pressure);
       lv_label_set_text_fmt(lb_diagnotics_sensor[4],"%d \xC2\xB0\x43",ui_diagnotics.current_temp);
       
    }
    else
    {
      lv_label_set_text_fmt(lb_diagnotics_sensor[3],"%d mBar",ui_diagnotics.current_pressure);
      lv_label_set_text_fmt(lb_diagnotics_sensor[4],"%d \xC2\xB0\x46",ui_diagnotics.current_temp);
    }
     lv_label_set_text_fmt(lb_diagnotics_sensor[5],"%d",ui_diagnotics.lux_data);
    
  }
  // reset bit
  {
  ui_diagnotics.update.Word = 0;
  }
}

void UI_diagnotics_loadscreen(void)
{
  lv_scr_load(screen_diagnotics);
  screen_active_flag.Bit.diagnotic = 1;
  screen_active_flag.Bit.main = 0;
  screen_active_flag.Bit.menu = 0;
  screen_active_flag.Bit.dialog = 0;
  screen_active_flag.Bit.compass = 0;
}

void UI_diagnotics_init(void)
{
  screen_diagnotics = lv_obj_create(NULL, NULL);
  ui_diagnotics_header();
  ui_diagnotics_sensor();
  ui_diagnotics_time();
  ui_diagnotics_config();
  ui_diagnotics_system();
  ui_diagnotics_button();
  
  //memcpy(ui_diagnotics.button_L, "Funtion", 7);
  memcpy(ui_diagnotics.button_R, "EXIT", 5);
  //
  ui_diagnotics.update.Word = 0xffffffff;
  UI_diagnotics_update();
}

