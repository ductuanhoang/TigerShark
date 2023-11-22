#include "ui_compass.h"

const uint16_t compass_widget_posY[6] = {5, 45, 111, 130, 160, 205};
const uint16_t hight_header2 = 21;

lv_obj_t *screen_compass;

lv_obj_t *lb_compass_header_BG[8], *lb_compass_header[8];
// lv_obj_t *lb_compass_time[3];
lv_obj_t *lb_compass_sensor_BG[3], *lb_compass_sensor[3];
// lv_obj_t *lb_compass_config[4];
// lv_obj_t *lb_compass_system[4];
lv_obj_t *lb_compass_button[2];
// lv_obj_t *img_compass_up[3], *img_compass_down[3];

ui_compass_t ui_compass;

uint8_t array_value_compass[6];
uint8_t times_blink_compass = 0;
extern uint16_t north_magnetic_degree;

void ui_compass_header(void)
{
  /****************************** HEADER ******************************/
  // Create style
  static lv_style_t style_compass_header;
  lv_style_init(&style_compass_header);
  lv_style_set_text_font(&style_compass_header, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_compass_header, lv_color_white());

  // lb header
  lb_compass_header_BG[1] = lv_obj_create(screen_compass);
  lb_compass_header[1] = lv_label_create(lb_compass_header_BG[1]);
  lv_label_set_long_mode(lb_compass_header[1], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[1], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[1], LV_TEXT_ALIGN_CENTER, 0); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[1], "COMPASS SCREEN");
  lv_obj_set_width(lb_compass_header_BG[1], 298);
  lv_obj_set_height(lb_compass_header_BG[1], hight_header2);
  lv_obj_set_width(lb_compass_header[1], 298);
  lv_obj_add_style(lb_compass_header[1], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[1], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[1], LV_ALIGN_TOP_LEFT, 10, compass_widget_posY[0]);

  lv_obj_align(lb_compass_header[1], LV_ALIGN_TOP_LEFT, 1, 1); // in Parent

  // lb_header_1 - Sensor
  lb_compass_header_BG[0] = lv_obj_create(screen_compass);
  lb_compass_header[0] = lv_label_create(lb_compass_header_BG[0]);
  lv_label_set_long_mode(lb_compass_header[0], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[0], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[0], LV_TEXT_ALIGN_CENTER, 0); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[0], "Sen 1 Mv    Sen 2 Mv    Sen 3 Mv");
  lv_obj_set_width(lb_compass_header_BG[0], 298);
  lv_obj_set_height(lb_compass_header_BG[0], hight_header2);
  lv_obj_set_width(lb_compass_header[0], 298);
  lv_obj_add_style(lb_compass_header[0], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[0], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[0], LV_ALIGN_TOP_LEFT, 10, compass_widget_posY[1]);
  lv_obj_align(lb_compass_header[0], LV_ALIGN_TOP_LEFT, 0, 0); // in Parent

  // lb compass
  lb_compass_header_BG[2] = lv_obj_create(screen_compass);
  lb_compass_header[2] = lv_label_create(lb_compass_header_BG[2]);
  lv_label_set_long_mode(lb_compass_header[2], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[2], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[2], LV_TEXT_ALIGN_CENTER, 0); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[2], "Current");
  lv_obj_set_width(lb_compass_header_BG[2], 100);
  lv_obj_set_height(lb_compass_header_BG[2], hight_header2);
  lv_obj_set_width(lb_compass_header[2], 100);
  lv_obj_add_style(lb_compass_header[2], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[2], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[2], LV_ALIGN_TOP_LEFT, 10, compass_widget_posY[3]);
  lv_obj_align(lb_compass_header[2], LV_ALIGN_TOP_LEFT, 1, 1); // in Parent

  lb_compass_header_BG[4] = lv_obj_create(screen_compass);
  lb_compass_header[4] = lv_label_create(lb_compass_header_BG[4]);
  lv_label_set_long_mode(lb_compass_header[4], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[4], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[4], LV_TEXT_ALIGN_CENTER, 0); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[4], "   ");
  lv_obj_set_width(lb_compass_header_BG[4], 70);
  lv_obj_set_height(lb_compass_header_BG[4], hight_header2);
  lv_obj_set_width(lb_compass_header[4], 70);
  lv_obj_add_style(lb_compass_header[4], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[4], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[4], LV_ALIGN_TOP_LEFT, 120, compass_widget_posY[3]);
  lv_obj_align(lb_compass_header[4], LV_ALIGN_TOP_LEFT, 1, 1); // in Parent

  lb_compass_header_BG[6] = lv_obj_create(screen_compass);
  lb_compass_header[6] = lv_label_create(lb_compass_header_BG[6]);
  lv_label_set_long_mode(lb_compass_header[6], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[6], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[6], LV_TEXT_ALIGN_CENTER, 0); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[6], "   ");
  lv_obj_set_width(lb_compass_header_BG[6], 70);
  lv_obj_set_height(lb_compass_header_BG[6], hight_header2);
  lv_obj_set_width(lb_compass_header[6], 70);
  lv_obj_add_style(lb_compass_header[6], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[6], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[6], LV_ALIGN_TOP_LEFT, 200, compass_widget_posY[3]);
  lv_obj_align(lb_compass_header[6], LV_ALIGN_TOP_LEFT, 1, 1); // in Parent

  lb_compass_header_BG[3] = lv_obj_create(screen_compass);
  lb_compass_header[3] = lv_label_create(lb_compass_header_BG[3]);
  lv_label_set_long_mode(lb_compass_header[3], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[3], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[3], LV_TEXT_ALIGN_CENTER, 0); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[3], "SetPoint");
  lv_obj_set_width(lb_compass_header_BG[3], 100);
  lv_obj_set_height(lb_compass_header_BG[3], hight_header2);
  lv_obj_set_width(lb_compass_header[3], 100);
  lv_obj_add_style(lb_compass_header[3], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[3], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[3], LV_ALIGN_TOP_LEFT, 10, compass_widget_posY[4]);
  lv_obj_align(lb_compass_header[3], LV_ALIGN_TOP_LEFT, 1, 1); // in Parent

  lb_compass_header_BG[5] = lv_obj_create(screen_compass);
  lb_compass_header[5] = lv_label_create(lb_compass_header_BG[5]);
  lv_label_set_long_mode(lb_compass_header[5], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[5], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[5], LV_TEXT_ALIGN_CENTER, 0); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[5], "   ");
  lv_obj_set_width(lb_compass_header_BG[5], 70);
  lv_obj_set_height(lb_compass_header_BG[5], hight_header2);
  lv_obj_set_width(lb_compass_header[5], 70);
  lv_obj_add_style(lb_compass_header[5], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[5], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[5], LV_ALIGN_TOP_LEFT, 120, compass_widget_posY[4]);
  lv_obj_align(lb_compass_header[5], LV_ALIGN_TOP_LEFT, 1, 1); // in Parent

  lb_compass_header_BG[7] = lv_obj_create(screen_compass);
  lb_compass_header[7] = lv_label_create(lb_compass_header_BG[7]);
  lv_label_set_long_mode(lb_compass_header[7], LV_LABEL_LONG_WRAP);           /*Break the long lines*/
  lv_label_set_recolor(lb_compass_header[7], true);                           /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_header[7], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT); /*Center aligned lines*/
  lv_label_set_text(lb_compass_header[7], "   ");
  lv_obj_set_width(lb_compass_header_BG[7], 70);
  lv_obj_set_height(lb_compass_header_BG[7], hight_header2);
  lv_obj_set_width(lb_compass_header[7], 70);
  lv_obj_add_style(lb_compass_header[7], &style_compass_header, LV_PART_MAIN);
  lv_obj_set_style_bg_color(lb_compass_header_BG[7], lv_color_hex(0x000080), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_header_BG[7], LV_ALIGN_TOP_LEFT, 200, compass_widget_posY[4]);
  lv_obj_align(lb_compass_header[7], LV_ALIGN_TOP_LEFT, 1, 1); // in Parent
}

void ui_compass_sensor(void)
{
  /******************************* SENSOR ******************************/
  // Create style Label
  static lv_style_t style_compass_sensor;
  lv_style_init(&style_compass_sensor);
  lv_style_set_text_font(&style_compass_sensor, &lv_font_montserrat_18);
  // lv_style_set_text_color(&style_compass_sensor, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_style_set_text_color(&style_compass_sensor, lv_color_hex(0x000080));

  static lv_style_t style_compass_sensor_BG;
  lv_style_init(&style_compass_sensor_BG);
  lv_style_set_bg_color(&style_compass_sensor_BG, lv_color_hex(0x000000));
  lv_style_set_bg_color(&style_compass_sensor_BG, lv_color_hex(0x828282));

  // Sensor 1
  lb_compass_sensor_BG[0] = lv_obj_create(screen_compass);
  lb_compass_sensor[0] = lv_label_create(lb_compass_sensor_BG[0]);
  lv_label_set_long_mode(lb_compass_sensor[0], LV_LABEL_LONG_WRAP);       /*Break the long lines*/
  lv_label_set_recolor(lb_compass_sensor[0], true);                       /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_compass_sensor[0], LV_TEXT_ALIGN_LEFT, LV_PART_MAIN| LV_STATE_DEFAULT); /*Center aligned lines*/
  lv_label_set_text(lb_compass_sensor[0], "0.96");
  lv_obj_set_width(lb_compass_sensor_BG[0], 96);
  lv_obj_set_height(lb_compass_sensor_BG[0], 33);
  lv_obj_set_width(lb_compass_sensor[0], 96);
  lv_obj_add_style(lb_compass_sensor_BG[0], &style_compass_sensor_BG, LV_STATE_DEFAULT);
  lv_obj_add_style(lb_compass_sensor[0], &style_compass_sensor, LV_STATE_DEFAULT);
  lv_obj_align(lb_compass_sensor_BG[0], LV_ALIGN_TOP_LEFT, 13, compass_widget_posY[1] + hight_header2);
  lv_obj_align(lb_compass_sensor[0], LV_ALIGN_TOP_LEFT, 18, 1); // in Parent

  // Sensor 2,3
  for (uint8_t iSS = 1; iSS < 3; iSS++)
  {
    // lb_compass_sensor_BG[iSS] = lv_obj_create(screen_compass, lb_compass_sensor_BG[0]);
    // lb_compass_sensor[iSS] = lv_label_create(lb_compass_sensor_BG[iSS], lb_compass_sensor[0]);
    // lv_label_set_text(lb_compass_sensor[iSS], "0.94");
    // lv_obj_align(lb_compass_sensor_BG[iSS], LV_ALIGN_TOP_LEFT, 13 + 98 * iSS, compass_widget_posY[1] + hight_header2);
  }
}

void ui_compass_time(void)
{
}

void ui_compass_config(void)
{
}

void ui_compass_system(void)
{
}

void ui_compass_button(void)
{
  lv_obj_t *main_button[2];

  /*Create style*/
  static lv_style_t style_compass_button;
  lv_style_init(&style_compass_button);
  lv_style_set_bg_color(&style_compass_button, lv_color_hex(0x004000));
  lv_style_set_text_color(&style_compass_button, lv_color_hex(0x000000));

  // button Static - Bright
  main_button[1] = lv_btn_create(screen_compass);
  lv_obj_set_size(main_button[1], 80, 30);
  lv_obj_add_style(main_button[1], &style_compass_button, LV_PART_MAIN);
  lv_obj_set_pos(main_button[1], 230, compass_widget_posY[5]);
  lb_compass_button[1] = lv_label_create(main_button[1]);
  lv_obj_set_style_text_color(lb_compass_button[1], lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_label_set_text(lb_compass_button[1], "Select");

  main_button[0] = lv_btn_create(screen_compass);
  lv_obj_set_size(main_button[0], 80, 30);
  lv_obj_add_style(main_button[0], &style_compass_button, LV_PART_MAIN);
  lv_obj_set_pos(main_button[0], 10, compass_widget_posY[5]);
  lb_compass_button[0] = lv_label_create(main_button[0]);
  lv_obj_set_style_text_color(lb_compass_button[0], lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_label_set_text(lb_compass_button[0], "Function");
}

void compass_convert(uint16_t degree, uint8_t id)
{
  if ((degree >= 338) || (degree <= 22))
  {
    lv_label_set_text(lb_compass_header[id], "N");
  }
  else if ((degree >= 23) && (degree <= 67))
  {
    lv_label_set_text(lb_compass_header[id], "NE");
  }
  else if ((degree >= 68) && (degree <= 112))
  {
    lv_label_set_text(lb_compass_header[id], "E");
  }
  else if ((degree >= 113) && (degree <= 157))
  {
    lv_label_set_text(lb_compass_header[id], "SE");
  }
  else if ((degree >= 158) && (degree <= 202))
  {
    lv_label_set_text(lb_compass_header[id], "S");
  }
  else if ((degree >= 203) && (degree <= 247))
  {
    lv_label_set_text(lb_compass_header[id], "SW");
  }
  else if ((degree >= 248) && (degree <= 292))
  {
    lv_label_set_text(lb_compass_header[id], "W");
  }
  else
  {
    lv_label_set_text(lb_compass_header[id], "NW");
  }
}

void UI_compass_update(void)
{
  if (ui_compass.update.Bit.sensor_data)
  {
    for (uint8_t iSS = 0; iSS < 3; iSS++)
    {
      memset(array_value_compass, 0, 6);
      ui_convert_to_sensor(array_value_compass, ui_compass.sensor[iSS].data);
      lv_label_set_text(lb_compass_sensor[iSS], (char *)&array_value_compass);
    }
    memset(array_value_compass, 0, 6);
    ui_convert_to_compass(array_value_compass, north_magnetic_degree);
    lv_label_set_text(lb_compass_header[6], (char *)&array_value_compass);
    compass_convert(north_magnetic_degree, 4);
    if (ui_compass.setpoint_flag)
    {
      memset(array_value_compass, 0, 6);
      ui_convert_to_compass(array_value_compass, ui_compass.compass_setpoint);
      lv_label_set_text(lb_compass_header[7], (char *)&array_value_compass);
      compass_convert(north_magnetic_degree, 5);
    }
    else
    {
      lv_label_set_text(lb_compass_header[7], "     ");
      lv_label_set_text(lb_compass_header[5], "     ");
    }
  }

  if (ui_compass.update.Bit.button_L)
  {
    lv_label_set_text(lb_compass_button[0], ui_compass.button_L);
  }
  if (ui_compass.update.Bit.button_R)
  {
    lv_label_set_text(lb_compass_button[1], ui_compass.button_R);
  }

  // reset bit
  {
    ui_compass.update.Word = 0;
  }
}

void UI_compass_loadscreen(void)
{
  lv_scr_load(screen_compass);
  screen_active_flag.Bit.diagnotic = 0;
  screen_active_flag.Bit.main = 0;
  screen_active_flag.Bit.menu = 0;
  screen_active_flag.Bit.dialog = 0;
  screen_active_flag.Bit.compass = 1;
}

void UI_compass_init(void)
{
  screen_compass = lv_obj_create(NULL);
  ui_compass_header();
  ui_compass_sensor();
  ui_compass_time();
  ui_compass_config();
  ui_compass_system();
  ui_compass_button();

  memcpy(ui_compass.button_L, "Function", 8);
  memcpy(ui_compass.button_R, "Select", 5);
  //
  ui_compass.update.Word = 0xffffffff;
  UI_compass_update();
}