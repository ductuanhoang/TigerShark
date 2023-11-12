#ifndef _FN3161H_H_
#define _FN3161H_H_
#include "main.h"
#include "usart.h"

#define UART_SIM_CTX     huart2
#define MAX_GSM_RX_BUFFER_SIZE          128
#define GSM_CONTROL_LOOP_PERIOD         20 
typedef struct
{
    char *cmd;
    char *expect_response_from_atc;
    uint16_t timeout_atc;
    uint16_t current_timeout_atc;
    uint16_t cmd_length;
    uint8_t retry_cnt_atc;
    void(*send_at_cb)(uint8_t, void*);
} gsm_at_cmd_t;

void wifi_test(void);


#endif