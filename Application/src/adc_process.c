#include "adc_process.h"
#include "adc.h"
#include "dma.h"
#include "sensors_calibration.h"
#include "main_process.h"
#include "tim.h"
//#include "gpio.h"

volatile uint16_t adc_value[ADC_CHANNEL_COUNT][ADC_SAMPLE_COUNT];
volatile uint16_t adc_buffer_sample[ADC_CHANNEL_COUNT * ADC_SAMPLE_COUNT];
uint16_t adc_value_avg[ADC_CHANNEL_COUNT];
static uint8_t collum_adc_index =0;
uint16_t adc_raw_value[ADC_CHANNEL_COUNT];

static void adc_avg_value(void)
{
    uint8_t i,j;
    uint32_t sumTmp = 0;
    for(j = 0; j < ADC_CHANNEL_COUNT; j++)
    {
        sumTmp = 0;
        for(i = 0; i < ADC_SAMPLE_COUNT; i++)
        {
            sumTmp += adc_value[j][i]; 
        }
        adc_value_avg[j] = sumTmp / ADC_SAMPLE_COUNT;
    }     
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    uint16_t i = 0, j = 0;
    uint32_t sumTmp = 0;
    HAL_TIM_Base_Stop(&htim4);
    for(j = 0; j < ADC_CHANNEL_COUNT; j++) {
        sumTmp = 0;
        for(i = 0; i < ADC_SAMPLE_COUNT; i++)
        {
            sumTmp += (adc_buffer_sample[i * ADC_CHANNEL_COUNT + j]); // 0 3 6 9 12
        }
        adc_value[j][collum_adc_index]  = sumTmp / ADC_SAMPLE_COUNT;
    }
    collum_adc_index++;
    if(collum_adc_index >= ADC_SAMPLE_COUNT)
    {
        collum_adc_index =0;
        adc_avg_value();
    }
    //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_buffer_sample, ADC_CHANNEL_COUNT * ADC_SAMPLE_COUNT);
    HAL_TIM_Base_Start(&htim4);
}

void adc_start_process(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_buffer_sample, ADC_CHANNEL_COUNT * ADC_SAMPLE_COUNT);
    HAL_TIM_Base_Start(&htim4);
}
void adc_calculate(uint16_t *sensor, uint16_t *avg)
{
    uint16_t i =0;
    for( i =0 ; i < 3; i++)
    {
      sensor[i] = sensor_calibrated(i,adc_value_avg[i]);
      avg[i] = adc_value_avg[i];
      adc_raw_value[i] = (uint16_t)((adc_value_avg[i] / count_per_mVolt)*0.95);
    }
}
uint16_t volt_calculate(void) /*get baterry value*/ 
{
    uint32_t temp = (uint16_t)((adc_value_avg[BATTERY_INDEX]/count_per_mVolt)*2);
    return temp;
}
bool adc_std_calib(void)
{
    return sensors_stdCalibratePpO2Sensors(systemStatus.ppO2_avg_sensor);
}
bool adc_alt_calib(void)
{
    
    return sensors_altCalibratePpO2Sensors(systemStatus.ppO2_avg_sensor,systemStatus.rawPressure_mBar,systemStatus.surfacePressure_mBar);
}