#ifndef _MAIN_PROCESS_H_
#define _MAIN_PROCESS_H_

#include "main.h"
#include "deco.h"
#include "gpio.h"
#include <stdbool.h>


#define   NUMBER_ADC_SENSOR         3
#define   NUMBER_O2_SENSOR          3
#define   CONTROLLER_MODE_PRIMARY   1

#define LOWEST_ATM_PERCENT              10
#define LOWLIMIT_ATM_PERCENT            18
#define HIGHLIMIT_ATM_PERCENT           180

#define MIN_LOW_PPO2_ATM_PERCENT        10
#define MAX_HIGH_PPO2_ATM_PERCENT       180
#define CONTROLSEGMENT_SURFACE          0
#define CONTROLSEGMENT_START_DESCENT    1
#define CONTROLSEGMENT_ASCENDED         2
#define TIME_WRAP_MINUTES               1000
#define UPDATE_TISSUES_PERIOD           10
#define UPDATE_TTS_PERIOD               30
#define DIVETHRESH_DECIMETERS           10
#define DESCENTTHRESH_DECIMETERS        30
#define ASCENTTHRESH_DECIMETERS         35
#define MIN_SURFACETIME_FOR_NEW_DIVE    5
#define MIN_OPENLIMIT_TICKS             1
#define MAX_OPENLIMIT_TICKS             28
#define MIN_OPEN_TICKS                  1
#define MAX_OPEN_TICKS                  30
#define STDMODEOPEN_TICKS               10
#define WARNINGERROR_PERCENT            15


typedef struct
{
    bool badPpO2Sensor1;
    bool badPpO2Sensor2;
    bool badPpO2Sensor3;
    bool badPpO2All;
}s_alarm_status;

typedef enum 
{
    SOLENOID_MODE_MANUAL = 0,
    SOLENOID_MODE_JUERGENSEN,
    SOLENOID_MODE_STANDARD,
} e_SolenoidMode;

typedef enum 
{
    IN_SURFACE_STATE = 0,
    IN_DIVE_STATE,
    IN_DESCENDING_STATE,
    IN_DEEP_STATE,
    IN_ASCENDING_STATE  
} e_diving_state;
typedef enum 
{
    SURFACE_CONTROL_STATE = 0,
    DIVE_CONTROL_STATE,
} e_control_state;
typedef enum 
{
    CLOSED_CIRCULT = 0,
    OPENED_CIRCULT
} e_circuit_mode;

typedef struct
{
    uint32_t badPpO2Sensor1             : 1;
    uint32_t badPpO2Sensor2             : 1;
    uint32_t badPpO2Sensor3             : 1;
    uint32_t ppO2Low                    : 1;
    uint32_t ppO2High                   : 1;
    uint32_t lowPrimaryBattery          : 1;
    uint32_t lowSecondaryBattery        : 1;
    uint32_t lowHighPressureO2          : 1;
    uint32_t lowDiluentPressure         : 1;
    uint32_t priSecDisagreeSensor1      : 1;
    uint32_t priSecDisagreeSensor2      : 1;
    uint32_t priSecDisagreeSensor3      : 1;
    uint32_t unableToControl            : 1;
    uint32_t unknownDepthAndTemperature : 1;
    uint32_t noDataFromOtherController  : 1;
    uint32_t tooManyLogFailures         : 1;
    uint32_t tooManyLogFailures_2       : 1;
    uint32_t ascendingTooFast           : 1;
    uint32_t stackTimerExpired          : 1;
    uint32_t padding                    : 13;
}   SystemStatus_AlertFlags;

typedef union
{
    uint32_t                all;
    SystemStatus_AlertFlags flags;
} SystemStatus_Alerts;

typedef struct
{
    uint8_t lowPpO2Sensor[3];
    uint8_t highPpO2Sensor[3];
} SystemStatus_Warnings;

typedef enum 
{
    indicatorStatus_controlGood = 0,             //  ppO2 good
    indicatorStatus_controlTransition ,        //  "transition" or unable to control due to no 2 good sensors
    indicatorStatus_controlLow,                 //  ppO2 excessively low           
    indicatorStatus_controlHigh,                //  ppO2 excessively high 
}IndicatorStatus;

typedef struct
{
    uint8_t                     controllerMode;

    bool                        valveOpen;

    SystemStatus_Alerts         alerts;
    SystemStatus_Warnings       warnings;
//    UserSettings_Settings       settings;
//    DisplayStatus               displayStatus;
    IndicatorStatus             indicatorStatus;

    uint16_t                    ppO2_atm_percent[NUMBER_ADC_SENSOR];
    uint16_t                    ppO2_avg_sensor[NUMBER_ADC_SENSOR];
    uint32_t                    ppO2Values_uVolt[NUMBER_O2_SENSOR];

    uint32_t                    lastGoodDepthReadingTime_sec;
    uint32_t                    surfaceTime_sec;
    uint16_t                    diveTime_sec;
    uint16_t                    current_diveTime_sec;
    uint32_t                    time_sec;

    uint8_t                     dipSwitches;    // 1-bits mean "on"
    uint8_t                     buttonStatus;    // 1-bits mean "down"

    int8_t                      temperature_C;
    uint16_t                    surfacePressure_mBar;
    uint16_t                    rawPressure_mBar;
    uint16_t                    ppO2_avg_atm_percent;

    uint16_t                    depth_decimeters;
    uint16_t                    lastDiveDepth_decimeter;
    uint16_t                    currentDiveDepth_decimeter;
    uint16_t                    maxDepth_decimeters;

    uint8_t                     circuitMode;
    uint8_t                     solMode;
    uint8_t                     setpoint_ata_percent;
    uint8_t                     gasMixO2_percent;
    uint8_t                     gasMixHe_percent;
    uint8_t                     gradientLo;
    uint8_t                     gradientHi;
    
    uint16_t                    stackTimeRemaining_sec;

    uint16_t                    bsMeter_maxDepth_deciMeters;
    uint32_t                    bsMeter_totalDiveTime_sec;
    uint32_t                    bsMeter_maxDepth_average;
    uint32_t                    bsMeter_diveTime_average;

    uint16_t                    deco_timeToSurface_min;
    uint16_t                    deco_firstStop_meters;
    uint16_t                    deco_firstTime_min;
    bool                        deco_hasDependency;
} s_SystemStatus;

typedef struct
{
    uint32_t units             : 1; // 0 = FSW,                   1 = MSW
    uint32_t ppO2units         : 1; // 0 = atm_percent,           1 = bars
    uint32_t divaEnabled       : 1; // 0 = divaDisabled,          1 = divaEnabled
    uint32_t divaMode          : 1; // 0 = ppO2 Mode,             1 = Setpoint Mode
    uint32_t signalOptShow     : 1; // 0 = hide signal opt,       1 = show signal opt
    uint32_t flashlightOptShow : 1; // 0 = hide flashlight opt,   1 = show flashlight opt
    uint32_t enableBuddyLeds   : 1; // 0 = buddy leds disabled,   1 = buddy leds enabled
    uint32_t enableSignalLeds  : 1; // 0 = signal leds off,       1 = signal leds on
    uint32_t enableFlashlight  : 1; // 0 = flashlight off,        1 = flashlight on
    uint32_t enableDecoLeds    : 1; // 0 = divaDecoLeds off,      1 = divaDecoLedsOn

    uint32_t showDeco          : 1; // 0 = Don't Show,            1 = Show Deco Info
    uint32_t decoMode          : 1; // 0 = Deco Off,              1 = Deco Mode On
    
    uint32_t divaSetpointBlinkRepeat_sec : 3;
    uint32_t screenRotation    : 2; // 0 = Landscape Mode,        1 = Portrait Mode
    uint32_t darkmode          : 1; // 0 = Landscape Mode,        1 = Portrait Mode
    uint32_t padding           : 14; 

    // add future persistant 1 bit settings here (16 settings max)

}   UserSettings_SettingFlags;

typedef union
{
    uint32_t                    all;
    UserSettings_SettingFlags   flags;
} UserSettings_Settings;

typedef struct
{
    uint8_t               controllerMode;

    uint8_t               setPoint_centiAta;
    uint8_t               gasMixO2_percent;
    uint8_t               gasMixHe_percent;
    uint8_t               gradientHigh;
    uint8_t               gradientLow;

    uint8_t               circuitMode;
    uint8_t               solMode;

    uint8_t               autoShutdownTime_min;     // 2 - 30 min
    uint8_t               batteryWarning_deciVolt;  // when batt reaches this voltage, warn user (1.2 - ??)

    uint8_t               diveLedBrightness_percent;
    uint8_t               decoLedBrightness_percent;
    uint8_t               flashlightBrightness_percent;
    
    uint16_t              flashlightTimeout_sec;
    uint16_t              divaSigLedTimeout_sec;
    
    uint8_t               backlightTimeout_sec;
    uint16_t              stackTime;
    uint32_t              newDeviceTime_sec;

    UserSettings_Settings settings;                // (32 bit) see Userscreen_t struct for info

} s_ControlSettings;



typedef struct 
{
    uint16_t stackTimeRemaining_sec;
    uint16_t lastDiveMaxDepth_deciMeters;
    uint16_t lastDiveDepth_deciMeters;
    uint16_t bsMeter_maxDepth_deciMeters;
    uint32_t bsMeter_totalDiveTime_sec;
    uint32_t lastDiveTime_sec;
    uint32_t number_dives;
    uint32_t average_depth;
    uint32_t average_dive_time;
    uint16_t new_dive_flag;
    uint16_t crc_value;
}DiveSettings;

typedef struct
{
      uint32_t decoN2CompartmentPressures_uBars_value[16];
      uint32_t decoHeCompartmentPressures_uBars_value[16];
      uint32_t decoUpdateTime_sec;
      uint16_t crc_value;
      uint16_t unused;
}s_deco_log;


typedef struct
{
    DiveSettings diveSettings;
    uint16_t         crc;
} DiveSettingsInfo;


extern s_SystemStatus systemStatus;


#define VALVE_CONTROL_ON() HAL_GPIO_WritePin(VALVE_PW_EN_GPIO_Port, VALVE_PW_EN_Pin, GPIO_PIN_SET);\
                           HAL_GPIO_WritePin(VALVE_CTRL_GPIO_Port, VALVE_CTRL_Pin, GPIO_PIN_SET);

                          
#define VALVE_CONTROL_OFF() HAL_GPIO_WritePin(VALVE_CTRL_GPIO_Port, VALVE_CTRL_Pin, GPIO_PIN_RESET);

    

void sensor_control_alarm(uint16_t *ppO2_atm_percent,uint16_t depth_decimeters,e_SolenoidMode solMode);
void depthAndTemperature_sensor_alarm(uint16_t depth_decimeters, uint8_t temperature);
void dim_backlight_lcd(void);
void control_updateValve(void);
void main_control(void);
uint8_t diveSettings_saveSettings(DiveSettings *diveSettings);
uint8_t diveSettings_getSettings(DiveSettings *diveSettings);
uint8_t deco_log_getSettings(s_deco_log *deco);
uint8_t  deco_log_saveSettings(s_deco_log *deco);
uint8_t time_saveSettings(void);
uint8_t time_getSettings(void);
void diveSettings_init(void);
void sd_card_init(void);
void reseting_stack_timer(void);
uint16_t get_reset_stack_time_value(void);
void control_indicator_led(void);
uint8_t sd_card_clear(void);


#endif
