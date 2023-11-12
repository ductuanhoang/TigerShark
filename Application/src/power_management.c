#include "power_management.h"
#include "tim.h"

void power_board_on(void)
{
    HAL_GPIO_WritePin(WAKE_PW_EN_GPIO_Port,WAKE_PW_EN_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(O2_SENSOR_EN_GPIO_Port,O2_SENSOR_EN_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(DIVA_PW_EN_GPIO_Port,DIVA_PW_EN_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(COM_PW_EN_GPIO_Port,COM_PW_EN_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LIGHT_SENSOR_EN_GPIO_Port,LIGHT_SENSOR_EN_Pin,GPIO_PIN_SET);
    
    HAL_GPIO_WritePin(WIFI_PW_EN_GPIO_Port,WIFI_PW_EN_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(WIFI_EN_GPIO_Port,WIFI_EN_Pin,GPIO_PIN_SET);
    
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim3,TIM_CHANNEL_3);
    
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
    HAL_TIMEx_PWMN_Start(&htim3,TIM_CHANNEL_4);
    
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
    HAL_TIMEx_PWMN_Start(&htim2,TIM_CHANNEL_4);


    
}
 
void power_board_off(void)
{
    HAL_GPIO_WritePin(WAKE_PW_EN_GPIO_Port,WAKE_PW_EN_Pin,GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(O2_SENSOR_EN_GPIO_Port,O2_SENSOR_EN_Pin,GPIO_PIN_SET);
}

void power_lcd_on(void)
{
    HAL_GPIO_WritePin(LCD_PW_EN_GPIO_Port,LCD_PW_EN_Pin,GPIO_PIN_SET);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
    //HAL_GPIO_WritePin(LCD_BL_EN_GPIO_Port,LCD_BL_EN_Pin,GPIO_PIN_SET);
}

void change_brightness_lcd(uint32_t bright)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, bright);
}