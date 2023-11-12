#ifndef _MS_5837_H_
#define _MS_5837_H_
#include <stdbool.h>
#include "main.h"

typedef enum
{
    SEND_CMD_D1_CONVERT =0,
    SEND_CMD_READ_PRESS_VALUE,
    RECEIVE_PRES_VALUE,
    SEND_CMD_D2_CONVERT,
    SEND_CMD_READ_TEMP_VALUE,
    RECEIVE_TEMP_VALUE,
    CALCULATE_ALL,
}e_ms5837_state;

typedef enum
{
    TEMP_C =0,
    TEMP_F,
}e_temp_type;

void MS5837_read();
uint8_t ms5837_crc4(uint16_t *n_prom);
bool ms5837_init(void);
void ms5837_calculate();
float ms5837_pressure(float conversion);
float ms5837_temperature();
float ms5837_depth();
float ms5837_altitude();
int16_t get_temperate(e_temp_type type);
void get_temperature_and_pressure(int8_t *temp, uint16_t *deth, uint16_t *mbar);
void pressure_sensor_init(void);
uint16_t get_pressure_mbar(void);

#endif