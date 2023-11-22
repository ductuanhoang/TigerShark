#include "ui_dialog.h"

const uint16_t dialog_widget_posY[] = {15, 50, 140, 168, 203};
const uint16_t dialog_widget_border_Y = 10;

lv_obj_t *screen_dialog;

lv_obj_t *lb_dialog_header;
lv_obj_t *lb_dialog_context;
lv_obj_t *lb_text_input_BG, *lb_text_input;
lv_obj_t *lb_preview_text, *line1_dialog_text;
lv_obj_t *lb_dialog_button[2];

ui_dialog_t ui_dialog;

uint8_t dialog_array_string[12];
uint8_t times_blink_dialog = 0;
char temp_char_blink;

void ui_dialog_border(void)
{
  lv_obj_t *menu_bt_border;

  // Create style
  static lv_style_t style_bt_border;
  lv_style_init(&style_bt_border);
  lv_style_set_line_width(&style_bt_border, 2);
  lv_style_set_radius(&style_bt_border, 20);

  // button border
  menu_bt_border = lv_btn_create(screen_dialog);
  lv_obj_set_size(menu_bt_border, 300, 183);
  lv_obj_add_style(menu_bt_border, &style_bt_border, LV_PART_MAIN);
  lv_obj_set_pos(menu_bt_border, 10, dialog_widget_border_Y);
}

void ui_dialog_header_context(void)
{
  /********************** Header Menu Label **************************/
  // Create style
  static lv_style_t style_dialog_text;
  lv_style_init(&style_dialog_text);
  lv_style_set_text_font(&style_dialog_text, &lv_font_montserrat_18);

  // Label dialog header
  lb_dialog_header = lv_label_create(screen_dialog);
  lv_label_set_long_mode(lb_dialog_header, LV_LABEL_LONG_WRAP);                                         /*Break the long lines*/
  lv_label_set_recolor(lb_dialog_header, true);                                                         /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_dialog_header, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT); /*Center aligned lines*/
  lv_label_set_text(lb_dialog_header, "DIALOG HEADER");
  lv_obj_set_width(lb_dialog_header, 298);
  lv_obj_add_style(lb_dialog_header, &style_dialog_text, LV_PART_MAIN);
  lv_obj_set_style_text_color(lb_dialog_header, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_dialog_header, LV_ALIGN_TOP_LEFT, 10, dialog_widget_posY[0]);

  // Label dialog context
  lb_dialog_context = lv_label_create(screen_dialog);
  lv_label_set_long_mode(lb_dialog_context, LV_LABEL_LONG_WRAP);                                       /*Break the long lines*/
  lv_label_set_recolor(lb_dialog_context, true);                                                       /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_dialog_context, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT); /*Center aligned lines*/
  lv_label_set_text(lb_dialog_context, "Dialog context..");
  lv_obj_set_width(lb_dialog_context, 278);
  lv_obj_add_style(lb_dialog_context, &style_dialog_text, LV_PART_MAIN);
  lv_obj_set_style_text_color(lb_dialog_context, lv_color_hex(0x004000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lb_dialog_context, LV_ALIGN_TOP_LEFT, 20, dialog_widget_posY[1]);
}

void ui_dialog_text_input(void)
{
  // Create style Label
  static lv_style_t style_dialog_text_input;
  lv_style_init(&style_dialog_text_input);
  lv_style_set_text_font(&style_dialog_text_input, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_dialog_text_input, lv_color_hex(0xFF0000));

  static lv_style_t style_dialog_text_input_BG;
  lv_style_init(&style_dialog_text_input_BG);
  lv_style_set_bg_color(&style_dialog_text_input_BG, lv_color_hex(0xD2D2D2));

  // Text input
  lb_text_input_BG = lv_obj_create(screen_dialog);
  lb_text_input = lv_label_create(lb_text_input_BG);
  lv_label_set_long_mode(lb_text_input, LV_LABEL_LONG_WRAP);                                       /*Break the long lines*/
  lv_label_set_recolor(lb_text_input, true);                                                       /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_text_input, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT); /*Center aligned lines*/
  lv_label_set_text(lb_text_input, "12_");
  lv_obj_set_width(lb_text_input_BG, 150);
  lv_obj_set_height(lb_text_input_BG, 25);
  lv_obj_set_width(lb_text_input, 150);
  lv_obj_add_style(lb_text_input_BG, &style_dialog_text_input_BG, LV_PART_MAIN);
  lv_obj_add_style(lb_text_input, &style_dialog_text_input, LV_PART_MAIN);
  lv_obj_align(lb_text_input_BG, LV_ALIGN_TOP_LEFT, 110, dialog_widget_posY[2]);
  lv_obj_align(lb_text_input, LV_ALIGN_TOP_LEFT, 5, 2); // in Parent
}

void ui_dialog_preview_text(void)
{
  /****************************** Preview Text ******************************/
  // Create style Label
  static lv_style_t style_dialog_preview_text;
  lv_style_init(&style_dialog_preview_text);
  lv_style_set_text_font(&style_dialog_preview_text, &lv_font_montserrat_18);
  lv_style_set_text_color(&style_dialog_preview_text, lv_color_hex(0xA0A0A0));

  // Preview Text
  lb_preview_text = lv_label_create(screen_dialog);
  lv_label_set_long_mode(lb_preview_text, LV_LABEL_LONG_WRAP);                                       /*Break the long lines*/
  lv_label_set_recolor(lb_preview_text, true);                                                       /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_align(lb_preview_text, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT); /*Center aligned lines*/
  lv_label_set_text(lb_preview_text, "1 2 3 4 5 6");
  lv_obj_set_width(lb_preview_text, 115);
  lv_obj_add_style(lb_preview_text, &style_dialog_preview_text, LV_PART_MAIN);
  lv_obj_align(lb_preview_text, LV_ALIGN_TOP_LEFT, 115, dialog_widget_posY[3]); // in Parent

  /****************************** Square ******************************/
  // Create style
  static lv_style_t style_dialog_line;
  lv_style_init(&style_dialog_line);
  lv_style_set_line_width(&style_dialog_line, 2);
  lv_style_set_line_color(&style_dialog_line, lv_palette_main(LV_PALETTE_CYAN));
  lv_style_set_line_rounded(&style_dialog_line, true);
  // Table
  static lv_point_t line_points_dialog[] = {{0, 0}, {15, 0}, {15, 20}, {0, 20}, {0, 0}};
  line1_dialog_text = lv_line_create(screen_dialog);
  lv_line_set_points(line1_dialog_text, line_points_dialog, 5);          /*Set the points*/
  lv_obj_add_style(line1_dialog_text, &style_dialog_line, LV_PART_MAIN); /*Set the points*/
  lv_obj_align(line1_dialog_text, LV_ALIGN_TOP_LEFT, 113, dialog_widget_posY[3] + 1);
}

void ui_dialog_button(void)
{
  lv_obj_t *dialog_button[2];

  /*Create style*/
  static lv_style_t style_dialog_button;
  lv_style_init(&style_dialog_button);
  lv_style_set_bg_color(&style_dialog_button, lv_color_hex(0x004000));
  lv_style_set_text_color(&style_dialog_button, lv_color_hex(0x000000));

  // button 1
  dialog_button[0] = lv_btn_create(screen_dialog);
  lv_obj_set_size(dialog_button[0], 120, 30);
  lv_obj_add_style(dialog_button[0], &style_dialog_button, LV_PART_MAIN);
  lv_obj_set_pos(dialog_button[0], 10, dialog_widget_posY[4]);
  lb_dialog_button[0] = lv_label_create(dialog_button[0]);
  lv_obj_set_style_text_color(lb_dialog_button[0], lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(lb_dialog_button[0], "Next");

  // button 2
  dialog_button[1] = lv_btn_create(screen_dialog);
  lv_obj_set_size(dialog_button[1], 120, 30);
  lv_obj_add_style(dialog_button[1], &style_dialog_button, LV_PART_MAIN);
  lv_obj_set_pos(dialog_button[1], 190, dialog_widget_posY[4]);
  lb_dialog_button[1] = lv_label_create(dialog_button[1]);
  lv_obj_set_style_text_color(lb_dialog_button[1], lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(lb_dialog_button[1], "Confirm");
}

void ui_show_hide_text_input(uint8_t visible) // 0: hide - 1:visible
{
  if (visible)
  {
    lv_obj_add_flag(lb_text_input_BG, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lb_text_input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lb_preview_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(line1_dialog_text, LV_OBJ_FLAG_HIDDEN);
  }
  else
  {
    lv_obj_clear_flag(lb_text_input_BG, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(lb_text_input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(lb_preview_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(line1_dialog_text, LV_OBJ_FLAG_HIDDEN);
  }
  // lv_obj_set_hidden(lb_text_input_BG, (visible == 0) ? true : false);
  // lv_obj_set_hidden(lb_text_input, (visible == 0) ? true : false);
  // lv_obj_set_hidden(lb_preview_text, (visible == 0) ? true : false);
  // lv_obj_set_hidden(line1_dialog_text, (visible == 0) ? true : false);
}

void UI_dialog_update(void)
{
  if (ui_dialog.update.Bit.header)
  {
    lv_label_set_text(lb_dialog_header, ui_dialog.header._char);
  }
  if (ui_dialog.update.Bit.context)
  {
    lv_label_set_text(lb_dialog_context, ui_dialog.context);
  }
  if (ui_dialog.update.Bit.text_input)
  {
    if (ui_dialog.sum_text_input != 0)
    {
      lv_label_set_text(lb_text_input, ui_dialog.text_input);
    }
  }
  if (ui_dialog.update.Bit.index_text_input)
  {
    if (ui_dialog.sum_text_input != 0)
    {
      temp_char_blink = ui_dialog.text_input[ui_dialog.index_text_input];
    }
  }
  if (ui_dialog.update.Bit.text_preview)
  {
    if (ui_dialog.sum_text_input != 0)
    {
      dialog_array_string[11] = 0;
      for (uint8_t iStr = 0; iStr < 11; iStr++)
      {
        if ((iStr % 2) == 0)
        {
          dialog_array_string[iStr] = ui_dialog.text_preview[iStr / 2];
        }
        else
        {
          dialog_array_string[iStr] = ' ';
        }
      }
      lv_label_set_text(lb_preview_text, (char *)&dialog_array_string);
    }
  }
  if (ui_dialog.update.Bit.button_L)
  {
    lv_label_set_text(lb_dialog_button[0], ui_dialog.button_L);
  }
  if (ui_dialog.update.Bit.button_R)
  {
    lv_label_set_text(lb_dialog_button[1], ui_dialog.button_R);
  }
  // reset bit
  {
    ui_dialog.update.Word = 0;
  }
}

void UI_dialog_timer_blink(void)
{
  if (screen_active_flag.Bit.dialog)
  {
    if (ui_dialog.sum_text_input != 0)
    {
      //
      if (times_blink_dialog == 0)
      {
        ui_dialog.text_input[ui_dialog.index_text_input] = '_';
        lv_label_set_text(lb_text_input, ui_dialog.text_input);
      }
      else
      {
        ui_dialog.text_input[ui_dialog.index_text_input] = temp_char_blink;
        lv_label_set_text(lb_text_input, ui_dialog.text_input);
      }
      //
      if (times_blink_dialog == 0)
        times_blink_dialog = 1;
      else
        times_blink_dialog = 0;
    }
  }
}

void UI_dialog_loadscreen(void)
{
  lv_scr_load(screen_dialog);
  screen_active_flag.Bit.main = 0;
  screen_active_flag.Bit.menu = 0;
  screen_active_flag.Bit.dialog = 1;
  screen_active_flag.Bit.compass = 0;
  if (ui_dialog.sum_text_input != 0)
    ui_show_hide_text_input(1);
  else
    ui_show_hide_text_input(0);
}

void UI_dialog_init(void)
{
  screen_dialog = lv_obj_create(NULL);
  ui_dialog_border();
  ui_dialog_header_context();
  ui_dialog_text_input();
  ui_dialog_preview_text();
  ui_dialog_button();

  // TEST
  memcpy(ui_dialog.header._char, "INPUT NUMBER", 12);
  memcpy(ui_dialog.context, "Type TIME with form hhmmss in 24h", 35);
  ui_dialog.sum_text_input = 6;
  ui_dialog.index_text_input = 1;
  memcpy(ui_dialog.text_input, "124", 3);
  memcpy(ui_dialog.text_preview, "123456", 6);
  memcpy(ui_dialog.button_L, "Next ->", 7);
  memcpy(ui_dialog.button_R, "Select", 6);
  //
  ui_dialog.update.Word = 0xffffffff;
  UI_dialog_update();
  //
  //  uint8_t test[6];
  //  for(uint8_t iT=0; iT<6; iT++)
  //  {
  //    test[iT] = times_blink_dialog + iT + '0';
  //  }
  //  memcpy(ui_dialog.text_preview, test, 6);
  //  ui_dialog.update.Bit.text_preview = 1;
  //  UI_dialog_update();
}
