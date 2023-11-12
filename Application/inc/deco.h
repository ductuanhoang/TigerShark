#ifndef _DECO_H_
#define _DECO_H_

#include "main.h"
#include <stdbool.h>
#include <math.h>

#if defined __cplusplus
    extern "C"
    {
#endif



    enum
    {
        deco_updateTissuesPeriod_ms = 10 * 1000,
        deco_updateTimeToSurface_ms = 30 * 1000,
    };

    typedef struct Compartment_
    {
        float pressure_bars;
        float ttsPressure_bars;  // for Time To Surface Calculation
    } Compartment;

    typedef struct MValues_t
    {
        float halfTime_min;
        float m0Value;
        float deltaM;
    } MValues;

    enum
    {
        deco_numberOfCompartments = 16,
    };

    typedef uint32_t CompartmentPressures_uBars[deco_numberOfCompartments];

    typedef enum GasType
    {
        gasType_nitrogen,
        gasType_helium,
    } GasType;

    typedef struct
    {
        uint64_t  currentDiveTime_sec;
        uint16_t  depth_deciMeters;
        uint16_t  ppO2_avg_atm_percent;
        uint8_t   gasMixHe_percent;
        uint8_t   gasMixO2_percent;
        uint8_t   gradientHigh_percent;
        uint8_t   gradientLow_percent;
        uint8_t   circuitMode;
    } Deco_SystemData;

    typedef struct
    {
        uint16_t timeToSurface_min;
        uint16_t firstStop_meters;
        uint16_t firstTime_min;
        uint16_t maxMinDepth_meters;

    } SystemStatus_Deco_t;

    void deco_init                         (  CompartmentPressures_uBars decoN2CompartmentPressures_uBars
                                            , CompartmentPressures_uBars decoHeCompartmentPressures_uBars
                                            , uint64_t  lastUpdateTime_sec_
                                           );
        // initialize deco compartments with passed in data (unless NULL, then initialized at sea level)
        // call before any other deco_ function

    bool deco_existingDependecy            (void);

    void deco_calculateTimeToSurface       (Deco_SystemData* decoSystemData, SystemStatus_Deco_t* decoInfo);

    void deco_updateTissues                (const Deco_SystemData* decoSystemData);

    void deco_getCompartmentPressures_uBars(  CompartmentPressures_uBars decoN2CompartmentPressures_uBbars
                                            , CompartmentPressures_uBars decoHeCompartmentPressures_uBars
                                           );



    #if defined __cplusplus
        } //extern "C"
    #endif



#endif
