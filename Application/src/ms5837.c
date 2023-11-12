#include "ms5837.h"
#include "i2c.h"
#include "string.h"
#include "main_process.h"


extern I2C_HandleTypeDef hi2c1;

#define MS5837_ADDR   0xEC
#define TEMPERRATURE   0
#define PRESSURE       1
#define MIN_TEMP       -2000   /*temp x 100*/
#define MAX_TEMP       8500   /*temp x 100*/

#define MIN_PRESS       0   
#define MAX_PRESS       300000   /*press(mBar) * x 10 */

#define SEALEVELSURFACEPRESSURE_MBAR    1013 
#define MAX_DEPTH_DECIMETER    1013 
#define AVERAGE_NUMBER   5 

static uint8_t ms5837_convert_d1 = 0x4A;
static uint8_t ms5837_convert_d2 = 0x5A;
static uint8_t ms5837_read_prom[7] = {0xA0,0xA2,0xA4,0xA6,0xA8,0xAA,0xAC};
static uint8_t ms5837_reset = 0x4A;
static uint8_t ms5837_adc_read = 0x00;
static float ms5837_mbar = 1.0f;
uint8_t rx1_buffer[3];
uint8_t rx2_buffer[3];
uint32_t D2_temp;
uint32_t D1_pres;
uint8_t temp_buffer[2];
uint16_t prom_buffer[8];
int32_t P,TEMP;
uint32_t test_temp,test_pres,test_deth,test_altitu;
static e_ms5837_state ms5837_state = SEND_CMD_D1_CONVERT;
uint8_t offset_flag = 0;
uint16_t press_offset = 0;
uint8_t index = 0;
uint16_t avg_value[3][5];
uint16_t current_depth;
static bool  sensorCalibrated;

static uint16_t surfacePressure_mbar = SEALEVELSURFACEPRESSURE_MBAR;


bool ms5837_init(void) 
{
    // Reset the MS5837, per datasheet
    HAL_I2C_Master_Transmit(&hi2c1,MS5837_ADDR,&ms5837_reset,1,100);
    HAL_Delay(30);    
	// Read calibration values and CRC
	for ( uint8_t i = 0 ; i < 7 ; i++ ) 
    {
        HAL_I2C_Master_Transmit(&hi2c1,MS5837_ADDR,&ms5837_read_prom[i],1,100);
        HAL_Delay(30);
		HAL_I2C_Master_Receive(&hi2c1,MS5837_ADDR,temp_buffer,2,100);
        prom_buffer[i] = (temp_buffer[0] << 8) | (temp_buffer[1]);
	}

	// Verify that data is correct with CRC
	uint8_t crcRead = prom_buffer[0] >> 12;
	uint8_t crcCalculated = ms5837_crc4((uint16_t*)(&prom_buffer[0]));

	if ( crcCalculated != crcRead ) {
        //systemStatus.alerts.flags.unknownDepthAndTemperature = 1;
        sensorCalibrated = false;
		return false; // CRC fail
	}
   // systemStatus.alerts.flags.unknownDepthAndTemperature = 0;
    sensorCalibrated = true;;
	return true;
}

void pressure_sensor_init(void)
{
    if(ms5837_init())
    {
        for(uint8_t i = 0; i < 35; i++)
        {
            MS5837_read();
            HAL_Delay(25);
        }
    } 
}

void MS5837_read(void) 
{
  if(sensorCalibrated)
  {
    switch(ms5837_state)
    {
        case SEND_CMD_D1_CONVERT:
            HAL_I2C_Master_Transmit(&hi2c1,MS5837_ADDR,&ms5837_convert_d1,1,100);
            ms5837_state = SEND_CMD_READ_PRESS_VALUE;
          break;
        case SEND_CMD_READ_PRESS_VALUE:
            HAL_I2C_Master_Transmit(&hi2c1,MS5837_ADDR,&ms5837_adc_read,1,100);
            ms5837_state = RECEIVE_PRES_VALUE;
          break;
        case RECEIVE_PRES_VALUE:
            HAL_I2C_Master_Receive(&hi2c1,MS5837_ADDR,rx1_buffer,3,100);
            D1_pres = ((uint32_t)(rx1_buffer[0]) << 16) | ((uint32_t)(rx1_buffer[1]) << 8) | ((uint32_t)(rx1_buffer[2]));
            memset(rx1_buffer,0,3);
            ms5837_state = SEND_CMD_D2_CONVERT;
          break;
        case SEND_CMD_D2_CONVERT:
            HAL_I2C_Master_Transmit(&hi2c1,MS5837_ADDR,&ms5837_convert_d2,1,100);
            ms5837_state = SEND_CMD_READ_TEMP_VALUE;
          break;
        case SEND_CMD_READ_TEMP_VALUE:
            HAL_I2C_Master_Transmit(&hi2c1,MS5837_ADDR,&ms5837_adc_read,1,100);
            ms5837_state = RECEIVE_TEMP_VALUE;
          break;
        case RECEIVE_TEMP_VALUE:
            HAL_I2C_Master_Receive(&hi2c1,MS5837_ADDR,rx2_buffer,3,100);
            D2_temp = ((uint32_t)(rx2_buffer[0]) << 16) | ((uint32_t)(rx2_buffer[1]) << 8) | ((uint32_t)(rx2_buffer[2]));
            memset(rx2_buffer,0,3);
            ms5837_state = CALCULATE_ALL;
          break;
        case CALCULATE_ALL:
          if((D1_pres > 0) && (D2_temp > 0))
          {
              ms5837_calculate();
              avg_value[0][index] = (uint16_t)ms5837_temperature(); 
              avg_value[2][index] = (uint16_t)ms5837_pressure(ms5837_mbar);
              if(avg_value[2][index] < surfacePressure_mbar)
              {
                    surfacePressure_mbar = avg_value[2][index];
              }
              systemStatus.surfacePressure_mBar = surfacePressure_mbar;
              current_depth = (uint16_t)(ms5837_depth() * 10);
              if(current_depth > MAX_DEPTH_DECIMETER)
                current_depth = MAX_DEPTH_DECIMETER;
              avg_value[1][index] = current_depth; 
              index++;
              if(index >= AVERAGE_NUMBER)
              {
                index = 0;
              }
              systemStatus.alerts.flags.unknownDepthAndTemperature = 0;
          }
          else
          {
            systemStatus.alerts.flags.unknownDepthAndTemperature = 1;
          }
          ms5837_state = SEND_CMD_D1_CONVERT;
    }
  }
}
void ms5837_calculate()
{
	// Given C1-C6 and D1, D2, calculated TEMP and P
	// Do conversion first and then second order temp compensation

	int32_t dT = 0;
	int64_t SENS = 0;
	int64_t OFF = 0;
	int32_t SENSi = 0;
	int32_t OFFi = 0;
	int32_t Ti = 0;
	int64_t OFF2 = 0;
	int64_t SENS2 = 0;

	// Terms called
	dT = D2_temp - (int32_t)(prom_buffer[5])*256;
	SENS = (int64_t)(prom_buffer[1])*32768+((int64_t)(prom_buffer[3])*dT)/256;
	OFF = (int64_t)(prom_buffer[2])*65536+((int64_t)(prom_buffer[4])*dT)/128;
	P = (D1_pres*SENS/(2097152)-OFF)/(8192);

	// Temp conversion
	TEMP = 2000+(int64_t)(dT)*prom_buffer[6]/8388608LL;

	//Second order compensation
	if((TEMP/100)<20)
	{//Low temp
		Ti = (3*(int64_t)(dT)*(int64_t)(dT))/(8589934592LL);
		OFFi = (3*(TEMP-2000)*(TEMP-2000))/2;
		SENSi = (5*(TEMP-2000)*(TEMP-2000))/8;
		if((TEMP/100)< (-15))
		{    //Very low temp
			OFFi = OFFi+7*(TEMP+1500)*(TEMP+1500);
			SENSi = SENSi+4*(TEMP+1500)*(TEMP+1500);
		}
	}
	else if((TEMP/100)>=20)
	{    //High temp
		Ti = 2*(dT*dT)/(137438953472LL);
		OFFi = (1*(TEMP-2000)*(TEMP-2000))/16;
		SENSi = 0;
	}

	OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
	SENS2 = SENS-SENSi;

	TEMP = (TEMP-Ti);
	P = (((D1_pres*SENS2)/2097152-OFF2)/8192);
}

float ms5837_pressure(float conversion) 
{
    return P*conversion/10.0f;
}

float ms5837_temperature() {
	return TEMP/100.0f;
}

// The pressure sensor measures absolute pressure, so it will measure the atmospheric pressure + water pressure
// We subtract the atmospheric pressure to calculate the depth with only the water pressure
// The average atmospheric pressure of 101300 pascal is used for the calcuation, but atmospheric pressure varies
// If the atmospheric pressure is not 101300 at the time of reading, the depth reported will be offset
// In order to calculate the correct depth, the actual atmospheric pressure should be measured once in air, and
// that value should subtracted for subsequent depth calculations.
float ms5837_depth() {
	return (((ms5837_pressure(ms5837_mbar)-surfacePressure_mbar)* 0.0145037) / 1.461505);
}

//float ms5837_altitude() {
//	return (1-pow((ms5837_pressure(ms5837_mbar)/1013.25),0.190284))*145366.45*0.3048;
//}

uint8_t ms5837_crc4(uint16_t *n_prom) 
{
	uint16_t n_rem = 0;

	n_prom[0] = ((n_prom[0]) & 0x0FFF);
	n_prom[7] = 0;

	for ( uint8_t i = 0 ; i < 16; i++ ) {
		if ( i%2 == 1 ) {
			n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
		} else {
			n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
		}
		for ( uint8_t n_bit = 8 ; n_bit > 0 ; n_bit-- ) {
			if ( n_rem & 0x8000 ) {
				n_rem = (n_rem << 1) ^ 0x3000;
			} else {
				n_rem = (n_rem << 1);
			}
		}
	}

	n_rem = ((n_rem >> 12) & 0x000F);

	return n_rem ^ 0x00;
}
int16_t get_temperate(e_temp_type type)
{
  if(type == TEMP_C)
  {
      return TEMP/100;
  }
  else
  {
    return (TEMP*18)/1000 + 32;
  }
}
void get_temperature_and_pressure(int8_t *temp, uint16_t *deth, uint16_t *mbar)
{
  if(systemStatus.alerts.flags.unknownDepthAndTemperature == 0)
  {
    uint32_t sum1,sum2,sum3;
    sum1 = sum2 = sum3 = 0;
    for(uint8_t i =0; i < AVERAGE_NUMBER ; i++)
    {
      sum1 += avg_value[0][i];
      sum2 += avg_value[1][i];
      sum3 += avg_value[2][i];
    }
    *temp = sum1/AVERAGE_NUMBER;
    *deth = sum2/AVERAGE_NUMBER;
    *mbar = sum3/AVERAGE_NUMBER;
  }
}
uint16_t get_pressure_mbar(void)
{
    return (P/10);
}