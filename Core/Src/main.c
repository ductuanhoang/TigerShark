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
// #include "lv_theme_default.h"
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
// #define LV_TICK_CUSTOM_SYS_TIME_EXPR HAL_GetTick()
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t deep;
// uint8_t buff11[5];
// CAN_TxHeaderTypeDef TxHeader;
// CAN_RxHeaderTypeDef RxHeader;
// CAN_FilterTypeDef sFilterConfig;
//
// uint8_t txData[2] = {0x0a,0x0b};
// uint8_t RxData[2];
//
// uint32_t TxMailBox = 0;
//
// void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//   /* Get RX message */
//   if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
//   {
//     /* Reception Error */
//     Error_Handler();
//   }
//
//   /* Display LEDx */
//   if ((RxHeader.StdId == 0x447) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 2))
//   {
//     //HAL_Delay(1);
//   }
//
// }

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
lv_obj_t *ui_Screen1;
lv_obj_t *ui_Bar1;
lv_obj_t *ui_Bar2;
lv_obj_t *ui_Label2;
lv_obj_t *ui_Label1;
lv_obj_t *ui_Label7;
lv_obj_t *ui_Label3;
lv_obj_t *ui_Label8;
lv_obj_t *ui_Label4;
lv_obj_t *ui_Button2;
lv_obj_t *ui_Button1;
lv_obj_t *ui_Button3;
lv_obj_t *ui_Button4;
lv_obj_t *ui_Button5;
lv_obj_t *ui_Label9;
lv_obj_t *ui_Label5;
lv_obj_t *ui_Label10;
lv_obj_t *ui_Label6;
lv_obj_t *ui_Label11;
lv_obj_t *ui_Label12;
lv_obj_t *ui_Button6;
lv_obj_t *ui_Button7;
lv_obj_t *ui_Label13;
lv_obj_t *ui_Label14;

// SCREEN: ui_Screen2
void ui_Screen1_screen_init(void)
{
ui_Screen1 = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_Bar1 = lv_bar_create(ui_Screen1);
lv_bar_set_value(ui_Bar1,25,LV_ANIM_OFF);
lv_obj_set_width( ui_Bar1, 5);
lv_obj_set_height( ui_Bar1, 160);
lv_obj_set_x( ui_Bar1, 10 );
lv_obj_set_y( ui_Bar1, 10 );

ui_Bar2 = lv_bar_create(ui_Screen1);
lv_bar_set_value(ui_Bar2,25,LV_ANIM_OFF);
lv_obj_set_width( ui_Bar2, 5);
lv_obj_set_height( ui_Bar2, 160);
lv_obj_set_x( ui_Bar2, -10 );
lv_obj_set_y( ui_Bar2, 10 );
lv_obj_set_align( ui_Bar2, LV_ALIGN_TOP_RIGHT );

ui_Label2 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label2, -10 );
lv_obj_set_y( ui_Label2, 170 );
lv_obj_set_align( ui_Label2, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_Label2,"N2");
lv_obj_set_style_text_font(ui_Label2, &lv_font_montserrat_10, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label1 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label1, 5 );
lv_obj_set_y( ui_Label1, 170 );
lv_label_set_text(ui_Label1,"ASC");
lv_obj_set_style_text_font(ui_Label1, &lv_font_montserrat_10, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label7 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label7, 149);
lv_obj_set_height( ui_Label7, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label7, 36 );
lv_obj_set_y( ui_Label7, 42 );
lv_label_set_text(ui_Label7,"70");
lv_obj_set_style_text_align(ui_Label7, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_Label7, &lv_font_montserrat_46, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Label7, lv_color_hex(0x381818), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Label7, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Label7, lv_color_hex(0x1762A7), LV_PART_MAIN | LV_STATE_CHECKED|LV_STATE_FOCUSED );
lv_obj_set_style_bg_opa(ui_Label7, 255, LV_PART_MAIN| LV_STATE_CHECKED|LV_STATE_FOCUSED);

ui_Label3 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label3, 55);
lv_obj_set_height( ui_Label3, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label3, -35 );
lv_obj_set_y( ui_Label3, 41 );
lv_obj_set_align( ui_Label3, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_Label3,"71");
lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x16DD48), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_Label3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_44, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label8 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label8, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label8, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label8, -34 );
lv_obj_set_y( ui_Label8, 21 );
lv_obj_set_align( ui_Label8, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_Label8,"NO-DECO");
lv_obj_set_style_text_font(ui_Label8, &lv_font_montserrat_12, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label4 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label4, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label4, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label4, 150 );
lv_obj_set_y( ui_Label4, 20 );
lv_label_set_text(ui_Label4,"FT");
lv_obj_set_style_text_font(ui_Label4, &lv_font_montserrat_12, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button2 = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_Button2, 160);
lv_obj_set_height( ui_Button2, 2);
lv_obj_set_x( ui_Button2, 30 );
lv_obj_set_y( ui_Button2, 100 );
lv_obj_add_flag( ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Button2, lv_color_hex(0x605757), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Button2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button1 = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_Button1, 80);
lv_obj_set_height( ui_Button1, 2);
lv_obj_set_x( ui_Button1, 30 );
lv_obj_set_y( ui_Button1, 175 );
lv_obj_add_flag( ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Button1, lv_color_hex(0x605757), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Button1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button3 = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_Button3, 70);
lv_obj_set_height( ui_Button3, 2);
lv_obj_set_x( ui_Button3, 120 );
lv_obj_set_y( ui_Button3, 175 );
lv_obj_add_flag( ui_Button3, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Button3, lv_color_hex(0x605757), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Button3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button4 = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_Button4, 70);
lv_obj_set_height( ui_Button4, 2);
lv_obj_set_x( ui_Button4, -30 );
lv_obj_set_y( ui_Button4, 175 );
lv_obj_set_align( ui_Button4, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_Button4, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button4, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Button4, lv_color_hex(0x605757), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Button4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button5 = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_Button5, 70);
lv_obj_set_height( ui_Button5, 2);
lv_obj_set_x( ui_Button5, -30 );
lv_obj_set_y( ui_Button5, 100 );
lv_obj_set_align( ui_Button5, LV_ALIGN_TOP_RIGHT );
lv_obj_add_flag( ui_Button5, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button5, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Button5, lv_color_hex(0x605757), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Button5, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label9 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label9, 62);
lv_obj_set_height( ui_Label9, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label9, 40 );
lv_obj_set_y( ui_Label9, 140 );
lv_label_set_text(ui_Label9,"1430");
lv_obj_set_style_text_font(ui_Label9, &lv_font_montserrat_26, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label5 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label5, 61);
lv_obj_set_height( ui_Label5, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label5, 125 );
lv_obj_set_y( ui_Label5, 140 );
lv_label_set_text(ui_Label5,"15");
lv_obj_set_style_text_color(ui_Label5, lv_color_hex(0x16DD48), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_Label5, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_Label5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_Label5, &lv_font_montserrat_26, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label10 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label10, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label10, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label10, 55 );
lv_obj_set_y( ui_Label10, 110 );
lv_label_set_text(ui_Label10,"PSI");

ui_Label6 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label6, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label6, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label6, 142 );
lv_obj_set_y( ui_Label6, 110 );
lv_label_set_text(ui_Label6,"GTR");

ui_Label11 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label11, 55);
lv_obj_set_height( ui_Label11, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label11, -37 );
lv_obj_set_y( ui_Label11, 125 );
lv_obj_set_align( ui_Label11, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_Label11,"70");
lv_obj_set_style_text_align(ui_Label11, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_Label11, &lv_font_montserrat_44, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label12 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label12, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label12, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label12, -44 );
lv_obj_set_y( ui_Label12, 107 );
lv_obj_set_align( ui_Label12, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_Label12,"DIVE-T");
lv_obj_set_style_text_font(ui_Label12, &lv_font_montserrat_12, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button6 = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_Button6, 3);
lv_obj_set_height( ui_Button6, 30);
lv_obj_set_x( ui_Button6, 90 );
lv_obj_set_y( ui_Button6, 190 );
lv_obj_add_flag( ui_Button6, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button6, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Button6, lv_color_hex(0x605757), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Button6, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button7 = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_Button7, 3);
lv_obj_set_height( ui_Button7, 30);
lv_obj_set_x( ui_Button7, 200 );
lv_obj_set_y( ui_Button7, 190 );
lv_obj_add_flag( ui_Button7, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button7, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Button7, lv_color_hex(0x605757), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Button7, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label13 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label13, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label13, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label13, 38 );
lv_obj_set_y( ui_Label13, 197 );
lv_label_set_text(ui_Label13,"GAS 1");

ui_Label14 = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_Label14, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label14, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label14, 127 );
lv_obj_set_y( ui_Label14, 197 );
lv_label_set_text(ui_Label14,"FO2: AIR");

}


void ui_Screen2_screen_init(void);
lv_obj_t *ui_Screen2;
lv_obj_t *ui____initial_actions0;

lv_obj_t *ui_Image1;
lv_obj_t *ui_Bar3;
lv_obj_t *ui_Panel1;
lv_obj_t *ui_Button8;

void ui_Screen2_screen_init(void)
{

ui_Screen2 = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Screen2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Screen2, lv_color_hex(0xF3E5E5), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Screen2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Image1 = lv_img_create(ui_Screen2);
lv_obj_set_width( ui_Image1, 120);
lv_obj_set_height( ui_Image1, 120);
lv_obj_set_x( ui_Image1, 28 );
lv_obj_set_y( ui_Image1, -28 );
lv_obj_set_align( ui_Image1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Image1, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_Image1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_Bar3 = lv_bar_create(ui_Screen2);
lv_bar_set_value(ui_Bar3,25,LV_ANIM_OFF);
lv_obj_set_width( ui_Bar3, 247);
lv_obj_set_height( ui_Bar3, 10);
lv_obj_set_x( ui_Bar3, 0 );
lv_obj_set_y( ui_Bar3, -10 );
lv_obj_set_align( ui_Bar3, LV_ALIGN_BOTTOM_MID );
lv_obj_set_style_anim_time(ui_Bar3, 500, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Panel1 = lv_obj_create(ui_Screen2);
lv_obj_set_width( ui_Panel1, 148);
lv_obj_set_height( ui_Panel1, 157);
lv_obj_set_x( ui_Panel1, -78 );
lv_obj_set_y( ui_Panel1, -30 );
lv_obj_set_align( ui_Panel1, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_Panel1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_Button8 = lv_btn_create(ui_Screen2);
lv_obj_set_width( ui_Button8, 100);
lv_obj_set_height( ui_Button8, 50);
lv_obj_set_x( ui_Button8, 56 );
lv_obj_set_y( ui_Button8, 67 );
lv_obj_set_align( ui_Button8, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Button8, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Button8, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_text_color(ui_Button8, lv_color_hex(0xCD2151), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_Button8, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

}
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
  // ui registers
    // UI_main_init();
    // UI_menu_init();
    // UI_dialog_init();
    // UI_compass_init();
    // UI_diagnotics_init();

//  rotate_screen(config_data.user_setting.settings.flags.screenRotation);
// lv_disp_t *dispp = lv_disp_get_default();
// lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);

// lv_disp_set_theme(dispp, theme);
  ui_Screen1_screen_init();
  ui_Screen2_screen_init();

lv_disp_load_scr( ui_Screen1);
  //  UI_splash_init(FW_VERSION);
  //  UI_splash_loadscreen();
  // lv_scr_load(ui_Screen2);
  static uint32_t current = 0;
  current = HAL_GetTick();

  while ((HAL_GetTick() - current) < 2000)
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
  // wifi_test();
  //  UI_compass_loadscreen();
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
      // vsm_cli_main_loop(0);
      lv_task_handler();
    }

    static uint32_t screen_update_ticks = 0;
    if (HAL_GetTick() - screen_update_ticks >= 500)
    {
      screen_update_ticks = HAL_GetTick();
      adc_calculate(systemStatus.ppO2_atm_percent, systemStatus.ppO2_avg_sensor);
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
      get_temperature_and_pressure(&systemStatus.temperature_C, &systemStatus.depth_decimeters, &systemStatus.rawPressure_mBar);
      sensor_control_alarm(systemStatus.ppO2_atm_percent, systemStatus.depth_decimeters, (e_SolenoidMode)systemStatus.solMode);
      depthAndTemperature_sensor_alarm(systemStatus.depth_decimeters, systemStatus.temperature_C);
      control_updateValve();
      sendDivaStatus(&systemStatus, &config_data.user_setting);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

#ifdef USE_FULL_ASSERT
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
