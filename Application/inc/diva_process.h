#ifndef _DIVA_PROCESS_H_
#define _DIVA_PROCESS_H_

//#include "main.h"
#include "deco.h"
#include "main_process.h"

typedef struct
{
    uint8_t                ppO2_atm_percent[3];

    uint8_t                setpoint_atm_percent;
    uint8_t                ppO2_avg_atm_percent;

    uint8_t                diveLedBrightness_percent;
    uint8_t                decoLedBrightness_percent;
    uint8_t                flashlightBrightness_percent;

    uint8_t                whiteLedBrightness_percent;

    uint16_t               decoState;

    UserSettings_Settings  settings;                         
    SystemStatus_Alerts alerts;

} DivaStatus;

typedef enum
{
    decoState_floor,
    decoState_belowTarget_5,
    decoState_belowTarget_4,
    decoState_belowTarget_3,
    decoState_belowTarget_2,
    decoState_belowTarget_1,
    decoState_atTarget,
    decoState_aboveTarget_1,
    decoState_aboveTarget_2,
    decoState_aboveTarget_3,
    decoState_aboveTarget_4,
    decoState_aboveTarget_5,
    decoState_ceiling,

    decoState_invalid,
} DecoState;
enum PacketIndex
{
    packetIndex_prefix,
    packetIndex_crc16          = packetIndex_prefix   + 3,
    packetIndex_version        = packetIndex_crc16    + 2,
    packetIndex_size           = packetIndex_version  + 2,
    packetIndex_type           = packetIndex_size     + 2,
    packetIndex_senderId       = packetIndex_type     + 2,
    packetIndex_destId         = packetIndex_senderId + 1,
    packetIndex_dataStart      = packetIndex_destId   + 1,
    packetIndex_packetStatus   = packetIndex_dataStart,
    packetIndex_divaStatus     = packetIndex_dataStart,
    packetIndex_systemStatus   = packetIndex_dataStart,
    packetIndex_logData        = packetIndex_dataStart
};

enum PacketSize
{
    packetSize_header            = packetIndex_dataStart,
    packetSize_packetStatus      = 1,
    packetSize_displayStatus     = 1,
//    packetSize_command           = 2,
//    packetSize_divaStatus        = sizeof(DivaStatus),
//
//
//    packetSize_serialNumber      = 5 + (16 - (5 % 16)),
//    packetSize_flashData         = appFlashSize_maxSize  + (16 - (appFlashSize_maxSize % 16)),
//
//    packetSize_deviceInfo        = deviceInfoSize_size,
//    packetSize_systemStatus      = sizeof(SystemStatus),
//    packetSize_controlSettings   = sizeof(ControlSettings),
//    packetSize_logEntry          = logEntrySize_size,
//    packetSize_o2CalInfo         = o2CalInfoSize_size,

    packetSize_max               = packetSize_header + 100,
//    packetSize_bslMax            = packetSize_header + packetSize_flashData,
//    packetSize_divaMax           = packetSize_header + packetSize_divaStatus
};

void sendDivaStatus(const s_SystemStatus *systemStatus, const s_ControlSettings* controlSettings);

#endif