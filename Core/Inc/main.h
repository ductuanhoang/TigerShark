/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ACC_SA0_Pin GPIO_PIN_1
#define ACC_SA0_GPIO_Port GPIOE
#define FL_CS_Pin GPIO_PIN_9
#define FL_CS_GPIO_Port GPIOB
#define LCD_RESET_Pin GPIO_PIN_6
#define LCD_RESET_GPIO_Port GPIOD
#define CAN_SHDN_Pin GPIO_PIN_12
#define CAN_SHDN_GPIO_Port GPIOC
#define VALVE_CTRL_Pin GPIO_PIN_10
#define VALVE_CTRL_GPIO_Port GPIOA
#define LCD_MODE_SET12_Pin GPIO_PIN_2
#define LCD_MODE_SET12_GPIO_Port GPIOF
#define LCD_MODE_SET0_Pin GPIO_PIN_1
#define LCD_MODE_SET0_GPIO_Port GPIOF
#define FLASHLIGHT_CTRL_Pin GPIO_PIN_9
#define FLASHLIGHT_CTRL_GPIO_Port GPIOA
#define COM_PW_EN_Pin GPIO_PIN_5
#define COM_PW_EN_GPIO_Port GPIOG
#define WIFI_EN_Pin GPIO_PIN_6
#define WIFI_EN_GPIO_Port GPIOC
#define RF_PW_EN_Pin GPIO_PIN_3
#define RF_PW_EN_GPIO_Port GPIOG
#define VALVE_PW_EN_Pin GPIO_PIN_4
#define VALVE_PW_EN_GPIO_Port GPIOG
#define WAKE_PW_EN_Pin GPIO_PIN_6
#define WAKE_PW_EN_GPIO_Port GPIOG
#define LCD_PW_EN_Pin GPIO_PIN_1
#define LCD_PW_EN_GPIO_Port GPIOG
#define DIVA_PW_EN_Pin GPIO_PIN_2
#define DIVA_PW_EN_GPIO_Port GPIOG
#define WIFI_PW_EN_Pin GPIO_PIN_0
#define WIFI_PW_EN_GPIO_Port GPIOG
#define WET_SW_Pin GPIO_PIN_4
#define WET_SW_GPIO_Port GPIOA
#define O2_SENSOR_EN_Pin GPIO_PIN_15
#define O2_SENSOR_EN_GPIO_Port GPIOG
#define MAIN_SW_Pin GPIO_PIN_5
#define MAIN_SW_GPIO_Port GPIOA
#define FL_HOLD_Pin GPIO_PIN_15
#define FL_HOLD_GPIO_Port GPIOB
#define FL_WP_Pin GPIO_PIN_14
#define FL_WP_GPIO_Port GPIOB
#define TAP_SW_Pin GPIO_PIN_6
#define TAP_SW_GPIO_Port GPIOA
#define LIGHT_SENSOR_EN_Pin GPIO_PIN_5
#define LIGHT_SENSOR_EN_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */
//#define GPIO_TURNON    GPIO_PIN_SET 
//#define GPIO_TURNOFF   GPIO_PIN_RESET
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
