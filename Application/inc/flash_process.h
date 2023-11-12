#ifndef __FLASH_PROCESS_H__
#define __FLASH_PROCESS_H__

#include "main.h"
#include "main_process.h"
#include "ui_control.h"
#include "sensors_calibration.h"

typedef union __attribute((packed))
{
    struct 
    {
        s_ControlSettings user_setting;
        s_ui_value ui_value;  
        sensor_calib cal;
        uint32_t code_check1;
        uint32_t code_check2;
    };
    uint8_t* raw_data;
}saving_data_t;

extern saving_data_t config_data;
void at_config_set_info(void);
void at_config_init(void);
#endif
