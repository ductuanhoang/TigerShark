/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_process.h"
#include "ms5837.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "main_process.h"
#include "ui_main.h"
#include "ui_menu.h"
#include "ui_dialog.h"
#include "ui_diagnotics.h"
#include "ui_splash.h"
#include "spi_ffs.h"
#include "power_management.h"
#include "flash_process.h"
#include "button_process.h"
#include "diva_process.h"
#include "compass.h"
#include "DFRobot_BMX160.h"
#include "veml7700.h"
#include "fn3161h.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FW_VERSION "V1.11"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t deep;
 // uint8_t buff11[5];
//CAN_TxHeaderTypeDef TxHeader;
//CAN_RxHeaderTypeDef RxHeader;
//CAN_FilterTypeDef sFilterConfig;
//
//uint8_t txData[2] = {0x0a,0x0b};
//uint8_t RxData[2];
//
//uint32_t TxMailBox = 0;
//
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//  /* Get RX message */
//  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
//  {
//    /* Reception Error */
//    Error_Handler();
//  }
//
//  /* Display LEDx */
//  if ((RxHeader.StdId == 0x447) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 2))
//  {
//    //HAL_Delay(1);
//  }
//
//}


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_FMC_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART3_UART_Init();
  MX_CAN1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  power_board_on();
  
//  sFilterConfig.FilterBank = 0;
//  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
//  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
//  sFilterConfig.FilterIdHigh = 0x0000;
//  sFilterConfig.FilterIdLow = 0x0000;
//  sFilterConfig.FilterMaskIdHigh = 0x0000;
//  sFilterConfig.FilterMaskIdLow = 0x0000;
//  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
//  sFilterConfig.FilterActivation = ENABLE;
//  sFilterConfig.SlaveStartFilterBank = 14;
//  HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
//    
//  
//  HAL_CAN_Start(&hcan1);
    
  
//  HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
//  
//  TxHeader.DLC = 2;
//  TxHeader.IDE = CAN_ID_STD;
//  TxHeader.RTR = CAN_RTR_DATA;
//  TxHeader.StdId = 0x123;
//  //TxHeader.StdId = 0x447;
//  TxHeader.TransmitGlobalTime = DISABLE;
  
 // HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
//  for(uint8_t loop = 0; loop < 10; loop++)
//  {
//    HAL_CAN_AddTxMessage(&hcan1,&TxHeader,txData,&TxMailBox);
//    HAL_Delay(500);
//  }
  power_lcd_on();
  ms5837_init();
  spiffs_flash_init();
  adc_start_process();
  
  at_config_init();
  
  lv_init();
  lv_port_disp_init();
  UI_main_init();
  UI_menu_init();
  UI_dialog_init();
  UI_compass_init();
  UI_diagnotics_init();
  
  rotate_screen(config_data.user_setting.settings.flags.screenRotation);
  UI_splash_init(FW_VERSION);
  UI_splash_loadscreen();
  static uint32_t current =0;
  current =  HAL_GetTick();
  
  while((HAL_GetTick() - current) < 2000)
  {
    static uint32_t plash_update = 0;
    if (HAL_GetTick() - plash_update >= 20)
    {
      plash_update = HAL_GetTick();
      lv_task_handler();
      MS5837_read();    
    }
  }
  power_on_lux_sensor();
  
  systemStatus.setpoint_ata_percent = config_data.user_setting.setPoint_centiAta;
  systemStatus.gasMixHe_percent = config_data.user_setting.gasMixHe_percent;
  systemStatus.gasMixO2_percent = config_data.user_setting.gasMixO2_percent;
  systemStatus.gradientLo = config_data.user_setting.gradientLow;
  systemStatus.gradientHi = config_data.user_setting.gradientHigh;
  systemStatus.circuitMode = config_data.user_setting.circuitMode;
  systemStatus.solMode = config_data.user_setting.solMode;
 
  UI_main_loadscreen();
  main_screen_init();
  diveSettings_init();
  DFRobot_BMX160();
  compass_init();
  button_init();
  //wifi_test();
  // UI_compass_loadscreen();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    static uint32_t lcd_update_prev = 0;
    if (HAL_GetTick() - lcd_update_prev >= 10)
    {
      lcd_update_prev = HAL_GetTick();
      //vsm_cli_main_loop(0);
      lv_task_handler();
    }
    
     static uint32_t screen_update_ticks =0;
    if (HAL_GetTick() - screen_update_ticks >= 500)
    {
      screen_update_ticks = HAL_GetTick();
      adc_calculate(systemStatus.ppO2_atm_percent,systemStatus.ppO2_avg_sensor);
      screen_update();
      auto_exit_setting_mode();
      control_bright_screen();
    }
    
    static uint32_t tick_1000ms = 0;
    if (HAL_GetTick() - tick_1000ms >= 1000)
    {
      tick_1000ms = HAL_GetTick();
      main_control();
      diva_timeout_update();
      control_indicator_led();
      
    }
    
    static uint32_t tick_100ms = 0;
    if (HAL_GetTick() - tick_100ms >= 100)
    {
      tick_100ms = HAL_GetTick();
      get_temperature_and_pressure(&systemStatus.temperature_C,&systemStatus.depth_decimeters,&systemStatus.rawPressure_mBar);
      sensor_control_alarm(systemStatus.ppO2_atm_percent,systemStatus.depth_decimeters,(e_SolenoidMode)systemStatus.solMode);
      depthAndTemperature_sensor_alarm(systemStatus.depth_decimeters,systemStatus.temperature_C);
      control_updateValve();
      sendDivaStatus(&systemStatus,&config_data.user_setting);
      read_lux_sensor();
    }
    
    static uint32_t tick_10ms = 0;
    if (HAL_GetTick() - tick_10ms >= 20)
    {
      tick_10ms = HAL_GetTick();
      button_task();
      MS5837_read();
      compass_loop();
    }
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
/* User can add his own implementation to report the HAL error return state */
__disable_irq();
while (1)
{
}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
/* User can add his own implementation to report the file name and line number,
 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
