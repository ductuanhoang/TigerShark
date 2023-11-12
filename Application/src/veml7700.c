#include "veml7700.h"
#include "i2c.h"


#define VEML7700_WRITE_ADDR   0x20
#define VEML7700_READ_ADDR    0x21
#define LUX_LEVEL_GAIN1_100MS 0.0576

uint8_t lux_data[2];
uint16_t lux_data_raw;
uint16_t lux_level;
uint8_t veml7700_cmd[2];
uint8_t veml7700_cfg[4];
HAL_StatusTypeDef ret = HAL_ERROR;

void power_on_lux_sensor(void)
{
     veml7700_cfg[0] = 0x00;
     veml7700_cfg[1] = 0x00;
     veml7700_cfg[2] = 0x00;

     ret = HAL_I2C_Master_Transmit(&hi2c1,VEML7700_WRITE_ADDR,veml7700_cfg,3,500);
    //ret = HAL_I2C_Master_Receive(&hi2c1,VEML7700_READ_ADDR,lux_data,2,100);
}

void read_lux_sensor(void)
{
    //ret = HAL_I2C_IsDeviceReady(&hi2c1,VEML7700_READ_ADDR,5,100);
    veml7700_cmd[0] = 0x05;
//    ret = HAL_I2C_Master_Transmit(&hi2c1,VEML7700_WRITE_ADDR,veml7700_cmd,1,100);
//    ret = HAL_I2C_Master_Receive(&hi2c1,VEML7700_READ_ADDR,lux_data,2,100);
    HAL_I2C_Mem_Read(&hi2c1,VEML7700_WRITE_ADDR,veml7700_cmd[0],1,&lux_data[0],2,500);
    lux_data_raw = (lux_data[1] << 8 | lux_data[0]);
    lux_level = (uint16_t)(lux_data_raw*LUX_LEVEL_GAIN1_100MS);

}