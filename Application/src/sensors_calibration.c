#include "sensors_calibration.h"
#include "flash_process.h"

bool ok1, ok2, ok3;
//sensor_calib cal;


int32_t ppO2_loLimit_counts;
int32_t ppO2_hiLimit_counts;

static bool check_sensor(uint16_t ave,uint16_t channel, int32_t loLimit, int32_t hiLimit, int16_t nominal)
{
    // m = y / (x - x0)

    int32_t temp = ave - config_data.cal.data[channel].offset;

    if( temp < loLimit  ||  temp > hiLimit )
        return false;

    if( temp <= 0 )
        return false;  // reject negative or infinite slope

   // config_data.cal.data[channel].slope = (float)nominal / (ave - config_data.cal.data[channel].offset);

    return true;
}

static bool calibrateSlope(uint16_t ave,uint16_t channel, int32_t loLimit, int32_t hiLimit, int16_t nominal)
{
    // m = y / (x - x0)

    //int32_t temp = ave - config_data.cal.data[channel].offset;

//    if( temp < loLimit  ||  temp > hiLimit )
//        return false;
//
//    if( temp <= 0 )
//        return false;  // reject negative or infinite slope

    config_data.cal.data[channel].slope = (float)nominal / (ave - config_data.cal.data[channel].offset);

    return true;
}


bool sensors_stdCalibratePpO2Sensors(uint16_t *buff)
{
    
    //count_per_mVolt = ((float)adc_value_avg[6]/1200.00);
    ppO2_loLimit_counts = (int32_t)( 400.0 * count_per_mVolt);
    ppO2_hiLimit_counts = (int32_t)(1500.0 * count_per_mVolt);
    
    ok1 = check_sensor(buff[0], 0,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
    ok2 = check_sensor(buff[1], 1,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
    ok3 = check_sensor(buff[2], 2,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);

    if(ok1 && ok2 && ok3)
    {
       ok1 = calibrateSlope(buff[0], 0,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
       ok2 = calibrateSlope(buff[1], 1,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
       ok3 = calibrateSlope(buff[2], 2,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
       
       config_data.cal.data[0].scaleFactor = 1.0;
       config_data.cal.data[1].scaleFactor = 1.0;
       config_data.cal.data[2].scaleFactor = 1.0;
       return ok1 && ok2 && ok3;
    }
    return false;
}

bool sensors_altCalibratePpO2Sensors(uint16_t *buff,uint16_t alt_mBar, uint16_t surfacePressure_mbar)
{
   //bool ok1, ok2, ok3;
    
   // count_per_mVolt = ((float)adc_value_avg[6]/1200.00);
    ppO2_loLimit_counts = (int32_t)( 40.0 * count_per_mVolt);
    ppO2_hiLimit_counts = (int32_t)(150.0 * count_per_mVolt);

    ok1 = check_sensor(buff[0], 0,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
    ok2 = check_sensor(buff[1], 1,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
    ok3 = check_sensor(buff[2], 2,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
    
    if(ok1 && ok2 && ok3)
    {
        ok1 = calibrateSlope(buff[0], 0,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
        ok2 = calibrateSlope(buff[1], 1,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);
        ok3 = calibrateSlope(buff[2], 2,ppO2_loLimit_counts, ppO2_hiLimit_counts, 100);

        config_data.cal.data[0].scaleFactor = ((float)alt_mBar / (float)surfacePressure_mbar);
        config_data.cal.data[1].scaleFactor = ((float)alt_mBar / (float)surfacePressure_mbar);
        config_data.cal.data[2].scaleFactor = ((float)alt_mBar / (float)surfacePressure_mbar);
        return ok1 && ok2 && ok3;
    }
     return false;    
}

int16_t sensor_calibrated(uint8_t channel, int32_t raw)
{
    float f = 0;
    if( raw >= MIN_ADC_COUNTS )
        f = config_data.cal.data[channel].slope * config_data.cal.data[channel].scaleFactor * (raw  -  config_data.cal.data[channel].offset);
    return f >= 0
        ? f > INT16_MAX? INT16_MAX: (int16_t)(f + 0.5)
        : f < INT16_MIN? INT16_MIN: (int16_t)(f - 0.5)
        ;
}

void sensors_init(void)
{
   // memcpy(config_data.cal.data, defaultCalData, sizeof(config_data.cal.data));
}


//static bool storeCal(Calibration* cal)
//{
//    REQUIRE(cal);
//
//    cal->crc = vp_crc16(0, &cal->data, sizeof cal->data);
//
//    return(vpFlash_write(mcuFlashId, flashAddress_calData, cal, sizeof(*cal)) == sizeof(*cal));
//}
//
//
//

//}
//
//
//
//bool sensors_init(bool isPrimary)
//{
//    bool ok = false;
//
//    vpFlash_init();
//    vpAdc_init();
//
//    if( isPrimary )
//    {
//        #ifdef SENSORS_PLAT_INIT
//            SENSORS_PLAT_INIT();
//        #endif
//    }
//
//    ok = vpFlash_read(mcuFlashId , &cal, flashAddress_calData, sizeof cal) == sizeof cal;
//
//    if( ok )
//    {
//        ok = vp_crc16(0, &config_data.cal.data, sizeof cal.data) == cal.crc;
//        if( ok )
//            xVP_TRACE(("Read cal data from flash Succeeded.\n\r"));
//    }
//
//    if( !ok )
//    {
//        memcpy(cal.data, defaultCalData, sizeof cal.data);
//        xVP_TRACE(("Using default cal data\n\r"));
//    }
//
//    return ok;
//}
//
//
//
//static const struct
//{
//    VPAdc_Channel p;
//    VPAdc_Channel n;
//} chanMap[systemStatus_numAdcChannels] =
//{
//    { 13, vpAdc_common },  //ppO2_1_atm
//    { 12, vpAdc_common },  //ppO2_2_atm
//    { 11, vpAdc_common },  //ppO2_3_atm
//    #ifdef ENABLE_HP
//        {  9,            8 },  //highPressureO2
//        { 15,           14 },  //highPressureDiluent
//    #endif
//    #ifdef ENABLE_READ_WATER
//        { 10, vpAdc_common },  // water conductivity
//    #endif
//    {  4, vpAdc_common },  // batteryV 1.5V
//    {  7, vpAdc_common },  // main 5V
//    {  6, vpAdc_common },  // ext 3.3V
//    {  5, vpAdc_common },  // int 3.3V
//    {  3, vpAdc_common },  // Solenoid Current
//    {  1,            0 },  // Battery Current (sampled over 0.1 ohm res)
//};
//
//
//
//int32_t sensors_readAvgRawValues_microVolt(uint8_t channel)
//{
//    return (int32_t)((float)ave[channel] * rawAdcScaleFactor[channel] / countsPerMicrovolt);
//}
//
//
//
//int16_t sensors_readAdc(uint8_t nextChannel)
//{
//    static uint8_t currentChannel = systemStatus_numAdcChannels; /* invalid initially */
//
//    #ifdef ENABLE_HP
//        if( nextChannel == systemStatus_highPressureO2_psi  ||  nextChannel == systemStatus_highPressureDiluent_psi )
//        {
//            vpDio_write(highPressureTestPin, true);
//        }
//    #endif
//
//    if( nextChannel >= systemStatus_batteryV_decivolts  &&  nextChannel < systemStatus_current_mA )
//    {
//        vpDio_write(batteryTestPin, true);
//    }
//
//    int32_t raw    = vpAdc_read_linked(chanMap[nextChannel].p, chanMap[nextChannel].n);
//    int16_t cooked = 0;
//
//    #ifdef ENABLE_HP
//        if( !(nextChannel == systemStatus_highPressureO2_psi  ||  nextChannel == systemStatus_highPressureDiluent_psi) )
//        {
//            vpDio_write(highPressureTestPin, false);
//        }
//    #endif
//
//    if( !(nextChannel >= systemStatus_batteryV_decivolts  &&  nextChannel < systemStatus_current_mA) )
//    {
//        vpDio_write(batteryTestPin, false);
//    }
//
//    if( currentChannel < systemStatus_numAdcChannels )
//    {
//        update(currentChannel, raw);
//
//        cooked = calibrated(currentChannel,
//                            currentChannel <= systemStatus_ppO2_3_atm_percent
//                                ? hist[currentChannel][histSize - 1]  // most recent ppO2 value (not average)
//                                : ave[currentChannel]
//                           );
//    }
//
//    currentChannel = nextChannel;
//
//    return cooked;
//}
//
//
//
//bool sensors_calibrateBatterySensor(void)
//{
//    enum
//    {
//        nominal = 36  //decivolts
//    };
//
//    const int32_t loLimit = (int32_t)(( 18.0/*decivolts*/ * 356.69/*mV*/ * countsPerMillivolt) / 36.0/*decivolts*/);
//    const int32_t hiLimit = (int32_t)((105.0/*decivolts*/ * 356.69/*mV*/ * countsPerMillivolt) / 36.0/*decivolts*/);
//
//    return calibrateSlope(systemStatus_batteryV_decivolts, loLimit, hiLimit, nominal)
//        && storeCal(&cal)
//        ;
//}
//
//
//
//#ifdef ENABLE_HP
//    bool sensors_zeroHighPressureSensors(void)
//    {
//        cal.data[systemStatus_highPressureO2_psi     ].offset = ave[systemStatus_highPressureO2_psi     ];
//        cal.data[systemStatus_highPressureDiluent_psi].offset = ave[systemStatus_highPressureDiluent_psi];
//
//        return storeCal(&cal);
//    }
//
//
//
//    bool sensors_calibrateHighPressureSensors(void)
//    {
//        enum
//        {
//            nominal = 3000  //psi
//        };
//
//        const int32_t loLimit = (int32_t)(10.0/*mV*/ * countsPerMillivolt);
//        const int32_t hiLimit = (int32_t)(80.0/*mV*/ * countsPerMillivolt);
//
//        bool ok1, ok2;
//
//        ok1 = calibrateSlope(systemStatus_highPressureO2_psi,      loLimit, hiLimit, nominal);
//        ok2 = calibrateSlope(systemStatus_highPressureDiluent_psi, loLimit, hiLimit, nominal);
//
//        return ok1 && ok2 && storeCal(&cal);
//    }
//#endif
//
//
//static const int32_t ppO2_loLimit_counts = (int32_t)( 40.0/*mV*/ * countsPerMillivolt);
//static const int32_t ppO2_hiLimit_counts = (int32_t)(150.0/*mV*/ * countsPerMillivolt);
//
//bool sensors_stdCalibratePpO2Sensors(void)
//{
//    enum
//    {
//        nominal = 100  //atm_percent
//    };
//
//    // TODO: allow ranges to be set for different o2 sensors
//    // sensors are linear within range
//
//    bool ok1, ok2, ok3;
//
//    cal.data[systemStatus_ppO2_1_atm_percent].scaleFactor = 1.0;
//    cal.data[systemStatus_ppO2_2_atm_percent].scaleFactor = 1.0;
//    cal.data[systemStatus_ppO2_3_atm_percent].scaleFactor = 1.0;
//
//    ok1 = calibrateSlope(systemStatus_ppO2_1_atm_percent, ppO2_loLimit_counts, ppO2_hiLimit_counts, nominal);
//    ok2 = calibrateSlope(systemStatus_ppO2_2_atm_percent, ppO2_loLimit_counts, ppO2_hiLimit_counts, nominal);
//    ok3 = calibrateSlope(systemStatus_ppO2_3_atm_percent, ppO2_loLimit_counts, ppO2_hiLimit_counts, nominal);
//
//    return ok1 && ok2 && ok3 && storeCal(&cal);
//}
//
//
//
//bool sensors_altCalibratePpO2Sensors(uint16_t alt_mBar, uint16_t surfacePressure_mbar)
//{
//    enum
//    {
//        nominal = 100  //atm_percent
//    };
//
//    // TODO: allow ranges to be set for different o2 sensors
//    // sensors are linear within range
//
//    uint16_t surfacePressure_mBar;
//    uint16_t seaLevelPressure_mBar;
//    int32_t  lowLimit_counts;
//    // min sensor value is 40 mV at sea level (1013 mBar)
//    // the higher the altitude, the lower the mV reading is in 100% O2
//
//    if( sensors_readSurfacePressure(&surfacePressure_mbar, &seaLevelPressure_mBar) )
//    {
//        bool ok1, ok2, ok3;
//
//        lowLimit_counts = ppO2_loLimit_counts;
//    
//        cal.data[systemStatus_ppO2_1_atm_percent].scaleFactor = ((float)alt_mBar / (float)surfacePressure_mbar);
//        cal.data[systemStatus_ppO2_2_atm_percent].scaleFactor = ((float)alt_mBar / (float)surfacePressure_mbar);
//        cal.data[systemStatus_ppO2_3_atm_percent].scaleFactor = ((float)alt_mBar / (float)surfacePressure_mbar);
//    
//        ok1 = calibrateSlope(systemStatus_ppO2_1_atm_percent, lowLimit_counts, ppO2_hiLimit_counts, nominal);
//        ok2 = calibrateSlope(systemStatus_ppO2_2_atm_percent, lowLimit_counts, ppO2_hiLimit_counts, nominal);
//        ok3 = calibrateSlope(systemStatus_ppO2_3_atm_percent, lowLimit_counts, ppO2_hiLimit_counts, nominal);
//    
//        return ok1 && ok2 && ok3 && storeCal(&cal);
//    }
//    else
//    {
//        return false;
//    }
//}
//
//
//
//bool sensors_ppO2SensorsAgree(int16_t a, int16_t b, int16_t sp)
//{
//    enum { limitDivisor = 5 }; // 20%
//
//    int16_t d = a > b? a - b: b - a;
//
//    int16_t ref = (a + b + 1) / 2  >=  sp
//        ? (a < b? a: b)  // smaller of a and b
//        : (a > b? a: b)  // larger of a and b
//        ;
//
//    return d  <  ref / limitDivisor;
//}



// EOF
