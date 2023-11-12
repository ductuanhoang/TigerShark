#ifndef __BUTTON_H
#define __BUTTON_H 832021

#include "main.h"
#include "gpio.h"

#define BTN_MAIN_IDX                1
#define BTN_TAP_IDX                 0
#define BTN_WET_IDX                 2


#define MAX_BTN_COUNT               3

#define TIME_DELAY_TICK     10
#define DEBOUND_COUNT       1 // equal 30 ms
#define BTN_HOLD_TIME       3200
#define PRESS_EVENT         0
#define RELEASE_EVENT       1
#define HOLD_EVENT          2
#define CONT_PRESS_TIMES		6

void button_init(void);
void button_task(void);
void reset_state(void);

extern uint8_t wakeup_flag;

#endif

