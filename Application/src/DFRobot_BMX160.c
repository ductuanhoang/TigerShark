/*!
 * @file DFRobot_BMX160.cpp
 * @brief define DFRobot_BMX160 class infrastructure, the implementation of basic methods
 * @copyright	Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [luoyufeng] (yufeng.luo@dfrobot.com)
 * @maintainer [Fary](feng.yang@dfrobot.com)
 * @version  V1.0
 * @date  2021-10-20
 * @url https://github.com/DFRobot/DFRobot_BMX160
 */
#include "DFRobot_BMX160.h"
#include "i2c.h"

//static sBmx160Dev_t static_Obmx160;
//static sBmx160SensorData_t static_Oaccel;
//static sBmx160SensorData_t static_Ogyro;
//static sBmx160SensorData_t static_Omagn;

float accelRange = BMX160_ACCEL_MG_LSB_2G;
float gyroRange = BMX160_GYRO_SENSITIVITY_250DPS;
uint8_t _addr = 0xD1;

sBmx160Dev_t Obmx160;

sBmx160SensorData_t Omagn;
sBmx160SensorData_t Oaccel;
sBmx160SensorData_t Ogyro; 

void DFRobot_BMX160(void)
{
//  Obmx160 = (sBmx160Dev_t*)&static_Obmx160;
//  Oaccel = ( sBmx160SensorData_t*)&static_Oaccel;
//  Ogyro = ( sBmx160SensorData_t*)&static_Ogyro;
//  Omagn = ( sBmx160SensorData_t*)&static_Omagn;
//  if (scan() == 1){
      softReset();
      writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x11);
      HAL_Delay(50);
      /* Set gyro to normal mode */
      writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x15);
      HAL_Delay(100);
      /* Set mag to normal mode */
      writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x19);
      HAL_Delay(10);
      setMagnConf();
//    }
}

const uint8_t int_mask_lookup_table[13] = {
    BMX160_INT1_SLOPE_MASK,
    BMX160_INT1_SLOPE_MASK,
    BMX160_INT2_LOW_STEP_DETECT_MASK,
    BMX160_INT1_DOUBLE_TAP_MASK,
    BMX160_INT1_SINGLE_TAP_MASK,
    BMX160_INT1_ORIENT_MASK,
    BMX160_INT1_FLAT_MASK,
    BMX160_INT1_HIGH_G_MASK,
    BMX160_INT1_LOW_G_MASK,
    BMX160_INT1_NO_MOTION_MASK,
    BMX160_INT2_DATA_READY_MASK,
    BMX160_INT2_FIFO_FULL_MASK,
    BMX160_INT2_FIFO_WM_MASK
};

uint8_t data_buff[128];

// bool begin()
// {
//     _pWire->begin();
//     if (scan() == true){
//         softReset();
//         writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x11);
//         HAL_Delay(50);
//         /* Set gyro to normal mode */
//         writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x15);
//         HAL_Delay(100);
//         /* Set mag to normal mode */
//         writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x19);
//         HAL_Delay(10);
//         setMagnConf();
//         return true;
//     }
//     else
//         return false;
// }

void setLowPower(){
    softReset();
    HAL_Delay(100);
    setMagnConf();
    HAL_Delay(100);
    writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x12);
    HAL_Delay(100);
    /* Set gyro to normal mode */
    writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x17);
    HAL_Delay(100);
    /* Set mag to normal mode */
    writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x1B);
    HAL_Delay(100);
}

void wakeUp(){
    softReset();
    HAL_Delay(100);
    setMagnConf();
    HAL_Delay(100);
    writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x11);
    HAL_Delay(100);
    /* Set gyro to normal mode */
    writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x15);
    HAL_Delay(100);
    /* Set mag to normal mode */
    writeBmxReg(BMX160_COMMAND_REG_ADDR, 0x19);
    HAL_Delay(100);
}

uint8_t softReset()
{
  int8_t rslt=BMX160_OK;
  if (&Obmx160 == NULL){
    rslt = BMX160_E_NULL_PTR;
  }  
  rslt = _softReset(&Obmx160);
  if (rslt == 0)
    return 1;
  else
    return 0;
}

int8_t  _softReset(sBmx160Dev_t *dev)
{
  int8_t rslt=BMX160_OK;
  uint8_t data = BMX160_SOFT_RESET_CMD;
  if (dev==NULL){
    rslt = BMX160_E_NULL_PTR;
  }
  writeBmxReg(BMX160_COMMAND_REG_ADDR, data);
  HAL_Delay(BMX160_SOFT_RESET_DELAY_MS);
  if (rslt == BMX160_OK){
    defaultParamSettg(dev);
  }  
  return rslt;
}

void defaultParamSettg(sBmx160Dev_t *dev)
{
  // Initializing accel and gyro params with
  dev->gyroCfg.bw = BMX160_GYRO_BW_NORMAL_MODE;
  dev->gyroCfg.odr = BMX160_GYRO_ODR_100HZ;
  dev->gyroCfg.power = BMX160_GYRO_SUSPEND_MODE;
  dev->gyroCfg.range = BMX160_GYRO_RANGE_2000_DPS;
  dev->accelCfg.bw = BMX160_ACCEL_BW_NORMAL_AVG4;
  dev->accelCfg.odr = BMX160_ACCEL_ODR_100HZ;
  dev->accelCfg.power = BMX160_ACCEL_SUSPEND_MODE;
  dev->accelCfg.range = BMX160_ACCEL_RANGE_2G;
  

  dev->prevMagnCfg = dev->magnCfg;
  dev->prevGyroCfg = dev->gyroCfg;
  dev->prevAccelCfg = dev->accelCfg;
}

void setMagnConf()
{
    writeBmxReg(BMX160_MAGN_IF_0_ADDR, 0x80);
    HAL_Delay(50);
    // Sleep mode
    writeBmxReg(BMX160_MAGN_IF_3_ADDR, 0x01);
    writeBmxReg(BMX160_MAGN_IF_2_ADDR, 0x4B);
    // REPXY regular preset
    writeBmxReg(BMX160_MAGN_IF_3_ADDR, 0x04);
    writeBmxReg(BMX160_MAGN_IF_2_ADDR, 0x51);
    // REPZ regular preset
    writeBmxReg(BMX160_MAGN_IF_3_ADDR, 0x0E);
    writeBmxReg(BMX160_MAGN_IF_2_ADDR, 0x52);
    
    writeBmxReg(BMX160_MAGN_IF_3_ADDR, 0x02);
    writeBmxReg(BMX160_MAGN_IF_2_ADDR, 0x4C);
    writeBmxReg(BMX160_MAGN_IF_1_ADDR, 0x42);
    writeBmxReg(BMX160_MAGN_CONFIG_ADDR, 0x08);
    writeBmxReg(BMX160_MAGN_IF_0_ADDR, 0x03);
    HAL_Delay(50);
}

void setGyroRange(eGyroRange_t bits){
    switch (bits){
        case eGyroRange_125DPS:
            gyroRange = BMX160_GYRO_SENSITIVITY_125DPS;
            break;
        case eGyroRange_250DPS:
            gyroRange = BMX160_GYRO_SENSITIVITY_250DPS;
            break;
        case eGyroRange_500DPS:
            gyroRange = BMX160_GYRO_SENSITIVITY_500DPS;
            break;
        case eGyroRange_1000DPS:
            gyroRange = BMX160_GYRO_SENSITIVITY_1000DPS;
            break;
        case eGyroRange_2000DPS:
            gyroRange = BMX160_GYRO_SENSITIVITY_2000DPS;
            break;
        default:
            gyroRange = BMX160_GYRO_SENSITIVITY_250DPS;
            break;
    }
}

void setAccelRange(eAccelRange_t bits){
    switch (bits){
        case eAccelRange_2G:
            accelRange = BMX160_ACCEL_MG_LSB_2G;
            break;
        case eAccelRange_4G:
            accelRange = BMX160_ACCEL_MG_LSB_4G;
            break;
        case eAccelRange_8G:
            accelRange = BMX160_ACCEL_MG_LSB_8G;
            break;
        case eAccelRange_16G:
            accelRange = BMX160_ACCEL_MG_LSB_16G * 10;
            break;
        default:
            accelRange = BMX160_ACCEL_MG_LSB_2G ;
            break;
    }
}

void getAllData(sBmx160SensorData_t *magn, sBmx160SensorData_t *gyro, sBmx160SensorData_t *accel){

    uint8_t data[23] = {0};
    int16_t x=0,y=0,z=0;
    // put your main code here, to run repeatedly:
    readReg(BMX160_MAG_DATA_ADDR, data, 23);
    if(magn){
        x = (int16_t) (((uint16_t)data[1] << 8) | data[0]);
        y = (int16_t) (((uint16_t)data[3] << 8) | data[2]);
        z = (int16_t) (((uint16_t)data[5] << 8) | data[4]);
        magn->x = x * BMX160_MAGN_UT_LSB;
        magn->y = y * BMX160_MAGN_UT_LSB;
        magn->z = z * BMX160_MAGN_UT_LSB;
    }
    if(gyro){
        x = (int16_t) (((uint16_t)data[9] << 8) | data[8]);
        y = (int16_t) (((uint16_t)data[11] << 8) | data[10]);
        z = (int16_t) (((uint16_t)data[13] << 8) | data[12]);
        gyro->x = x * gyroRange;
        gyro->y = y * gyroRange;
        gyro->z = z * gyroRange;
    }
    if(accel){
        x = (int16_t) (((uint16_t)data[15] << 8) | data[14]);
        y = (int16_t) (((uint16_t)data[17] << 8) | data[16]);
        z = (int16_t) (((uint16_t)data[19] << 8) | data[18]);
        accel->x = x * accelRange;
        accel->y = y * accelRange;
        accel->z = z * accelRange;
    }
}

void writeBmxReg(uint8_t reg, uint8_t value)
{
    uint8_t buffer[1] = {value};
    writeReg(reg, buffer, 1);
}

void writeReg(uint8_t reg, uint8_t *pBuf, uint16_t len)
{
  data_buff[0] = reg;
  for(uint16_t i = 0; i < len; i++)
  {
    data_buff[i+1] = pBuf[i];
  }
   // HAL_I2C_Master_Transmit(&hi2c1,dev_addr,read_data,len,100);
    HAL_I2C_Master_Transmit(&hi2c1,_addr,data_buff,len+1,100);
}

void readReg(uint8_t reg, uint8_t *pBuf, uint16_t len)
{

    HAL_I2C_Master_Transmit(&hi2c1,_addr,&reg,1,100);
    HAL_I2C_Master_Receive(&hi2c1,_addr,pBuf,len,100);
}

uint8_t scan()
{
    if(HAL_I2C_IsDeviceReady(&hi2c1, _addr, 2,5) == HAL_OK)
    {
        return 1;
    }
    return 0;
}