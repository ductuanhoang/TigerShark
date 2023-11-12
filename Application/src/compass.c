#include "compass.h"
#include "Fusion.h"
#include "DFRobot_BMX160.h"
#include "flash_process.h"
//#include "bmx160.h"
//#include "bmi160.h"
//#include "i2c.h"

#define SAMPLE_PERIOD (0.02f)

#define SAMPLE_RATE (50)


FusionVector gyroscope = {0.0f, 0.0f, 0.0f}; // replace this with actual gyroscope data in degrees/s
FusionVector accelerometer = {0.0f, 0.0f, 1.0f}; // replace this with actual accelerometer data in g
FusionVector magnetometer = {1.0f, 0.0f, 0.0f}; // replace this with actual magnetometer data in arbitrary units
const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
const FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};
const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};
const FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector hardIronOffset = {0.0f, 0.0f, 0.0f};

float mag_buff[20];
float sum_mag = 0;
uint8_t mag_id = 0;
uint16_t north_magnetic_degree;
extern saving_data_t config_data;
// Initialise algorithms
FusionOffset offset;
FusionAhrs ahrs;
// Set AHRS algorithm settings
const FusionAhrsSettings settings = {
        .gain = 0.5f,
        .accelerationRejection = 10.0f,
        .magneticRejection = 20.0f,
        .rejectionTimeout = 5 * SAMPLE_RATE, /* 5 seconds */
};


void compass_init(void)
{
    FusionOffsetInitialise(&offset, SAMPLE_RATE);
    FusionAhrsInitialise(&ahrs);
    FusionAhrsSetSettings(&ahrs, &settings);
}


void compass_loop(void)
{
    getAllData(&Omagn, &Ogyro, &Oaccel);
    gyroscope.axis.x = Ogyro.x;
    gyroscope.axis.y = Ogyro.y;
    gyroscope.axis.z = Ogyro.z;

    accelerometer.axis.x = Oaccel.x;
    accelerometer.axis.y = Oaccel.y;
    accelerometer.axis.z = Oaccel.z;

    magnetometer.axis.x = Omagn.x;
    magnetometer.axis.y = Omagn.y;
    magnetometer.axis.z = Omagn.z;

    gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
    accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
    magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);
    // Update gyroscope offset correction algorithm
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);
    FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, SAMPLE_PERIOD);
    mag_buff[mag_id] = FusionCompassCalculateHeading(accelerometer,magnetometer);
    mag_id++;
    if(mag_id >= 20)
    {
        mag_id = 0;
    }
    sum_mag = 0;
    for(uint8_t id = 0; id < 20; id++)
    {
        sum_mag += mag_buff[id];
    }
    if(sum_mag < 0)
    {
        north_magnetic_degree = (360-(uint16_t)((sum_mag/20) + 360));
    }
    else
    {
        north_magnetic_degree = (360- (uint16_t)(sum_mag/20));
        if(north_magnetic_degree == 360)
        {
            north_magnetic_degree = 0;
        }
    }
    if(config_data.user_setting.settings.flags.screenRotation == 1)
    {
        uint16_t convert_compass = 0;
        convert_compass = north_magnetic_degree + 180;
        north_magnetic_degree  = (convert_compass%360);
    }
}
