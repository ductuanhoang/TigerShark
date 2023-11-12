#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "main.h"
#include <stdbool.h>


typedef struct
{
    int16_t offset;
    float   slope;
    float   scaleFactor;
} calib_value;

typedef struct
{
    calib_value data[3]; /*three sensor*/
    uint16_t         crc;
} sensor_calib;

#define MIN_ADC_COUNTS      1 

int16_t sensor_calibrated(uint8_t channel, int32_t raw);
void sensors_init(void);
bool sensors_stdCalibratePpO2Sensors(uint16_t *buff);
bool sensors_altCalibratePpO2Sensors(uint16_t *buff,uint16_t alt_mBar, uint16_t surfacePressure_mbar);

#define _vRef_mv             2500
#define count_per_mVolt  ( (float)(1L << 12) / _vRef_mv )

extern bool ok1, ok2, ok3;

#endif
