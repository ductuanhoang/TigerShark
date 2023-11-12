#include "fn3161h.h"
#include "usart.h"
#include "string.h"

gsm_at_cmd_t gsm_at_cmd_ctx;
uint8_t gsm_rx_buffer[MAX_GSM_RX_BUFFER_SIZE];
uint8_t gsm_rx_idx = 0;
uint32_t time_prev = 0;

void wifi_test(void)
{
   //HAL_UART_Receive_DMA(&huart2,buff1,20);
  // HAL_UART_Transmit(&huart2,"AT",2,100);
}
void gsm_uart_rx_handler(uint8_t rx_data)
{
    gsm_rx_buffer[gsm_rx_idx++] = rx_data;
    if (gsm_rx_idx >= MAX_GSM_RX_BUFFER_SIZE)
    {
        gsm_rx_idx = 0;
    }
  //  gsm_new_uart_data_flag = 1;
}

void gsm_control_init(void)
{
    memset(&gsm_at_cmd_ctx, 0, sizeof(gsm_at_cmd_t));
}

void gsm_clear_rx_buffer(void)
{
    memset(gsm_rx_buffer, 0, MAX_GSM_RX_BUFFER_SIZE);
    gsm_rx_idx = 0;
}


void gsm_control_send_at_cmd(char* at_cmd, char* expect_response, uint16_t timeout_ms,
                            uint8_t retry_count, void(*f_cb)(uint8_t, void*))
{
    gsm_at_cmd_ctx.cmd = at_cmd;
    gsm_at_cmd_ctx.expect_response_from_atc = expect_response;
    gsm_at_cmd_ctx.timeout_atc = timeout_ms;
    gsm_at_cmd_ctx.current_timeout_atc = 0;
    gsm_at_cmd_ctx.retry_cnt_atc = retry_count;
    gsm_at_cmd_ctx.cmd_length = strlen(at_cmd);
    gsm_at_cmd_ctx.send_at_cb = f_cb;

    if (at_cmd != NULL)
    {
        gsm_clear_rx_buffer();
        HAL_UART_Transmit(&UART_SIM_CTX, (uint8_t *)at_cmd, gsm_at_cmd_ctx.cmd_length, 100);
    }
}
void gsm_control_task(void)
{
    if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE)!= 0))
    {
        gsm_uart_rx_handler(huart2.Instance->RDR);
    }
    if(HAL_GetTick() - time_prev < GSM_CONTROL_LOOP_PERIOD)
    {
        return;
    }

    time_prev = HAL_GetTick();
}