// deco.c
//
// Implements the Buhlmann deco calculations
//
// Copyright 2011 VPI Engineering
// All rights are reserved.

#include "deco.h"
#include "main_process.h"

#define ASCEND_RATE         (-10.0f)       // meters per minute assumed rate m/min * .1 bar/m
#define LOG2                (0.693147181f)
#define EMPTY_N2_COMP_VALUE (1.0f)
#define EMPTY_He_COMP_VALUE (1.0f)



static bool        init;
static Compartment compartmentN2[deco_numberOfCompartments];
static Compartment compartmentHe[deco_numberOfCompartments];



// "C" coefficients for buhlmann ZH-L16 for Nitrogen
static const MValues mValueN2[deco_numberOfCompartments] =
{
    {   5.0f, 29.6f, 1.7928f },
    {   8.0f, 25.4f, 1.5352f },
    {  12.5f, 22.5f, 1.3847f },
    {  18.5f, 20.3f, 1.2780f },
    {  27.0f, 18.5f, 1.2306f },
    {  38.3f, 16.9f, 1.1857f },
    {  54.3f, 15.9f, 1.1504f },
    {  77.0f, 15.2f, 1.1223f },
    { 109.0f, 14.7f, 1.0999f },
    { 146.0f, 14.3f, 1.0844f },
    { 187.0f, 14.0f, 1.0731f },
    { 239.0f, 13.7f, 1.0635f },
    { 305.0f, 13.4f, 1.0552f },
    { 390.0f, 13.1f, 1.0478f },
    { 498.0f, 12.9f, 1.0414f },
    { 635.0f, 12.7f, 1.0359f }
};

// "C" coefficients for buhlmann ZH-L16 for helium
static const MValues mValueHe[deco_numberOfCompartments] =
{
    {   1.88f, 37.2f, 2.0964f },
    {   3.02f, 31.2f, 1.7400f },
    {   4.72f, 27.2f, 1.5321f },
    {   6.99f, 24.3f, 1.3845f },
    {  10.21f, 22.4f, 1.3189f },
    {  14.48f, 20.8f, 1.2568f },
    {  20.53f, 19.4f, 1.2079f },
    {  29.11f, 18.2f, 1.1692f },
    {  41.20f, 17.4f, 1.1419f },
    {  55.19f, 16.8f, 1.1232f },
    {  70.69f, 16.4f, 1.1115f },
    {  90.34f, 16.2f, 1.1022f },
    { 115.29f, 16.1f, 1.0963f },
    { 147.42f, 16.1f, 1.0904f },
    { 188.24f, 16.0f, 1.0850f },
    { 240.03f, 15.9f, 1.0791f }
};



#define METERS_SW_TO_BAR(depth_meters)          ((float)depth_meters * 0.1005f + 1.0f)
#define BAR_METERS_SW   (ambientPressure_bars)  (((float)ambientPressure_bars - 1.0f) * 9.9506)

// P(atm-seaLevel) + depth * conversionFactor
#define DECIMETERS_SW_TO_ATM(depth_deciMeters) (1.0f + (float)depth_deciMeters * 0.009921f)



bool invariants(void)
{
    return init;
}



static void calculateCircutData(  const Deco_SystemData* decoSystemData
                                , float*                 fractionN2
                                , float*                 fractionHe
                               )
{
    float pressure_atm;
    float fractionO2;
    float gasMixN2;
    float ratioHeN2;
    float fractionSum;

    pressure_atm = DECIMETERS_SW_TO_ATM(decoSystemData->depth_deciMeters);

    fractionO2   = ((float)decoSystemData->ppO2_avg_atm_percent / 100.0f) / pressure_atm;

    switch( decoSystemData->circuitMode )
    {
        case CLOSED_CIRCULT:
            gasMixN2 = 1.0f
                     - ((float)decoSystemData->gasMixHe_percent / 100.0f)
                     - ((float)decoSystemData->gasMixO2_percent / 100.0f)
                     ;

            ratioHeN2 =  ((float)decoSystemData->gasMixHe_percent / 100.0f)
                      / (((float)decoSystemData->gasMixHe_percent / 100.0f) + gasMixN2)
                      ;

            fractionSum = 1.0f - fractionO2;

            *fractionHe = fractionSum * ratioHeN2;
            *fractionN2 = fractionSum - *fractionHe;
            break;

        case OPENED_CIRCULT:
            *fractionN2 = 1.0f
                        - (float)decoSystemData->gasMixHe_percent / 100.0f
                        - (float)decoSystemData->gasMixO2_percent / 100.0f
                        ;

            *fractionHe = (float)decoSystemData->gasMixHe_percent / 100.0f;
            break;

        default:
         //   ASSERT(false);
          break;
    }

}



static float calculateAlveolarPressure(float ambientPressure_bars, float gasFraction)
{
    // Palv=[Pamb - PH2O + (1 - Rq)/Rq * PCO2  ] * f
    // Palv 	partial pressure of the inert gas in the alveoli [Bar]
    // Pamb 	ambient pressure, i.e. absolute pressure of breathing mix [Bar]
    // PH2O 	water vapor partial pressure, at 37 degrees of Celsius ca. 0.0627 Bar (47 mm Hg)
    // PCO2 	carbon dioxid partial pressure, ca. 0.0534 Bar (40 mm Hg)
    // Rq 	the respiratory quotient
    // f 	fraction of inert gases in the breathing mx; e.g.: N2 in dry air = ca. 0.78. Normally for diving we put f = 0,79xx, i.e.: we take into account the various trace gases

    // Schreiner:        Rq = 0.8
    // Workman:          Rq = 0.9
    // Buehlmann / Hahn: Rq = 1.0

    const float waterVaporPp_bars    = 0.0627f;
    const float carbonDioxidePp_bars = 0.0534f;
    const float respritoryQuotient   = 0.9f;
    float       alveolarPressure_bars;

    alveolarPressure_bars =
        (   ambientPressure_bars
         - waterVaporPp_bars
         + (1 - respritoryQuotient ) / respritoryQuotient * carbonDioxidePp_bars
         )
         * gasFraction;

    return alveolarPressure_bars;
}



static float calculateSlidingGradientFactor(  float                depth_meters
                                            , uint8_t              gradHigh_percent
                                            , uint8_t              gradLow_percent
                                            , SystemStatus_Deco_t* decoInfo
                                           )
{

    enum { maxMinDepth_min = 9 };

    float gradientFactor      = 0.0;
    float gradientFactor_high = (float)gradHigh_percent / 100.0f;
    float gradientFactor_low  = (float)gradLow_percent  / 100.0f;

    if( decoInfo->maxMinDepth_meters < maxMinDepth_min )
        decoInfo->maxMinDepth_meters = maxMinDepth_min;

    if( depth_meters > (float)decoInfo->maxMinDepth_meters )
        depth_meters = (float)decoInfo->maxMinDepth_meters;

    gradientFactor = gradientFactor_high -
                  (  (gradientFactor_high - gradientFactor_low)
                   * (depth_meters / (float)decoInfo->maxMinDepth_meters)
                  );


    return gradientFactor;
}



float schreiner(  float   startDepth_meters
                , float   endDepth_meters
                , int     compartment
                , float   time_min
                , bool    simulate
                , GasType gasType
                , float   gasFraction
               )
{
    float   tissueConstant;
    float   rate;
    float   newTissuePressure_bars;
    float   initialTissuePressure_bars;
    float   pressureCopmartment_bars;
    float   halfTime_min;

    switch(gasType)
    {
        case gasType_nitrogen:
            initialTissuePressure_bars = simulate?
                                         compartmentN2[compartment].ttsPressure_bars:
                                         compartmentN2[compartment].pressure_bars;

            halfTime_min = mValueN2[compartment].halfTime_min;
            break;

        case gasType_helium:
            initialTissuePressure_bars = simulate?
                                         compartmentHe[compartment].ttsPressure_bars:
                                         compartmentHe[compartment].pressure_bars;

            halfTime_min = mValueHe[compartment].halfTime_min;
            break;

        default:
            //ASSERT(false);
          break;
    }

    if( time_min == 0 )
        return initialTissuePressure_bars;

    tissueConstant = LOG2 / halfTime_min;
    pressureCopmartment_bars = calculateAlveolarPressure(METERS_SW_TO_BAR(startDepth_meters), gasFraction);

   // ASSERT( time_min != 0 );
    rate = 0.1f * ( endDepth_meters - startDepth_meters ) / time_min;

    newTissuePressure_bars = pressureCopmartment_bars
                           + gasFraction
                           * rate
                           * (time_min - 1 / tissueConstant)
                           - ( pressureCopmartment_bars - initialTissuePressure_bars - gasFraction * rate / tissueConstant)
                           * (float)exp( -tissueConstant * time_min )
                           ;

    return newTissuePressure_bars;
}



float haldane(float pressureCopmartment_bars, float initialTissuePressure_bars, float halfTime_min, float time_min)
{
    float tissueConstant;
    float newTissuePressure_bars;     // new tissue pressure

    tissueConstant = LOG2 / halfTime_min;

    newTissuePressure_bars = pressureCopmartment_bars
                           + ( initialTissuePressure_bars - pressureCopmartment_bars )
                           * exp(-tissueConstant * time_min);

    return newTissuePressure_bars;
}



//---------------------------------------------------------------------
// calculate the minimum depth of a compartment
// returns depth as meters sea water msw

static float calcMinCompartmentDepth(  uint16_t             compartment
                                     , float                depth_meters
                                     , uint8_t              gradHigh_percent
                                     , uint8_t              gradLow_percent
                                     , SystemStatus_Deco_t* decoInfo
                                    )
{
    float ttsPressureN2_bars    = compartmentN2[compartment].ttsPressure_bars;
    float mN2                   = mValueN2     [compartment].m0Value;
    float mN2Delta              = mValueN2     [compartment].deltaM;

    float ttsPressureHe_bars    = compartmentHe[compartment].ttsPressure_bars;
    float mHe                   = mValueHe     [compartment].m0Value;
    float mHeDelta              = mValueHe     [compartment].deltaM;

    float totalTtsPressure_bars;
    float mValue;
    float mDelta;
    float minimumDepth;
    float gradientFactor;

    gradientFactor = calculateSlidingGradientFactor(depth_meters, gradHigh_percent, gradLow_percent, decoInfo);
        // new 2 point gf version

    totalTtsPressure_bars = ttsPressureN2_bars + ttsPressureHe_bars;
    mValue                = ( (mN2 * ttsPressureN2_bars)
                          +   (mHe * ttsPressureHe_bars)
                            )
                          /    totalTtsPressure_bars;

    mDelta                = ( (mN2Delta * ttsPressureN2_bars)
                          +   (mHeDelta * ttsPressureHe_bars)
                            )
                          / totalTtsPressure_bars;

    // add gradient factors
    mValue = gradientFactor * (mValue - 10.0f) + 10.0f; // magic numbers taken from RevC deco algorithm
    mDelta = mDelta * gradientFactor  - gradientFactor + 1.0f;

    minimumDepth = ( (totalTtsPressure_bars * 10.0f) - mValue ) / mDelta;

    if( minimumDepth < 0.0 )
        minimumDepth = 0.0;

    return minimumDepth;
}



// TODO: These two functions are exactly the same except for the last line,
// put common part in another function that returns mDelta and mValue

// calculate the new M value at the new depth
float calculateNewMValue(  float                depth
                         , int                  compartment
                         , uint8_t              gradHigh_percent
                         , uint8_t              gradLow_percent
                         , SystemStatus_Deco_t* decoInfo
                        )
{
    float ttsPressureN2_bars    = compartmentN2[compartment].ttsPressure_bars;
    float mN2                   = mValueN2     [compartment].m0Value;
    float mN2Delta              = mValueN2     [compartment].deltaM;

    float ttsPressureHe_bars    = compartmentHe[compartment].ttsPressure_bars;
    float mHe                   = mValueHe     [compartment].m0Value;
    float mHeDelta              = mValueHe     [compartment].deltaM;

    float totalTtsPressure_bars;
    float mValue;
    float newMValue;
    float mDelta;
    float gradientFactor;

    gradientFactor = calculateSlidingGradientFactor(depth, gradHigh_percent, gradLow_percent, decoInfo);
        // new 2 point gf version

    totalTtsPressure_bars = ttsPressureN2_bars + ttsPressureHe_bars;
    mValue                = ( (mN2 * ttsPressureN2_bars)
                          +   (mHe * ttsPressureHe_bars)
                            )
                          /    totalTtsPressure_bars;

    mDelta                = ( (mN2Delta * ttsPressureN2_bars)
                          +   (mHeDelta * ttsPressureHe_bars)
                            )
                          / totalTtsPressure_bars;


    // add gradient factors
    mValue    = gradientFactor * (mValue - 10.0f) + 10.0f; // magic numbers taken from RevC deco algorithm
    mDelta    = mDelta * gradientFactor - gradientFactor + 1.0f;
    newMValue = mValue + mDelta * depth;


    return newMValue;
}



static float calculateLogTime(  float pressureCopmartment_bars
                              , float initialTissuePressure_bars
                              , float finalTissuePressure_msw
                              , float halfTime_min
                             )
{
    float   m0Value_bars;
    float   time_mins;
    float   pressureDifference_bars;

    m0Value_bars            = finalTissuePressure_msw / 10.0f;  // convert msw to bars TODO: use macro for this
    pressureDifference_bars = (pressureCopmartment_bars - initialTissuePressure_bars);

    if( pressureDifference_bars != 0.0 )
    {
        pressureDifference_bars = (pressureCopmartment_bars - m0Value_bars) / pressureDifference_bars;

        if( pressureDifference_bars < 0 )
        {
            time_mins = -10.0;
        }
        else
        {
            time_mins = (-halfTime_min / LOG2 ) * (float)log(pressureDifference_bars);
        }
    }
    else
    {
        time_mins = 0.0;
    }


    return time_mins;
}



#if 0
    // calculate the no stop time (TODO: find out why it's not used)
    static float calculateNoStopTime(int compartment)
    {
        float m0Value;
        float halfTime_min;

        float pressureN2_bars  = compartmentN2[compartment].pressure_bars;
        float alveolarPressure = calculateAlveolarPressure(METERS_SW_TO_BAR(0.0), 0.78);

        // alveolarPressure fixed at any gas percentage
        m0Value      = mValueN2[compartment].m0Value;
        halfTime_min = mValueN2[compartment].halfTime_min;

        return calculateLogTime(pressureN2_bars, alveolarPressure, m0Value, halfTime_min);
    }
#endif



// calculate the time to surface
void deco_calculateTimeToSurface(Deco_SystemData* decoSystemData, SystemStatus_Deco_t* decoInfo)
{
    float   maxTime_min;

    float   decoTimesN2_min[deco_numberOfCompartments];     // deco times
    float   decoTime_min;
    float   maxTotal;

    float   fractionN2;
    float   fractionHe;
    float   startN2;
    float   nextN2;
    float   startHe;
    float   nextHe;

    float   newDepthN2;
    float   newDepthHe;

    float   ascentTime;
    float   timeTotal;

    uint16_t  previousDepth_meters;
    uint16_t  nextDepth_meters;
    uint16_t  newDepth_meters;

    float   minDepth_meters;

    bool     isFirst;
    bool     isValid;

    uint16_t firstStopTime_min;
    uint16_t firstStopDepth_meters;

    uint16_t i              = 0;
    uint16_t j              = 0;

    enum
    {
        simulationIncTime_min   = 1,
        decoStopInterval_meters = 3,
        maxStopTime_min         = 500,
        simulate                = true,
    };

    isValid              = false;
    isFirst              = true;  // for first deco time
    previousDepth_meters = decoSystemData->depth_deciMeters / 10;


    minDepth_meters = 0.0;

    for(i = 0; i < deco_numberOfCompartments; ++i)
    {
        float tempMinDepth_meters;

        // copy pressure values for simulation
        compartmentN2[i].ttsPressure_bars = compartmentN2[i].pressure_bars;
        compartmentHe[i].ttsPressure_bars = compartmentHe[i].pressure_bars;

        tempMinDepth_meters = calcMinCompartmentDepth(  i
                                                     , (float)decoSystemData->depth_deciMeters / 10.0f
                                                     , decoSystemData->gradientHigh_percent
                                                     , decoSystemData->gradientLow_percent
                                                     , decoInfo
                                                     );


        if( tempMinDepth_meters > minDepth_meters )
            minDepth_meters = tempMinDepth_meters;
    }

    // calculate initial ceiling for this compartment
    // calculate time at this stop
    // msw must round to 3 meter intervals
    //newDepth_meters = ((uint16_t)minDepth_meters + decoStopInterval_meters - 1);
    //newDepth_meters -= newDepth_meters % decoStopInterval_meters;

    newDepth_meters = (uint16_t)(minDepth_meters + 2.9);
    newDepth_meters -= newDepth_meters % decoStopInterval_meters;


    firstStopDepth_meters = newDepth_meters;
    firstStopTime_min     = 0;
    timeTotal             = 0.0;

    if( firstStopDepth_meters > decoInfo->maxMinDepth_meters )
        decoInfo->maxMinDepth_meters = firstStopDepth_meters;

    while( newDepth_meters > 0 )      // big loop
    {

        isValid          = true;
        nextDepth_meters = newDepth_meters - decoStopInterval_meters;

        // add time it takes to asscend from difference in stops
        ascentTime = ((float)previousDepth_meters - (float)newDepth_meters) / (-ASCEND_RATE);
        timeTotal += ascentTime;


        decoSystemData->depth_deciMeters = newDepth_meters * 10;

        calculateCircutData(decoSystemData, &fractionN2, &fractionHe);

        if( ascentTime != 0)
        {
            for(i = 0; i < deco_numberOfCompartments; ++i)           // update all compartments
            {
                compartmentN2[i].ttsPressure_bars = schreiner(previousDepth_meters,
                                                              newDepth_meters,
                                                              i,
                                                              ascentTime,
                                                              simulate,
                                                              gasType_nitrogen,
                                                              fractionN2
                                                             );

                compartmentHe[i].ttsPressure_bars = schreiner(previousDepth_meters,
                                                              newDepth_meters,
                                                              i,
                                                              ascentTime,
                                                              simulate,
                                                              gasType_helium,
                                                              fractionHe
                                                             );
            }
        }

        // we are now at xx msw (simulated)
        // calculate time to stay at that depth
        // calculate the maximum allowed pressure at the next stop (depth -3) msw
        // then calculate how long we have to wait for this to occur
        // do new method of 1 minute iterations
        //

        newDepthHe =  calculateAlveolarPressure(METERS_SW_TO_BAR(newDepth_meters), fractionHe);
        newDepthN2 =  calculateAlveolarPressure(METERS_SW_TO_BAR(newDepth_meters), fractionN2);

        for(i = 0; i < deco_numberOfCompartments; i++)
        {
            startN2 = compartmentN2[i].ttsPressure_bars;
            startHe = compartmentHe[i].ttsPressure_bars;

            maxTotal   =  calculateNewMValue(nextDepth_meters, i, decoSystemData->gradientHigh_percent, decoSystemData->gradientLow_percent, decoInfo) / 10.0f;      // get a new Max value

            for(j = 0; j < maxStopTime_min; j++)
            {
                nextN2 = haldane(newDepthN2, startN2, mValueN2[i].halfTime_min, simulationIncTime_min);
                nextHe = haldane(newDepthHe, startHe, mValueHe[i].halfTime_min, simulationIncTime_min);


                if( (nextN2 + nextHe) <= maxTotal  || j >= maxStopTime_min - 1)
                {
                    break;    // we are done
                }

                startN2 = nextN2;
                startHe = nextHe;
            }
            decoTimesN2_min[i] = (float)(j + 1);
        }

        #if 0 // ->1 print out new m values
            for(i = 0; i < deco_numberOfCompartments; ++i)
            {
            }
        #endif

        // find the max times

        maxTime_min = 0.0;

        for(i = 0; i < deco_numberOfCompartments; ++i)
        {
            if( decoTimesN2_min[i] > maxTime_min )
                maxTime_min = decoTimesN2_min[i];
        }

        decoTime_min = maxTime_min;

        // get the time of the fist stio for the user
        if( isFirst && decoTime_min >= 1 )
        {
            firstStopDepth_meters = (uint16_t)newDepth_meters;
            firstStopTime_min     = (uint16_t)(decoTime_min);
            isFirst = false;
        }

        if( firstStopTime_min == 0 && firstStopDepth_meters != 0 )
            firstStopDepth_meters = 0;


        // now apply the time to the simulated compartments
        timeTotal += decoTime_min;

        for(i = 0; i < deco_numberOfCompartments; ++i)
        {
            compartmentN2[i].ttsPressure_bars = haldane(  newDepthN2
                                                        , compartmentN2[i].ttsPressure_bars
                                                        , mValueN2[i].halfTime_min
                                                        , decoTime_min
                                                        );

            compartmentHe[i].ttsPressure_bars = haldane(  newDepthHe
                                                        , compartmentHe[i].ttsPressure_bars
                                                        , mValueHe[i].halfTime_min
                                                        , decoTime_min
                                                        );
        }

        previousDepth_meters  = newDepth_meters;
        newDepth_meters      -= decoStopInterval_meters;
    }

    if( isValid )
    {
        ascentTime = (previousDepth_meters) / (-ASCEND_RATE);  // bug fix v5.3 tbm

        timeTotal += ascentTime;

        if( timeTotal < firstStopTime_min )
            timeTotal = firstStopTime_min;
    }
    else
    {
        ascentTime                  = 0.0;
        firstStopDepth_meters       = 0;
        firstStopTime_min           = 0;
        timeTotal                   = 0;
    }

  //  vpOs_criticalRegionEnter();
        decoInfo->firstStop_meters  = firstStopDepth_meters;
        decoInfo->firstTime_min     = firstStopTime_min;
        decoInfo->timeToSurface_min = (uint16_t)timeTotal;           // give the user the answer, round up
   // vpOs_criticalRegionLeave();

}



#if 0 // this function is not used, TODO: determine if this is needed

    //---------------------------------------------------------------------------
    // calculate the new tissue pressure accounting for a pressure change
    // The "Schreiner" equation
    // ins:  external START pressure(inspired,alvioli), rate , time , inital compartment pressure
    // outs: new compartment pressure
    //
    // pi = inspired pressure (bars)
    // po = initial tissue compartment pressure
    // ht = half time in minutes
    // t = time in minutes for this to occur
    float   Schreiner5(float Pi, float Po, float HT, float t, float N2)
    {
        float k;
        float NewPt;     // new tissue pressure

        swat();

        k = LOG2 / HT;              // calculate

        NewPt = Pi + N2 * UPRATE * 0.1 * (t - 1 / k) - ( Pi - Po - N2*UPRATE * 0.1 / k) * exp( -k * t );

        return NewPt;

    }
#endif



// compute the new compartment pressures based on depth
static void updateTissue(uint16_t compartment, float depth_meters, float time_min, float fractionN2, float fractionHe)
{
    float   ambientPressure_bars;    // ambient
    float   alveolarPressure_bars;   // alveolar
    float   lastPressure_bars;       // last pressure
    float   newPressureN2_bars;      // new N2 pressure
    float   newPressureHe_bars;      // new HE pressure
    float   halfTime_min;

    ambientPressure_bars = METERS_SW_TO_BAR(depth_meters);

    // update compartments for N2
    lastPressure_bars     = compartmentN2[compartment].pressure_bars;
    alveolarPressure_bars = calculateAlveolarPressure(  ambientPressure_bars
                                                      , fractionN2
                                                      );

    halfTime_min          = mValueN2[compartment].halfTime_min;
    newPressureN2_bars    = haldane(  alveolarPressure_bars
                                    , lastPressure_bars
                                    , halfTime_min
                                    , time_min
                                    );
    compartmentN2[compartment].pressure_bars = newPressureN2_bars;

    // update compartments for He
    lastPressure_bars     = compartmentHe[compartment].pressure_bars;
    alveolarPressure_bars = calculateAlveolarPressure(  ambientPressure_bars
                                                      , fractionHe
                                                      );

    halfTime_min          = mValueHe[compartment].halfTime_min;
    newPressureHe_bars    = haldane(  alveolarPressure_bars
                                    , lastPressure_bars
                                    , halfTime_min
                                    , time_min
                                    );
    compartmentHe[compartment].pressure_bars = newPressureHe_bars;

}



bool deco_existingDependecy (void)
{
    int i;

    for(i = 0; i < deco_numberOfCompartments; ++i)
    {
        if( compartmentN2[i].pressure_bars > EMPTY_N2_COMP_VALUE )
            return true;

        if( compartmentHe[i].pressure_bars > EMPTY_He_COMP_VALUE )
            return true;

    }

    return false;
}



static uint64_t lastUpdateTime_sec  = 0;

void deco_updateTissues(const Deco_SystemData* decoSystemData)
{
    uint8_t i;
    float   elapsedTime_min;
    float   fractionN2 = 0;
    float   fractionHe = 0;

    elapsedTime_min = (float)(decoSystemData->currentDiveTime_sec - lastUpdateTime_sec) / 60.0f;

    if( elapsedTime_min < 0 )
      elapsedTime_min = 0;


    calculateCircutData(decoSystemData, &fractionN2, &fractionHe);

    for(i = 0; i < deco_numberOfCompartments; ++i)
    {
        updateTissue(  i
                     , (float)decoSystemData->depth_deciMeters / 10.0f
                     , elapsedTime_min
                     , fractionN2
                     , fractionHe
                     );
    }

    lastUpdateTime_sec = decoSystemData->currentDiveTime_sec;
}



void deco_getCompartmentPressures_uBars(  CompartmentPressures_uBars decoN2CompartmentPressures_uBars
                                        , CompartmentPressures_uBars decoHeCompartmentPressures_uBars
                                        )
{
    uint8_t i;

    //vpOs_criticalRegionEnter();
        for(i = 0; i < deco_numberOfCompartments; ++i)
        {
            decoN2CompartmentPressures_uBars[i] = (uint32_t)(compartmentN2[i].pressure_bars * 1000000.0);
            decoHeCompartmentPressures_uBars[i] = (uint32_t)(compartmentHe[i].pressure_bars * 1000000.0);
        }
  //  vpOs_criticalRegionLeave();
}



void deco_init(  CompartmentPressures_uBars decoN2CompartmentPressures_uBars
               , CompartmentPressures_uBars decoHeCompartmentPressures_uBars
               , uint64_t lastUpdateTime_sec_
              )
{
    int i;

    if(!init)
    {
        lastUpdateTime_sec = lastUpdateTime_sec_;

        for(i = 0; i < deco_numberOfCompartments; ++i)
        {
            if( decoN2CompartmentPressures_uBars && decoHeCompartmentPressures_uBars )
            {
                compartmentN2[i].pressure_bars = (float)decoN2CompartmentPressures_uBars[i] / 1000000.0f;

                if( compartmentN2[i].pressure_bars < 0 )
                    compartmentN2[i].pressure_bars = 0;

                compartmentHe[i].pressure_bars = (float)decoHeCompartmentPressures_uBars[i] / 1000000.0f;

                if( compartmentHe[i].pressure_bars < 0 )
                    compartmentHe[i].pressure_bars = 0;
            }
            else
            {
                compartmentN2[i].pressure_bars = 0.736f; // tissue pressure in bars at sea level
                compartmentHe[i].pressure_bars = 0.0f;
            }

        }

        init = true;
    }
}



// EOF
