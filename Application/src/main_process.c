#include "main_process.h"
#include "spi_ffs.h"
#include "flash_process.h"
//#include "fatfs.h"
//#include "fatfs_platform.h"
//#include "adc_process.h"
#include "tim.h"
#include "rtc.h"
#include "crc16.h"
#include <time.h>

uint16_t  nominalSetpoint_centiAta;
uint32_t   lastDecoLogWrite_sec;
uint32_t lastDiveTime_sec;
uint8_t repeat_count;
uint8_t blink_led = 0;

#define AVERAGESCALE  6
#define MAX_SURFACE_TIME  3596400   /*mean 999 hours*/

DiveSettings diveSettings = 
{
  .stackTimeRemaining_sec = 360,
  .bsMeter_maxDepth_deciMeters = 0,
  .bsMeter_totalDiveTime_sec = 0,
  .average_depth = 0,
  .average_dive_time = 0,    
};
s_deco_log deco_log;

SystemStatus_Deco_t   decoInfo = 
{
    .firstStop_meters   = 0,
    .firstTime_min      = 0,
    .maxMinDepth_meters = 0,
    .timeToSurface_min  = 0,
};
s_SystemStatus systemStatus =
{
    .setpoint_ata_percent = 100,
    .gasMixO2_percent = 21,
    .gasMixHe_percent = 0,
    .gradientLo = 10,
    .gradientHi = 100,
    .circuitMode = SOLENOID_MODE_STANDARD,
    .circuitMode = CLOSED_CIRCULT,
    
};

e_control_state control_state = SURFACE_CONTROL_STATE;
const float   factor =  0.15  * 221.5;


static uint8_t  updateTissuesPeriod = 0;
static uint8_t  updateTimeToSurface = 0;
// control values
static uint8_t nominalPpO2_atm_percent[3] =
{
    // all setpoints are in atm
    /* surface      */  {  40 },
    /* startDescent */  { 100 },
    /* ascended     */  { 100 },
};

uint16_t cycleValveTicks = 60;
uint16_t onValveTicks = 0;

void control_updateValve(void)  /*100 ms*/ 
{
    static uint16_t current_ticks =0;
    static uint8_t update_onvalveticks  =0;
    if((onValveTicks > 0) && (update_onvalveticks ==0))
    {
        update_onvalveticks =1;
        cycleValveTicks = 60;
        current_ticks = onValveTicks;
    }
    if(cycleValveTicks > 0)
    {
        cycleValveTicks--;
        if(cycleValveTicks == 0)
        {
            update_onvalveticks = 0;
        }
    }
        
    if(current_ticks > 0)
    {
       VALVE_CONTROL_ON();
        current_ticks--;       
    }else
    {
       VALVE_CONTROL_OFF();
    }
}
static uint16_t distanceFromOthers(uint16_t a, uint16_t b, uint16_t c)
{
    uint16_t temp = (b + c + 1) / 2;

    return a > temp ? (a - temp): (temp -a);
}

static bool sensors_ppO2SensorsAgree(uint16_t a, uint16_t b, uint16_t sp)
{

    uint16_t d = a > b? a - b: b - a;

    uint16_t ref = (a + b + 1) / 2  >=  sp
        ? (a < b? a: b)  // smaller of a and b
        : (a > b? a: b)  // larger of a and b
        ;

    return d  <  ref / 5;
}

static void calculate_average_value(void)
{
    diveSettings_getSettings(&diveSettings);
    
    if(diveSettings.new_dive_flag)
    {
        diveSettings.number_dives++;
        diveSettings.average_depth = ((diveSettings.average_depth * (diveSettings.number_dives - 1)) + diveSettings.lastDiveDepth_deciMeters) / diveSettings.number_dives;
        diveSettings.average_dive_time = ((diveSettings.average_dive_time * (diveSettings.number_dives - 1)) + diveSettings.lastDiveTime_sec) / diveSettings.number_dives;
        diveSettings.new_dive_flag = 0;
    }      
    diveSettings_saveSettings(&diveSettings);

    systemStatus.bsMeter_diveTime_average = diveSettings.average_dive_time;
    systemStatus.bsMeter_maxDepth_average = diveSettings.average_depth;

}

void sensor_control_alarm(uint16_t *ppO2_atm_percent,uint16_t depth_decimeters,e_SolenoidMode solMode)
{
    uint16_t sp                      = nominalSetpoint_centiAta;
    uint16_t  p1                     = ppO2_atm_percent[0];
    uint16_t  p2                     = ppO2_atm_percent[1];
    uint16_t  p3                     = ppO2_atm_percent[2]; 
    
    uint16_t average_ppO2_atm_percent = 0;
    uint16_t errors_ppO2_atm_percent = 0;

    int  lows                    = 0;
    int  highs                   = 0;
    float valveOpen_ticks   = 0;
    const float barPerAtm = 1.01325;
    
    bool bad1                       = false;
    bool bad2                       = false;
    bool bad3                       = false;

    bool agree_1_2                  = sensors_ppO2SensorsAgree(p1, p2, sp);
    bool agree_1_3                  = sensors_ppO2SensorsAgree(p1, p3, sp);
    bool agree_2_3                  = sensors_ppO2SensorsAgree(p2, p3, sp);
    systemStatus.alerts.flags.badPpO2Sensor1 = false;
    systemStatus.alerts.flags.badPpO2Sensor2  = false;
    systemStatus.alerts.flags.badPpO2Sensor3  = false;
    systemStatus.alerts.flags.unableToControl = false;
    if( agree_1_2 && agree_1_3 && agree_2_3 )
    {
        average_ppO2_atm_percent = (p1 + p2 + p3) / 3;
    }
    else if( !agree_1_2 && !agree_1_3 && !agree_2_3 )
    {
        //onValveTicks =0;
        if( p1 < MIN_LOW_PPO2_ATM_PERCENT  &&  p2 < MIN_LOW_PPO2_ATM_PERCENT  &&  p3 < MIN_LOW_PPO2_ATM_PERCENT )
        {
            //ds                      = displayStatus_controlInactive;
            //indStatus                 = 
          // alarm_status.lowPpO2Atm         = true;
          systemStatus.alerts.flags.ppO2Low = true;
          
        }
        else
        {
            //ds                      = displayStatus_controlFailure;
            //indStatus               = 
            systemStatus.alerts.flags.unableToControl = true;
        }
    }
    else
    {
        if( !agree_1_2 && !agree_1_3 )
            bad1 = true;
        else if( !agree_1_2 && !agree_2_3 )
            bad2 = true;
        else if( !agree_1_3 && !agree_2_3 )
            bad3 = true;
        else
        {
            uint16_t dist1 = distanceFromOthers(p1, p2, p3);
            uint16_t dist2 = distanceFromOthers(p2, p1, p3);
            uint16_t dist3 = distanceFromOthers(p3, p1, p2);

            if( !agree_1_2 )
            {
                if( dist1 > dist2 )
                    bad1 = true;
                else
                    bad2 = true;
            }
            else if( !agree_1_3 )
            {
                if( dist1 > dist3 )
                    bad1 = true;
                else
                    bad3 = true;
            }
            else if( !agree_2_3 )
            {
                if( dist2 > dist3 )
                    bad2 = true;
                else
                    bad3 = true;
            }
            else
            {
               // ASSERT(false);
            }
        } 

        if( bad1 )
        {
            systemStatus.alerts.flags.badPpO2Sensor1 = true;
            average_ppO2_atm_percent = (p2 + p3) / 2;
        }
        else if( bad2 )
        {
           systemStatus.alerts.flags.badPpO2Sensor2      = true;
           average_ppO2_atm_percent = (p1 + p3) / 2;
        }
        else if( bad3 )
        {
           systemStatus.alerts.flags.badPpO2Sensor3    = true;
           average_ppO2_atm_percent = (p1+ p2) / 2;
        }
        else
        {
            //ASSERT(false);
        }     
     }
    systemStatus.ppO2_avg_atm_percent = average_ppO2_atm_percent;
    
    if( config_data.user_setting.settings.flags.ppO2units == 1)
    {
        p1 = (int16_t)(p1 * barPerAtm + 0.5F);
        p2 = (int16_t)(p2 * barPerAtm + 0.5F);
        p3 = (int16_t)(p3 * barPerAtm + 0.5F);
        sp = (uint8_t)(sp * barPerAtm + 0.5F);
    }

    if( (systemStatus.warnings.lowPpO2Sensor[0]   = (p1  <  sp - WARNINGERROR_PERCENT))  &&  !systemStatus.alerts.flags.badPpO2Sensor1 )
        ++lows;
    if( (systemStatus.warnings.highPpO2Sensor[0]  = (p1  >  sp + WARNINGERROR_PERCENT))  &&  !systemStatus.alerts.flags.badPpO2Sensor1 )
        ++highs;
    if( (systemStatus.warnings.lowPpO2Sensor[1]   = (p2  <  sp - WARNINGERROR_PERCENT))  &&  !systemStatus.alerts.flags.badPpO2Sensor2 )
        ++lows;
    if( (systemStatus.warnings.highPpO2Sensor[1]  = (p2  >  sp + WARNINGERROR_PERCENT))  &&  !systemStatus.alerts.flags.badPpO2Sensor2 )
        ++highs;
    if( (systemStatus.warnings.lowPpO2Sensor[2]   = (p3  <  sp - WARNINGERROR_PERCENT))  &&  !systemStatus.alerts.flags.badPpO2Sensor3 )
        ++lows;
    if( (systemStatus.warnings.highPpO2Sensor[2]  = (p3  >  sp + WARNINGERROR_PERCENT))  &&  !systemStatus.alerts.flags.badPpO2Sensor3 )
        ++highs;
    
    if(systemStatus.alerts.flags.unableToControl)
    {
        systemStatus.indicatorStatus = indicatorStatus_controlTransition;
    }
    else
    {
        systemStatus.indicatorStatus = (lows == 1  ||  highs == 1)? indicatorStatus_controlTransition
                                        :  lows  > 0                 ? indicatorStatus_controlLow
                                        :  highs > 0                 ? indicatorStatus_controlHigh
                                        :                              indicatorStatus_controlGood
                                        ;
    }

    if( lows == 1  &&  highs == 1 )
    {
      
    }
    else
    {
      
        systemStatus.alerts.flags.ppO2High = average_ppO2_atm_percent > HIGHLIMIT_ATM_PERCENT;
        systemStatus.alerts.flags.ppO2Low = average_ppO2_atm_percent < LOWLIMIT_ATM_PERCENT;

        if(sp >= average_ppO2_atm_percent)
        {
            errors_ppO2_atm_percent = sp  - average_ppO2_atm_percent;
        }

        onValveTicks = 0;
        if(systemStatus.circuitMode == OPENED_CIRCULT)
            solMode = SOLENOID_MODE_MANUAL;
        
        if( errors_ppO2_atm_percent >= 0)
        {   
            switch( solMode )
            {
                case SOLENOID_MODE_MANUAL:
                    if( average_ppO2_atm_percent >= LOWLIMIT_ATM_PERCENT)
                    {
                        onValveTicks = 0;
                         break;
                    }
                    // fallthrough to Juergensen Mode if avg setpoint is less then lowLimit      
                case SOLENOID_MODE_JUERGENSEN:
                    // See "Control" section of (TODO: update V number for RevD) for formula
                    valveOpen_ticks = factor * errors_ppO2_atm_percent / (depth_decimeters + 100);
                    if( valveOpen_ticks > 0 )
                    {
                        onValveTicks = valveOpen_ticks < MIN_OPENLIMIT_TICKS? MIN_OPEN_TICKS
                            :            valveOpen_ticks > MAX_OPENLIMIT_TICKS? MAX_OPEN_TICKS
                            :                                                  (uint16_t)valveOpen_ticks
                            ;
                    }
                break;
                case SOLENOID_MODE_STANDARD:
                    if(errors_ppO2_atm_percent > 0)
                        onValveTicks = STDMODEOPEN_TICKS;
                    break;
                default:
                    break;
            }
        }
    }
}
void depthAndTemperature_sensor_alarm(uint16_t depth_decimeters, uint8_t temperature)
{   
    static uint8_t sub_state =0;
    systemStatus.depth_decimeters = depth_decimeters;
    systemStatus.temperature_C = temperature;
    if(control_state == SURFACE_CONTROL_STATE)
    {
      if( systemStatus.depth_decimeters >= DIVETHRESH_DECIMETERS)
        {
            
            if( systemStatus.surfaceTime_sec  >=  MIN_SURFACETIME_FOR_NEW_DIVE*60)    
            {
                systemStatus.diveTime_sec        = 0;
                systemStatus.maxDepth_decimeters = systemStatus.depth_decimeters;
                calculate_average_value();
                systemStatus.surfaceTime_sec = 0;
            }
            systemStatus.current_diveTime_sec = 0;          
            control_state = DIVE_CONTROL_STATE;
            sub_state = 0;
        }     
    }
    else
    {
      switch (sub_state)
      {
      case 0 : // controlDescending state
        if( systemStatus.setpoint_ata_percent > 100)
        {
            nominalSetpoint_centiAta = nominalPpO2_atm_percent[CONTROLSEGMENT_START_DESCENT];
        }
        else
        {
            nominalSetpoint_centiAta = systemStatus.setpoint_ata_percent;
        }
        sub_state = 1;
        break;
      case 1:  // controlDescending state
        if( systemStatus.depth_decimeters > DESCENTTHRESH_DECIMETERS)
        {
            sub_state = 2;
        }
        break;
      case 2:  // controlDeep state
        nominalSetpoint_centiAta = systemStatus.setpoint_ata_percent;
        if( systemStatus.depth_decimeters <= ASCENTTHRESH_DECIMETERS)
        {
            sub_state = 3;
        }
        break;
      case 3:  // controlAscending state
       if( systemStatus.setpoint_ata_percent > 100)
        {
            nominalSetpoint_centiAta = nominalPpO2_atm_percent[CONTROLSEGMENT_ASCENDED];
        }
        else
        {
            nominalSetpoint_centiAta = systemStatus.setpoint_ata_percent;
        }
        sub_state = 4;
        break;
      case 4:  // controlDeep state
        nominalSetpoint_centiAta = systemStatus.setpoint_ata_percent;
        if( systemStatus.depth_decimeters >= DESCENTTHRESH_DECIMETERS)
        {
            sub_state = 2;
        }
        
        break;
      }
      // change back to surface mode if depth is below threshold
      if( systemStatus.depth_decimeters < DIVETHRESH_DECIMETERS )
      {

        diveSettings_getSettings(&diveSettings);

        if( systemStatus.maxDepth_decimeters > diveSettings.bsMeter_maxDepth_deciMeters )
        {
            diveSettings.bsMeter_maxDepth_deciMeters = systemStatus.maxDepth_decimeters;
          //  systemStatus.maxDepth_decimeters = systemStatus.lastDiveDepth_decimeter;
        }
        
        diveSettings.bsMeter_totalDiveTime_sec  += systemStatus.current_diveTime_sec;
        diveSettings.stackTimeRemaining_sec      = systemStatus.stackTimeRemaining_sec;
        diveSettings.lastDiveDepth_deciMeters = systemStatus.maxDepth_decimeters;
        diveSettings.lastDiveTime_sec = systemStatus.diveTime_sec + systemStatus.current_diveTime_sec;
        systemStatus.diveTime_sec = systemStatus.diveTime_sec + systemStatus.current_diveTime_sec;
        systemStatus.current_diveTime_sec = 0;
        diveSettings.new_dive_flag = 1;
        
//        diveSettings.number_dives++;
//        diveSettings.average_depth = ((diveSettings.average_depth * (diveSettings.number_dives - 1)) + systemStatus.maxDepth_decimeters) / diveSettings.number_dives;
//        diveSettings.average_dive_time = ((diveSettings.average_dive_time * (diveSettings.number_dives - 1)) + systemStatus.diveTime_sec) / diveSettings.number_dives;
          
        diveSettings_saveSettings(&diveSettings);

        systemStatus.bsMeter_maxDepth_deciMeters = diveSettings.bsMeter_maxDepth_deciMeters;
        systemStatus.bsMeter_totalDiveTime_sec   = diveSettings.bsMeter_totalDiveTime_sec;
       // systemStatus.bsMeter_diveTime_average = diveSettings.average_dive_time;
       // systemStatus.bsMeter_maxDepth_average = diveSettings.average_depth;
        
        control_state = SURFACE_CONTROL_STATE;
        if(systemStatus.diveTime_sec > 300)
        {
          systemStatus.surfaceTime_sec = 0;
        }
      }
//      if( systemStatus.depth_decimeters >= systemStatus.currentDiveDepth_decimeter )
//      {
//         systemStatus.currentDiveDepth_decimeter = systemStatus.depth_decimeters;
//      }
//      
//      if( systemStatus.currentDiveDepth_decimeter >= systemStatus.lastDiveDepth_decimeter)
//      {
//         systemStatus.lastDiveDepth_decimeter = systemStatus.currentDiveDepth_decimeter;
//      }
      if(systemStatus.depth_decimeters >= systemStatus.maxDepth_decimeters)
      {
        systemStatus.maxDepth_decimeters = systemStatus.depth_decimeters;
      }
    }
}

#define OFF GPIO_PIN_SET
#define ON GPIO_PIN_RESET

void control_indicator_led(void)
{    
    if(systemStatus.indicatorStatus == indicatorStatus_controlLow)
    {
      if(blink_led)
      {
        blink_led = 0;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0); //led green off
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 500); // led green on 50%
      }
      else
      {
        blink_led = 1;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0); //led green off
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0); // led green off
      }
    }
    else if(systemStatus.indicatorStatus == indicatorStatus_controlHigh)
    {
      if(blink_led)
      {
        blink_led = 0;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 500); //led green on
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0); // led green onf
      }
      else
      {
        blink_led = 1;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0); //led green off
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0); // led green off
      }
    }
    else if(systemStatus.indicatorStatus == indicatorStatus_controlTransition)
    {
      if(blink_led)
      {
        blink_led = 0;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 500); //led green on
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0); // led green onf
      }
      else
      {
        blink_led = 1;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0); //led green off
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 500); // led green off
      }
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0); //led green on
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0); // led green onf
    }
}


/*1000 ms*/
static void update_tissues(void)
{
   // CompartmentPressures_uBars decoN2CompartmentPressures_uBars;
   // CompartmentPressures_uBars decoHeCompartmentPressures_uBars;
    Deco_SystemData            decoSystemData;

    decoSystemData.currentDiveTime_sec  = systemStatus.time_sec;
    decoSystemData.depth_deciMeters     = systemStatus.depth_decimeters;
    decoSystemData.ppO2_avg_atm_percent = systemStatus.ppO2_avg_atm_percent;
    decoSystemData.gasMixHe_percent     = systemStatus.gasMixHe_percent;
    decoSystemData.gasMixO2_percent     = systemStatus.gasMixO2_percent;
    decoSystemData.gradientHigh_percent = systemStatus.gradientHi;
    decoSystemData.gradientLow_percent  = systemStatus.gradientLo;
    decoSystemData.circuitMode          = systemStatus.circuitMode;

    deco_updateTissues(&decoSystemData); 
    
    if( (systemStatus.time_sec - lastDecoLogWrite_sec) >= 30 ) // 30s
    {
        deco_getCompartmentPressures_uBars(deco_log.decoN2CompartmentPressures_uBars_value, deco_log.decoHeCompartmentPressures_uBars_value);
        deco_log.decoUpdateTime_sec = systemStatus.time_sec;
        //save data
        deco_log.crc_value = vp_crc16(0,(uint8_t*)(&deco_log),sizeof(s_deco_log) - 2);
        deco_log_saveSettings(&deco_log);
        lastDecoLogWrite_sec = systemStatus.time_sec;
    }
}
static void update_timetosurface(void)
{
    Deco_SystemData decoSystemData;
    decoSystemData.currentDiveTime_sec  = systemStatus.time_sec;
    decoSystemData.depth_deciMeters     = systemStatus.depth_decimeters;
    decoSystemData.ppO2_avg_atm_percent = systemStatus.ppO2_avg_atm_percent;
    decoSystemData.gasMixHe_percent     = systemStatus.gasMixHe_percent;
    decoSystemData.gasMixO2_percent     = systemStatus.gasMixO2_percent;
    decoSystemData.gradientHigh_percent = systemStatus.gradientHi;
    decoSystemData.gradientLow_percent  = systemStatus.gradientLo;
    decoSystemData.circuitMode          = systemStatus.circuitMode;

    deco_calculateTimeToSurface(&decoSystemData, &decoInfo);  
}
static void control_in_surface_mode(s_SystemStatus *p_status)
{
    p_status->time_sec++;
    p_status->surfaceTime_sec++;
    
    if( systemStatus.setpoint_ata_percent > 100 )
    {
        nominalSetpoint_centiAta = nominalPpO2_atm_percent[CONTROLSEGMENT_SURFACE];
    }
    else
    {
        nominalSetpoint_centiAta = systemStatus.setpoint_ata_percent;
    }

     if((p_status->surfaceTime_sec) >= MAX_SURFACE_TIME)
     {
        p_status->surfaceTime_sec = MAX_SURFACE_TIME; 
     }
}

static uint32_t get_timestamp(void)
{
    RTC_TimeTypeDef currentTime;
    RTC_DateTypeDef currentDate;
    
    time_t timestamp;
    struct tm currTime;
    
    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);

    currTime.tm_year = currentDate.Year + 100;  // In fact: 2000 + 18 - 1900
    currTime.tm_mday = currentDate.Date;
    currTime.tm_mon  = currentDate.Month - 1;

    currTime.tm_hour = currentTime.Hours;
    currTime.tm_min  = currentTime.Minutes;
    currTime.tm_sec  = currentTime.Seconds;

    timestamp = mktime(&currTime);
    return timestamp;
}

static void control_in_dive_mode(s_SystemStatus *p_status)
{
     p_status->time_sec++;
   //  p_status->diveTime_sec++;
     p_status->current_diveTime_sec++;
   //  p_status->diveTime_sec +=dive_time;
     if((p_status->diveTime_sec + p_status->current_diveTime_sec) == TIME_WRAP_MINUTES * 60)
     {
         p_status->diveTime_sec = 0;
         p_status->current_diveTime_sec = 0;
     }
     if( systemStatus.stackTimeRemaining_sec < 60 )
    {
        systemStatus.alerts.flags.stackTimerExpired = true;
    }
    else
    {
        if((p_status->current_diveTime_sec%60) == 0)
        {
          systemStatus.stackTimeRemaining_sec -= 60;
          systemStatus.alerts.flags.stackTimerExpired = false;
        }
    }
    lastDiveTime_sec = get_timestamp();
    time_saveSettings();
}
void main_control(void)
{
    switch (control_state)
    {
        case SURFACE_CONTROL_STATE:
          control_in_surface_mode(&systemStatus);
//          ui_main.dive_state = 0;
          break;
        case DIVE_CONTROL_STATE:  
          control_in_dive_mode(&systemStatus);
//          ui_main.dive_state = 1;
          break;
            
    }
     updateTissuesPeriod++;
     updateTimeToSurface++;
    if(updateTissuesPeriod >= UPDATE_TISSUES_PERIOD)
    {
        updateTissuesPeriod =0;
        update_tissues();
        
    }
    if(updateTimeToSurface >= UPDATE_TTS_PERIOD)
    {
        updateTimeToSurface =0;
        update_timetosurface();
    }

}
void wait_wet_input(void)
{
   // if(HA)

}
void sd_card_init(void)
{
   // f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
}

uint8_t sd_card_clear(void)
{
  spiffs_file fd = SPIFFS_remove(&fs, "deco_log");
  fd = SPIFFS_remove(&fs, "dive_log");
  fd = SPIFFS_remove(&fs, "time_log");
  return 1;
}
//
uint8_t  diveSettings_saveSettings(DiveSettings *diveSettings)
{
    spiffs_file fd = SPIFFS_open(&fs, "dive_setting", SPIFFS_RDWR,0);
    SPIFFS_write(&fs, fd,(uint8_t *)(diveSettings), sizeof(DiveSettings));
    SPIFFS_close(&fs, fd);
    return 1;
}
uint8_t diveSettings_getSettings(DiveSettings *diveSettings)
{
    spiffs_file fd = SPIFFS_open(&fs, "dive_setting", SPIFFS_RDWR, 0);
    if(fd < 0)
    {
         SPIFFS_close(&fs, fd);
         fd = SPIFFS_open(&fs, "dive_setting", SPIFFS_CREAT| SPIFFS_TRUNC | SPIFFS_RDWR, 0);
         SPIFFS_write(&fs, fd,(uint8_t *)(diveSettings), sizeof(DiveSettings));
         SPIFFS_close(&fs, fd);
         return 1;
    }
    else
    {
        SPIFFS_read(&fs, fd, (uint8_t *)(diveSettings), sizeof(DiveSettings));
        SPIFFS_close(&fs, fd);
    }
    
    return 1;
}

uint8_t  deco_log_saveSettings(s_deco_log *deco)
{
    spiffs_file fd = SPIFFS_open(&fs, "deco_log", SPIFFS_RDWR,0);
    SPIFFS_write(&fs, fd,(uint8_t *)(deco), sizeof(s_deco_log));
    SPIFFS_close(&fs, fd);
    return 1;
    
}

uint8_t deco_log_getSettings(s_deco_log *deco)
{
    spiffs_file fd = SPIFFS_open(&fs, "deco_log", SPIFFS_RDWR, 0);
    if(fd < 0)
    {
       SPIFFS_close(&fs, fd);
       fd = SPIFFS_open(&fs, "deco_log", SPIFFS_CREAT| SPIFFS_TRUNC | SPIFFS_RDWR, 0);
       SPIFFS_write(&fs, fd,(uint8_t *)(deco), sizeof(s_deco_log));
       SPIFFS_close(&fs, fd);
       return 0;
    }
    else
    {
        SPIFFS_read(&fs, fd, (uint8_t *)(deco), sizeof(s_deco_log));
        SPIFFS_close(&fs, fd);
    }
    return 1;
}

uint8_t time_saveSettings(void)
{
    spiffs_file fd = SPIFFS_open(&fs, "time_log", SPIFFS_RDWR,0);
    SPIFFS_write(&fs, fd,(uint8_t *)(&lastDiveTime_sec), sizeof(uint32_t));
    SPIFFS_close(&fs, fd);
    return 1;
}
uint8_t time_getSettings(void)
{
    spiffs_file fd = SPIFFS_open(&fs, "time_log", SPIFFS_RDWR, 0);
    if(fd < 0)
    {
       SPIFFS_close(&fs, fd);
       //fd = SPIFFS_open(&fs, "time_log", SPIFFS_CREAT| SPIFFS_TRUNC | SPIFFS_RDWR, 0);
     // SPIFFS_write(&fs, fd,(uint8_t *)(&lastDiveTime_sec), sizeof(uint32_t));
      // SPIFFS_close(&fs, fd);
       return 0;
    }
    else
    {
        SPIFFS_read(&fs, fd, (uint8_t *)(&lastDiveTime_sec), sizeof(uint32_t));
        SPIFFS_close(&fs, fd);
    }
    return 1;
}


void diveSettings_init(void)
{
    //systemStatus.dipSwitches = ui_readDipSwitches();
    //systemStatus.alerts.flags.unknownDepthAndTemperature = true;

    systemStatus.ppO2_avg_atm_percent = 0;
    systemStatus.time_sec             = get_timestamp();
    

    diveSettings_getSettings(&diveSettings);
//    
//    if(diveSettings.new_dive_flag)
//    {
//        diveSettings.number_dives++;
//        diveSettings.average_depth = ((diveSettings.average_depth * (diveSettings.number_dives - 1)) + diveSettings.lastDiveDepth_deciMeters) / diveSettings.number_dives;
//        diveSettings.average_dive_time = ((diveSettings.average_dive_time * (diveSettings.number_dives - 1)) + diveSettings.lastDiveTime_sec) / diveSettings.number_dives;
//        diveSettings.new_dive_flag = 0;
//    }    

    systemStatus.stackTimeRemaining_sec      = diveSettings.stackTimeRemaining_sec;
    systemStatus.maxDepth_decimeters         = diveSettings.bsMeter_maxDepth_deciMeters;
    systemStatus.bsMeter_maxDepth_deciMeters = diveSettings.bsMeter_maxDepth_deciMeters;
    systemStatus.bsMeter_totalDiveTime_sec   = diveSettings.bsMeter_totalDiveTime_sec;
    systemStatus.bsMeter_diveTime_average = diveSettings.average_dive_time;
    systemStatus.bsMeter_maxDepth_average = diveSettings.average_depth;
     
    diveSettings_saveSettings(&diveSettings);
    
    if(time_getSettings() == 0)
    {
        time_saveSettings();
    
    }
    else
    {
        if(lastDiveTime_sec > 0 && lastDiveTime_sec <= systemStatus.time_sec )
        {
            systemStatus.surfaceTime_sec = systemStatus.time_sec - lastDiveTime_sec;
        }
        else
        {
            systemStatus.surfaceTime_sec = 0;
        }
        if(systemStatus.surfaceTime_sec >= MAX_SURFACE_TIME)
        {
            systemStatus.surfaceTime_sec  = MAX_SURFACE_TIME;
        }
    }
    if(deco_log_getSettings(&deco_log) == 1)
    {
        if(deco_log.crc_value = vp_crc16(0,(uint8_t*)(&deco_log),sizeof(s_deco_log) -2))
        {
            Deco_SystemData decoSystemData;
            deco_init(deco_log.decoN2CompartmentPressures_uBars_value, deco_log.decoHeCompartmentPressures_uBars_value, deco_log.decoUpdateTime_sec);
             // update tissues (assume depth of 0 at 21% air for time duration
            decoSystemData.currentDiveTime_sec  = systemStatus.time_sec;
            decoSystemData.depth_deciMeters     = 0;
            decoSystemData.ppO2_avg_atm_percent = 100;
            decoSystemData.gasMixHe_percent     = 0;
            decoSystemData.gasMixO2_percent     = 21;
            decoSystemData.gradientHigh_percent = config_data.user_setting.gradientHigh;
            decoSystemData.gradientLow_percent  = config_data.user_setting.gradientLow;
            decoSystemData.circuitMode          = 1;

            deco_updateTissues(&decoSystemData);

            decoSystemData.circuitMode          = 0;
            deco_calculateTimeToSurface(&decoSystemData, &decoInfo);
        }
        else
        {
            deco_init(NULL, NULL, systemStatus.time_sec);
        }
    }
    else
    {
     deco_init(NULL, NULL, systemStatus.time_sec);
    }
}
void reseting_stack_timer(void)
{
    diveSettings_getSettings(&diveSettings);

    if( systemStatus.depth_decimeters >= DIVETHRESH_DECIMETERS )
    {
        diveSettings.stackTimeRemaining_sec = systemStatus.stackTimeRemaining_sec + 600;
    }
    else
    {
        diveSettings.stackTimeRemaining_sec = config_data.user_setting.stackTime * 60;
    }

    systemStatus.stackTimeRemaining_sec = diveSettings.stackTimeRemaining_sec;

    diveSettings_saveSettings(&diveSettings);

    if( systemStatus.stackTimeRemaining_sec > 0 )
    {
        systemStatus.alerts.flags.stackTimerExpired = false;
    }
}

uint16_t get_reset_stack_time_value(void)
{
    uint16_t ret = 0;
    if( systemStatus.depth_decimeters >= DIVETHRESH_DECIMETERS )
    {
        ret = systemStatus.stackTimeRemaining_sec + 600;
    }
    else
    {
        ret = config_data.user_setting.stackTime * 60;
    }

    return ret/60;
}