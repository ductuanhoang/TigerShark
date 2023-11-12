#include "ui_menu.h"
#include "ui_img_storage.h"
#include "ui_control.h"

const uint16_t menu_widget_posY[4] = {10, 50, 70, 203};
char user_setpoint_list_context[7][20];
lv_obj_t *screen_menu;

lv_obj_t *lb_menu_sensor_BG[3], *lb_menu_sensor[3];
lv_obj_t *lb_menu_header;
lv_obj_t *lb_menu_listview_BG[5], *lb_menu_listview[5], *lb_menu_listview_value[5];
lv_obj_t *lb_menu_button[2];
lv_obj_t *img_menu_up[3], *img_menu_down[3];

ui_menu_t ui_menu;

uint8_t array_value_menu[6];
uint8_t times_blink_menu = 0;

void ui_menu_header(void)
{
  /****************************** TABLE ******************************/
  // Create style
  static lv_style_t style_menu_line;
  lv_style_init(&style_menu_line);
  lv_style_set_line_width(&style_menu_line, LV_STATE_DEFAULT, 2);
  lv_style_set_line_color(&style_menu_line, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_style_set_line_rounded(&style_menu_line, LV_STATE_DEFAULT, true);
  
  // Table
  static lv_point_t line_points_menu[] = { {0, 0}, {294, 0}, {294, 35}, {0, 35}, {0, 0},
                                      {196, 0}, {196, 35}, {98, 35}, {98, 0},  };
  lv_obj_t * line1;
  line1 = lv_line_create(screen_menu, NULL);
  lv_line_set_points(line1, line_points_menu, 9);     /*Set the points*/
  lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_menu_line);     /*Set the points*/
  lv_obj_align(line1, NULL, LV_ALIGN_IN_TOP_LEFT, 12, menu_widget_posY[0]);
  
  /********************** Header Menu Label **************************/
  // Create style
  static lv_style_t style_menu_header;
  lv_style_init(&style_menu_header);
  lv_style_set_text_font(&style_menu_header, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_menu_header, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x40, 0x00));
  
  // Label Menu header
  lb_menu_header = lv_label_create(screen_menu, NULL);
  lv_label_set_long_mode(lb_menu_header, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_menu_header, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_menu_header, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(lb_menu_header, "OPTION MENU");
  lv_obj_set_width(lb_menu_header, 298);
  lv_obj_add_style(lb_menu_header, LV_LABEL_PART_MAIN , &style_menu_header);
  lv_obj_align(lb_menu_header, NULL, LV_ALIGN_IN_TOP_LEFT, 10, menu_widget_posY[1]); // in Parent
}

void ui_menu_sensor(void)
{
  /******************************* SENSOR ******************************/
  // Create style Label
  static lv_style_t style_menu_sensor;
  lv_style_init(&style_menu_sensor);
  lv_style_set_text_font(&style_menu_sensor, LV_STATE_DEFAULT, &lv_font_montserrat_30);
  lv_style_set_text_color(&style_menu_sensor, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_style_set_text_color(&style_menu_sensor, LV_STATE_FOCUSED, LV_COLOR_WHITE);
  
  static lv_style_t style_menu_sensor_BG;
  lv_style_init(&style_menu_sensor_BG);
  lv_style_set_bg_color(&style_menu_sensor_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_bg_color(&style_menu_sensor_BG, LV_STATE_FOCUSED, LV_COLOR_GRAY);
  
  // Sensor 1
  lb_menu_sensor_BG[0] = lv_obj_create(screen_menu, NULL);
  lb_menu_sensor[0] = lv_label_create(lb_menu_sensor_BG[0], NULL);
  lv_label_set_long_mode(lb_menu_sensor[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_menu_sensor[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_menu_sensor[0], LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
  lv_label_set_text(lb_menu_sensor[0], "0.96");
  lv_obj_set_width(lb_menu_sensor_BG[0], 96);
	lv_obj_set_height(lb_menu_sensor_BG[0], 33);
  lv_obj_set_width(lb_menu_sensor[0], 96);
  lv_obj_add_style(lb_menu_sensor_BG[0], LV_LABEL_PART_MAIN , &style_menu_sensor_BG);
  lv_obj_add_style(lb_menu_sensor[0], LV_LABEL_PART_MAIN , &style_menu_sensor);
  lv_obj_align(lb_menu_sensor_BG[0], NULL, LV_ALIGN_IN_TOP_LEFT, 13, menu_widget_posY[0] + 1);
  lv_obj_align(lb_menu_sensor[0], NULL, LV_ALIGN_IN_TOP_LEFT, 15, 1); // in Parent

  // Sensor 2,3
  for(uint8_t iSS=1; iSS<3; iSS++)
  {
    lb_menu_sensor_BG[iSS] = lv_obj_create(screen_menu, lb_menu_sensor_BG[0]);
    lb_menu_sensor[iSS] = lv_label_create(lb_menu_sensor_BG[iSS], lb_menu_sensor[0]);
    lv_label_set_text(lb_menu_sensor[iSS], "0.94");
    lv_obj_align(lb_menu_sensor_BG[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 13 + 98 * iSS, menu_widget_posY[0] + 1);
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
    img_menu_up[iSS] = lv_img_create(lb_menu_sensor_BG[iSS], NULL);
    lv_img_set_src(img_menu_up[iSS], &img_arrow_up);
    lv_obj_align(img_menu_up[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 78, 7);
    // Down
    img_menu_down[iSS] = lv_img_create(lb_menu_sensor_BG[iSS], NULL);
    lv_img_set_src(img_menu_down[iSS], &img_arrow_down);
    lv_obj_align(img_menu_down[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 78, 7);
  }
}

void ui_menu_listview(void)
{
  /*Create style*/
  static lv_style_t style_menu_list;
  lv_style_init(&style_menu_list);
  lv_style_set_text_font(&style_menu_list, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_menu_list, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_style_set_text_color(&style_menu_list, LV_STATE_FOCUSED, LV_COLOR_WHITE);
  
  static lv_style_t style_menu_list_BG;
  lv_style_init(&style_menu_list_BG);
  lv_style_set_bg_color(&style_menu_list_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_bg_color(&style_menu_list_BG, LV_STATE_FOCUSED, LV_COLOR_NAVY);
  
  // listview 1
  lb_menu_listview_BG[0] = lv_obj_create(screen_menu, NULL);
  lb_menu_listview[0] = lv_label_create(lb_menu_listview_BG[0], NULL);
  lv_label_set_long_mode(lb_menu_listview[0], LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  lv_label_set_recolor(lb_menu_listview[0], true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(lb_menu_listview[0], LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
  lv_label_set_text(lb_menu_listview[0], "Menu 1");
  lv_obj_set_width(lb_menu_listview_BG[0], 298);
  lv_obj_set_height(lb_menu_listview_BG[0], 25);
  lv_obj_set_width(lb_menu_listview[0], 298);
  lv_obj_add_style(lb_menu_listview_BG[0], LV_LABEL_PART_MAIN , &style_menu_list_BG);
  lv_obj_add_style(lb_menu_listview[0], LV_LABEL_PART_MAIN , &style_menu_list);
  lv_obj_align(lb_menu_listview_BG[0], NULL, LV_ALIGN_IN_TOP_LEFT, 10, menu_widget_posY[2]);
  lv_obj_align(lb_menu_listview[0], NULL, LV_ALIGN_IN_TOP_LEFT, 10, 2); // in Parent
  
  // listview 2,3,4,5
  for (uint8_t iLv=1; iLv<5; iLv++)
  {
    lb_menu_listview_BG[iLv] = lv_obj_create(screen_menu, lb_menu_listview_BG[0]); // copy from 0
    lb_menu_listview[iLv] = lv_label_create(lb_menu_listview_BG[iLv], lb_menu_listview[0]);       // copy from 0
    lv_label_set_text(lb_menu_listview[iLv], "Menu x");                     // set Text
    lv_obj_align(lb_menu_listview_BG[iLv], NULL, LV_ALIGN_IN_TOP_LEFT, 10, menu_widget_posY[2] + 25 * iLv);
    lv_obj_align(lb_menu_listview[iLv], NULL, LV_ALIGN_IN_TOP_LEFT, 10, 2); // in Parent
  }
  
  // listview value 1,2,3,4,5
  for (uint8_t iLv=0; iLv<5; iLv++)
  {
    lb_menu_listview_value[iLv] = lv_label_create(lb_menu_listview_BG[iLv], lb_menu_listview[iLv]);       // copy from 0
    lv_label_set_text(lb_menu_listview_value[iLv], "value x");                     // set Text
    lv_obj_align(lb_menu_listview_value[iLv], NULL, LV_ALIGN_IN_TOP_LEFT, 200, 2); // in Parent
  }
}

void ui_menu_button(void)
{
  lv_obj_t *menu_button[2];
  
  /*Create style*/
  static lv_style_t style_button;
  lv_style_init(&style_button);
  lv_style_set_bg_color(&style_button, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x40, 0x00));
  lv_style_set_text_color(&style_button, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  
  // button 1
  menu_button[0] = lv_btn_create(screen_menu, NULL);
  lv_obj_set_size(menu_button[0], 120, 30);
  lv_obj_add_style(menu_button[0], LV_BTN_PART_MAIN, &style_button);
  lv_obj_set_pos(menu_button[0], 10, menu_widget_posY[3]);
  lb_menu_button[0] = lv_label_create(menu_button[0], NULL);
  lv_obj_set_style_local_text_color(lb_menu_button[0], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text(lb_menu_button[0], "Next");
  
  // button 2
  menu_button[1] = lv_btn_create(screen_menu, NULL);
  lv_obj_set_size(menu_button[1], 120, 30);
  lv_obj_add_style(menu_button[1], LV_BTN_PART_MAIN, &style_button);
  lv_obj_set_pos(menu_button[1], 190, menu_widget_posY[3]);
  lb_menu_button[1] = lv_label_create(menu_button[1], NULL);
  lv_obj_set_style_local_text_color(lb_menu_button[1], LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
  lv_label_set_text(lb_menu_button[1], "Select");
}

void ui_menu_set_focus_listview(uint8_t index_lv)
{
  // set Text to Listview
  if(index_lv%5 == 0)
  {
    for (uint8_t iLv=0; iLv<5; iLv++)
    {
      if((index_lv + iLv) < ui_menu.sum_list_menu)
      {
        lv_label_set_text(lb_menu_listview[iLv], ui_menu.list_menu[index_lv + iLv]._char);
        lv_label_set_text(lb_menu_listview_value[iLv], ui_menu.list_menu_value[index_lv + iLv]._char);
      }
      else
      {
        lv_label_set_text(lb_menu_listview[iLv], " ");
        lv_label_set_text(lb_menu_listview_value[iLv], " ");
      }
    }
  }
  else
  {
     lv_label_set_text(lb_menu_listview[index_lv%5], ui_menu.list_menu[index_lv]._char);
     lv_label_set_text(lb_menu_listview_value[index_lv%5], ui_menu.list_menu_value[index_lv]._char);
  }
  // set focus listview
  for (uint8_t iLv=0; iLv<5; iLv++)
  {
    lv_obj_set_state(lb_menu_listview[iLv], (index_lv%5 == iLv) ? LV_STATE_FOCUSED : LV_STATE_DEFAULT);
    lv_obj_set_state(lb_menu_listview_value[iLv], (index_lv%5 == iLv) ? LV_STATE_FOCUSED : LV_STATE_DEFAULT);
    lv_obj_set_state(lb_menu_listview_BG[iLv], (index_lv%5 == iLv) ? LV_STATE_FOCUSED : LV_STATE_DEFAULT);
  }
}
void ui_menu_update_value_listview(uint8_t index_lv)
{
  // set Text to Listview
     lv_label_set_text(lb_menu_listview[index_lv%5 -1], ui_menu.list_menu[index_lv -1]._char);
     lv_label_set_text(lb_menu_listview_value[index_lv%5 -1], ui_menu.list_menu_value[index_lv-1]._char);
     lv_label_set_text(lb_menu_listview[index_lv%5], ui_menu.list_menu[index_lv]._char);
     lv_label_set_text(lb_menu_listview_value[index_lv%5], ui_menu.list_menu_value[index_lv]._char);
  // set focus listview
}
void UI_menu_update(void)
{
  if(ui_menu.update.Bit.sensor_value)
  {
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      ui_convert_to_sensor(array_value_menu, ui_menu.sensor[iSS].data);
      lv_label_set_text(lb_menu_sensor[iSS], (char*)&array_value_menu);
    }
  }
  if(ui_menu.update.Bit.sensor_blink)
  {
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      if(ui_menu.sensor[iSS].blink == 0)
      {
        lv_obj_set_state(lb_menu_sensor_BG[iSS], LV_STATE_DEFAULT);
        lv_obj_set_state(lb_menu_sensor[iSS], LV_STATE_DEFAULT);
      }
    }
  }
  if(ui_menu.update.Bit.sensor_arrow)
  {
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      if(ui_menu.sensor[iSS].arrow == ARROW_UP)
      {
        lv_obj_set_hidden(img_menu_up[iSS], false);
        lv_obj_set_hidden(img_menu_down[iSS], true);
        lv_obj_align(lb_menu_sensor[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 10, 1);
      }
      else if(ui_menu.sensor[iSS].arrow == ARROW_DOWN)
      {
        lv_obj_set_hidden(img_menu_up[iSS], true);
        lv_obj_set_hidden(img_menu_down[iSS], false);
        lv_obj_align(lb_menu_sensor[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 10, 1);
      }
      else
      {
        lv_obj_set_hidden(img_menu_up[iSS], true);
        lv_obj_set_hidden(img_menu_down[iSS], true);
        lv_obj_align(lb_menu_sensor[iSS], NULL, LV_ALIGN_IN_TOP_LEFT, 15, 1);
      }  
    }
  }
  if(ui_menu.update.Bit.header_menu)
  {
    lv_label_set_text(lb_menu_header, ui_menu.header_menu._char);
  }
  if(ui_menu.update.Bit.index_listview)
  {
    ui_menu_set_focus_listview(ui_menu.index_listview);
  }
  if(ui_menu.update.Bit.index_listview_value)
  {
    ui_menu_update_value_listview(ui_menu.index_listview);
  }
  if(ui_menu.update.Bit.button_L)
  {
    lv_label_set_text(lb_menu_button[0], ui_menu.button_L);
  }
  if(ui_menu.update.Bit.button_R)
  {
    lv_label_set_text(lb_menu_button[1], ui_menu.button_R);
  }
  // reset bit
  {
    ui_menu.update.Word = 0;
  }
}

void UI_menu_timer_blink(void)
{
  if(screen_active_flag.Bit.menu)
  {
    //
    for(uint8_t iSS=0; iSS<3; iSS++)
    {
      if(ui_menu.sensor[iSS].blink)
      {
        if(times_blink_menu == 0)
        {
          lv_obj_set_state(lb_menu_sensor_BG[iSS], LV_STATE_FOCUSED);
          lv_obj_set_state(lb_menu_sensor[iSS], LV_STATE_FOCUSED);
        }
        else
        {
          lv_obj_set_state(lb_menu_sensor_BG[iSS], LV_STATE_DEFAULT);
          lv_obj_set_state(lb_menu_sensor[iSS], LV_STATE_DEFAULT);
        }
      }
    }
    //
    if(times_blink_menu == 0)
      times_blink_menu = 1;
    else
      times_blink_menu = 0;
  }
}

void UI_menu_loadscreen(void)
{
  lv_scr_load(screen_menu);
  screen_active_flag.Bit.main = 0;
  screen_active_flag.Bit.menu = 1;
  screen_active_flag.Bit.dialog = 0;
  screen_active_flag.Bit.diagnotic =0;
  screen_active_flag.Bit.compass = 0;
}

void UI_menu_init(void)
{
  screen_menu = lv_obj_create(NULL, NULL);
  ui_menu_header();
  ui_menu_sensor();
  ui_menu_listview();
  ui_menu_button();
  
  // TEST
  ui_menu.sensor[0].data = 123;
  ui_menu.sensor[1].data = 456;
  ui_menu.sensor[2].data = 789;
  ui_menu.sensor[0].blink = 1;
  ui_menu.sensor[2].blink = 1;
  ui_menu.sensor[0].arrow = ARROW_UP;
  ui_menu.sensor[1].arrow = ARROW_NONE;
  ui_menu.sensor[2].arrow = ARROW_DOWN;
  //
  ui_menu.sum_list_menu = 15;
 // memcpy(ui_menu.list_menu, menu_option_context, 15*25);
  ui_menu.index_listview = 0;
  memcpy(ui_menu.header_menu._char, "OPT MENU SETTING", 16);
  memcpy(ui_menu.button_L, "Next", 4);
  memcpy(ui_menu.button_R, "Select", 6);
  //
  ui_menu.update.Word = 0xff;
  UI_menu_update(); 
}

