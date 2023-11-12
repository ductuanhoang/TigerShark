#ifndef _ADC_PROCESS_H_
#define _ADC_PROCESS_H_

#include "main.h"
#include <stdbool.h>

#define ADC_SAMPLE_COUNT     40
#define ADC_CHANNEL_COUNT    7


#define O2_SENSOR1_INDEX     0
#define O2_SENSOR2_INDEX     1
#define O2_SENSOR3_INDEX     2
#define CURRENT_INDEX        3
#define BATTERY_INDEX        4

#define VREF_INDEX           6



void adc_start_process(void);

void adc_calculate(uint16_t *sensor, uint16_t *avg);
uint16_t volt_calculate(void);
bool adc_std_calib(void);
bool adc_alt_calib(void);

extern uint16_t adc_value_avg[ADC_CHANNEL_COUNT];
extern uint16_t adc_raw_value[ADC_CHANNEL_COUNT];

#endif