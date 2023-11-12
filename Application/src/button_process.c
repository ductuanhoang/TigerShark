#include "button_process.h"
#include "ui_main.h"
#include "ui_menu.h"
#include "ui_dialog.h"
#include "ui_control.h"

extern s_screen_control_t menu_button;
extern s_screen_control_t dialog_button;
extern s_screen_control_t main_button;
extern s_screen_control_t diag_button;
extern s_screen_control_t compass_button;
uint8_t index_left_menu = 0;
uint8_t menu_max_index = 0;
uint8_t index_right_menu = 0;
uint8_t wakeup_flag = 0;

extern uint8_t timeout ;

typedef struct
{
	uint8_t idle_level : 1;
	uint8_t current_pin_state : 1;
	uint8_t stable_state : 1;
	uint8_t last_state : 1;
    uint8_t padding : 4;
    uint8_t debound_count;
    uint32_t hold_time;
    void (*f_cb)(uint8_t);

    // specific for switch count
    uint8_t continuos_press_times;
} button_config_t;

button_config_t btn_cfg[MAX_BTN_COUNT];

uint8_t press_event = 0;
uint8_t release_event = 0;
uint8_t hold_event = 0;
uint8_t hold_event_exec = 0;
uint8_t btn_cos_pressed = 0;
static uint32_t sys_tick = 0;

// specific for switch count
uint8_t continuos_press_times_count = 0;

// for flashing
uint8_t flashing_time_count = 0;
uint8_t is_flashing = 0;

void btn_main_handler(uint8_t event);
void btn_tap_handler(uint8_t event);
void btn_wet_handler(uint8_t event);


void button_init(void)
{
    uint8_t i = 0;

    btn_cfg[BTN_MAIN_IDX].idle_level = 0;
    btn_cfg[BTN_TAP_IDX].idle_level = 0;
    btn_cfg[BTN_WET_IDX].idle_level = 0;

    for(i = 0; i < MAX_BTN_COUNT; i++) {
        btn_cfg[i].last_state = btn_cfg[i].current_pin_state = btn_cfg[i].stable_state = btn_cfg[i].idle_level;
        btn_cfg[i].debound_count = 0;
        btn_cfg[i].hold_time = 0;
    }

    btn_cfg[BTN_TAP_IDX].f_cb = btn_tap_handler;
    btn_cfg[BTN_MAIN_IDX].f_cb = btn_main_handler;
    btn_cfg[BTN_WET_IDX].f_cb = btn_wet_handler;
}

void button_task(void)
{
    uint8_t i = 0;

    btn_cfg[BTN_WET_IDX].current_pin_state =   HAL_GPIO_ReadPin(WET_SW_GPIO_Port,WET_SW_Pin);
    btn_cfg[BTN_MAIN_IDX].current_pin_state =  HAL_GPIO_ReadPin(MAIN_SW_GPIO_Port,MAIN_SW_Pin);
    btn_cfg[BTN_TAP_IDX].current_pin_state =   HAL_GPIO_ReadPin(TAP_SW_GPIO_Port,TAP_SW_Pin);

    // key event detect process
    for(i = 0 ; i < MAX_BTN_COUNT; i++)
    {
        if (btn_cfg[i].current_pin_state ^ btn_cfg[i].last_state)
        {
            if (btn_cfg[i].debound_count++ > DEBOUND_COUNT)
            {
                btn_cfg[i].stable_state = btn_cfg[i].current_pin_state;
                if (btn_cfg[i].stable_state != btn_cfg[i].idle_level)
                {
                    press_event |= 1 << i;
                    // capture hold time
                    btn_cfg[i].hold_time = sys_tick;
                }
                else
                {
                    release_event |= 1 << i;
                    // clear hold time count, on hold time
                    btn_cfg[i].hold_time = 0;
                }
            }
        }
        else
        {
            btn_cfg[i].debound_count = 0;
        }

        btn_cfg[i].last_state = btn_cfg[i].stable_state;
    }
    // event handler process
    for(i = 0; i < MAX_BTN_COUNT; i++) {
        if (btn_cfg[i].stable_state != btn_cfg[i].idle_level)
        {
            if (sys_tick - btn_cfg[i].hold_time >= BTN_HOLD_TIME)
            {
               // hold_event |= 1 << i; // capture hold event
            }
        }
        // process press event
        if ((press_event >> i) & 0x01)
        {
            if (btn_cfg[i].f_cb != NULL)
            {
                btn_cfg[i].f_cb(PRESS_EVENT);
            }

            press_event &= ~(1 << i);
        }

        // process release event
        if ((release_event >> i) & 0x01)
        {
            if (btn_cfg[i].f_cb != NULL)
            {
                btn_cfg[i].f_cb(RELEASE_EVENT);
            }

            release_event &= ~(1 << i);
        }

    }

    sys_tick += TIME_DELAY_TICK;
}


void btn_main_handler(uint8_t event)
{
    if (event == PRESS_EVENT)
    {
      if(screen_active_flag.Bit.main == 1)
      {
         if(main_button.buton_cb[main_button.current_index].f_main != NULL)
         {
            main_button.buton_cb[main_button.current_index].f_main();
         }
      }
      else if(screen_active_flag.Bit.menu == 1)
      {
         if(menu_button.buton_cb[menu_button.current_index].f_main != NULL)
         {
            menu_button.buton_cb[menu_button.current_index].f_main();
         };
       
      }
      else if(screen_active_flag.Bit.diagnotic == 1)
      {
         if(diag_button.buton_cb[diag_button.current_index].f_main != NULL)
         {
            diag_button.buton_cb[diag_button.current_index].f_main();
         }      
      }
      else if(screen_active_flag.Bit.compass == 1)
      {
         if(compass_button.buton_cb[compass_button.current_index].f_main != NULL)
         {
            compass_button.buton_cb[compass_button.current_index].f_main();
         }      
      }
      else
      {
         if(dialog_button.buton_cb[dialog_button.current_index].f_main != NULL)
         {
            dialog_button.buton_cb[dialog_button.current_index].f_main();
         }      
      }
      timeout = 14;
      wakeup_flag = 1;
    }
}

void btn_tap_handler(uint8_t event)
{
    if (event == PRESS_EVENT)
    {
      if(screen_active_flag.Bit.main == 1)
      {
         if(main_button.buton_cb[main_button.current_index].f_tap != NULL)
         {
            main_button.buton_cb[main_button.current_index].f_tap();
         }
      }
      else if (screen_active_flag.Bit.menu == 1)
      {
         if(menu_button.buton_cb[menu_button.current_index].f_tap != NULL)
         {
            menu_button.buton_cb[menu_button.current_index].f_tap();
         };
       
      }
      else if(screen_active_flag.Bit.diagnotic == 1)
      {
         if(diag_button.buton_cb[diag_button.current_index].f_tap != NULL)
         {
            diag_button.buton_cb[diag_button.current_index].f_tap();
         }      
      }
      else if(screen_active_flag.Bit.compass == 1)
      {
         if(compass_button.buton_cb[compass_button.current_index].f_tap != NULL)
         {
            compass_button.buton_cb[compass_button.current_index].f_tap();
         }      
      }
      else
      {
         if(dialog_button.buton_cb[dialog_button.current_index].f_tap != NULL)
         {
            dialog_button.buton_cb[dialog_button.current_index].f_tap();
         }      
      }
      timeout = 14;
     wakeup_flag = 1;
    }
}

void btn_wet_handler(uint8_t event)
{
    if (event == PRESS_EVENT)
    {

    }
    else if(event == RELEASE_EVENT)
	{

	}
    wakeup_flag =1;
}


