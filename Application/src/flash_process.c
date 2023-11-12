#include "flash_process.h"
#include "crc16.h"
#include "spi_ffs.h"

saving_data_t config_data;
#define vRef_mv             2500
#define countsPerMillivolt  ((float)(1L << 12) / vRef_mv)

uint32_t add;

//static void Flash_read(uint8_t *rBuf, uint32_t size);
//static void Flash_write(uint8_t *wBuf, uint32_t size);
//
//#define  APPLICATION_INFORMATION    0x081C0000  /*Sector 11*/
//
//static void Flash_read(uint8_t *rBuf, uint32_t size)
//{
//    uint32_t flash_data;
//    for(uint32_t current_id =0; current_id < size; current_id +=4)
//    {
//       flash_data = *(uint32_t*)(APPLICATION_INFORMATION + current_id);
//       rBuf[current_id] = flash_data & 0xFF;
//       rBuf[current_id + 1] = (flash_data & 0xFF00) >> 8;
//       rBuf[current_id + 2] = (flash_data & 0xFF0000) >> 16;
//       rBuf[current_id + 3] = flash_data >> 24;
//    }
//}
//
//static void Flash_write(uint8_t *wBuf, uint32_t size)
//{
//    uint32_t data_to_flash;
//    HAL_FLASH_Unlock();
//    FLASH_Erase_Sector(FLASH_SECTOR_11,FLASH_VOLTAGE_RANGE_3);
//    for(uint16_t id =0 ; id < size; id+=4)
//    {
//        data_to_flash = ((uint32_t)wBuf[id] << 0) | ((uint32_t)wBuf[id+1] << 8) | ((uint32_t)wBuf[id+2] << 16) |((uint32_t)wBuf[id+3] << 24);
//        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(APPLICATION_INFORMATION + id),data_to_flash);
//    }          
//    HAL_FLASH_Lock(); 
//}
static void set_default_config(void)
{
  /*control settings*/
    config_data.user_setting.controllerMode = CONTROLLER_MODE_PRIMARY;
    config_data.user_setting.setPoint_centiAta =  100;
    config_data.user_setting.gasMixO2_percent =  21;
    config_data.user_setting.gasMixHe_percent =   0;
    config_data.user_setting.gradientHigh = 100;
    config_data.user_setting.gradientLow =  10;
    config_data.user_setting.circuitMode = CLOSED_CIRCULT;
    config_data.user_setting.solMode = SOLENOID_MODE_JUERGENSEN;
    config_data.user_setting.autoShutdownTime_min =  10;
    config_data.user_setting.batteryWarning_deciVolt =  38;
    config_data.user_setting.diveLedBrightness_percent = 100;
    config_data.user_setting.decoLedBrightness_percent = 100;
    config_data.user_setting.flashlightBrightness_percent = 100;
    config_data.user_setting.flashlightTimeout_sec = 360;
    config_data.user_setting.divaSigLedTimeout_sec = 360;
    config_data.user_setting.stackTime = 360;
    config_data.user_setting.backlightTimeout_sec = 10;
    config_data.user_setting.newDeviceTime_sec =   0;
    config_data.user_setting.settings.all = 154172;
     /*gas settings*/;
    for(uint8_t i =0; i < 10; i++)
    {
        config_data.ui_value.gas_define[i].He =0 + i;
        config_data.ui_value.gas_define[i].O2 =21 + i;
        sprintf(config_data.ui_value.gas_define[i].name,"GAS MIX %d",i+1);
    }
    config_data.ui_value.gradient[0].gHigh = 100;
    config_data.ui_value.gradient[0].gLow = 10;
    
    config_data.ui_value.gradient[1].gHigh = 95;
    config_data.ui_value.gradient[1].gLow = 20;
    
    config_data.ui_value.gradient[2].gHigh = 85;
    config_data.ui_value.gradient[2].gLow = 25;
    
    config_data.ui_value.gradient[3].gHigh = 75;
    config_data.ui_value.gradient[3].gLow = 30;
    
    config_data.ui_value.gradient[4].gHigh = 70;
    config_data.ui_value.gradient[4].gLow = 35;
    
    config_data.ui_value.gradient[5].gHigh = 71;
    config_data.ui_value.gradient[5].gLow = 36;
    
    config_data.ui_value.setpoint_value[0].value = 40;
    config_data.ui_value.setpoint_value[0].edit =  0;
    config_data.ui_value.setpoint_value[1].value = 70;
    config_data.ui_value.setpoint_value[1].edit =  0;
    config_data.ui_value.setpoint_value[2].value = 100;
    config_data.ui_value.setpoint_value[2].edit =  0;
    config_data.ui_value.setpoint_value[3].value = 120;
    config_data.ui_value.setpoint_value[3].edit =  0;
    config_data.ui_value.setpoint_value[4].value = 40;
    config_data.ui_value.setpoint_value[4].edit =  1;
    config_data.ui_value.setpoint_value[5].value = 40;
    config_data.ui_value.setpoint_value[5].edit =  1;
    /*cab settings*/
    for(uint8_t i  = 0; i < 3; i++)
    {
        config_data.cal.data[i].offset = 0;
        config_data.cal.data[i].slope = 100.00 / (50.00 * countsPerMillivolt * 10.60);
        config_data.cal.data[i].scaleFactor = 1;
    }
    config_data.cal.crc = vp_crc16(0, &config_data.cal.data[0], sizeof(config_data.cal.data));
    config_data.code_check1 = 'A';
    config_data.code_check2 = 'A';
}
void at_config_load_info(void)
{
    //Flash_read((uint8_t*)&config_data, sizeof(saving_data_t));
}

void at_config_init(void)
{
   spiffs_file fd = SPIFFS_open(&fs, "config_data", SPIFFS_RDWR, 0);
   if(fd < 0)
   {
      SPIFFS_close(&fs, fd);
      set_default_config();
      fd = SPIFFS_open(&fs, "config_data", SPIFFS_CREAT| SPIFFS_TRUNC | SPIFFS_RDWR, 0);
      if (SPIFFS_write(&fs, fd,(uint8_t *)(&config_data), sizeof(saving_data_t)) < 0) printf("errno %i\n", SPIFFS_errno(&fs));
      SPIFFS_close(&fs, fd);
   }
   else
   {
      if(SPIFFS_read(&fs, fd, (uint8_t *)(&config_data), sizeof(saving_data_t)) < 0) printf("errno %i\n", SPIFFS_errno(&fs));
      SPIFFS_close(&fs, fd);
   }
   
}
void at_config_set_info(void)
{
  spiffs_file fd = SPIFFS_open(&fs, "config_data", SPIFFS_RDWR, 0);
  if (SPIFFS_write(&fs, fd,(uint8_t *)(&config_data), sizeof(saving_data_t)) < 0) printf("errno %i\n", SPIFFS_errno(&fs));
      SPIFFS_close(&fs, fd);
}

