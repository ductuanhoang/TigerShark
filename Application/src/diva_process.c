#include "diva_process.h"
#include "usart.h"
#include "crc16.h"
#include "string.h"
extern SystemStatus_Deco_t   decoInfo;

#define VERSION   1026
extern UART_HandleTypeDef huart3;
DivaStatus divaStatus;
uint8_t  divaStatusPacket[20];
uint8_t txPacket[packetSize_max];

const uint8_t packet_prefix[3] =
{
    0xAA, 0x5A, 0x77
};
static DecoState updateDivaDecoState(const s_SystemStatus* systemStatus, const s_ControlSettings* controlSettings)
{
    enum
    {
        decoWindow_deciMeters            = 60,
        decoWindowSegmentSize_deciMeters = 5,
        decoWindowNumSegments            = decoWindow_deciMeters / decoWindowSegmentSize_deciMeters
    };

    int16_t     error_deciMeters = systemStatus->depth_decimeters - decoInfo.firstStop_meters * 10;
    DecoState decoState        = decoState_invalid;

    enum
    {
        floor_deciMeters         = 30,
        belowTarget_5_deciMeters = 25,
        belowTarget_4_deciMeters = 20,
        belowTarget_3_deciMeters = 15,
        belowTarget_2_deciMeters = 10,
        belowTarget_1_deciMeters = 5,
        atTarget_deciMeters      = 0,
        aboveTarget_1_deciMeters = -5,
        aboveTarget_2_deciMeters = -10,
        aboveTarget_3_deciMeters = -15,
        aboveTarget_4_deciMeters = -20,
        aboveTarget_5_deciMeters = -25,
        ceiling_deciMeters       = -30,
    };

    if( error_deciMeters > floor_deciMeters )
    {
        decoState = decoState_floor;
    }
    else if( error_deciMeters < ceiling_deciMeters )
    {
        decoState = decoState_ceiling;
    }
    else
    {
        error_deciMeters -= error_deciMeters % decoWindowSegmentSize_deciMeters;

        switch( error_deciMeters )
        {
        case floor_deciMeters:
            decoState = decoState_floor;
            break;

        case belowTarget_5_deciMeters:
            decoState = decoState_belowTarget_5;
            break;

        case belowTarget_4_deciMeters:
            decoState = decoState_belowTarget_4;
            break;

        case belowTarget_3_deciMeters:
            decoState = decoState_belowTarget_3;
            break;

        case belowTarget_2_deciMeters:
            decoState = decoState_belowTarget_2;
            break;

        case belowTarget_1_deciMeters:
            decoState = decoState_belowTarget_1;
            break;

        case atTarget_deciMeters:
            decoState = decoState_atTarget;
            break;

        case aboveTarget_1_deciMeters:
            decoState = decoState_aboveTarget_1;
            break;

        case aboveTarget_2_deciMeters:
            decoState = decoState_aboveTarget_2;
            break;

        case aboveTarget_3_deciMeters:
            decoState = decoState_aboveTarget_3;
            break;

        case aboveTarget_4_deciMeters:
            decoState = decoState_aboveTarget_4;
            break;

        case aboveTarget_5_deciMeters:
            decoState = decoState_aboveTarget_5;
            break;

        case ceiling_deciMeters:
            decoState = decoState_ceiling;
            break;

        default:
            decoState = decoState_invalid;
        }
    }

    return decoState;
}

void packet_writeHeader(  uint8_t *buffer
                        , uint8_t          type
                        , uint8_t            senderId
                        , uint8_t            destId
                        , const uint8_t*      packetData
                        , size_t              packetDataSize
                        )
{
    uint16_t  packetCrc = 0;


    memcpy(buffer, packet_prefix, sizeof(packet_prefix));

    buffer[packetIndex_version] = (VERSION) >> 8;
    buffer[packetIndex_version +1] = (VERSION & 0xFF);
    buffer[packetIndex_size] = ((packetDataSize + packetSize_header)) >> 8;
    buffer[packetIndex_size +1] = ((packetDataSize + packetSize_header) & 0xFF);
    buffer[packetIndex_type] = (type ) >> 8;
    buffer[packetIndex_type +1] = (type & 0xFF);

    buffer[packetIndex_senderId] = senderId;
    buffer[packetIndex_destId]   = destId;

    packetCrc = vp_crc16(0, buffer + packetIndex_version, packetSize_header - packetIndex_version);
    packetCrc = vp_crc16(packetCrc, packetData, packetDataSize);

    
    buffer[packetIndex_crc16] = (packetCrc ) >> 8;
    buffer[packetIndex_crc16 +1] = (packetCrc & 0xFF);
}

bool packet_sendPacket(uint8_t destId, uint8_t packetType, const uint8_t* dataPacket, size_t dataSize)
{
    bool status     = false;
    size_t packetSize = 13 + dataSize;

    packet_writeHeader(  txPacket
                       , packetType
                       , 2
                       , destId
                       , dataPacket
                       , dataSize
                       );

    memcpy(txPacket + 13, dataPacket, dataSize); 
    HAL_UART_Transmit(&huart3,txPacket,packetSize,100);
    return status;
}

void sendDivaStatus(const s_SystemStatus *systemStatus, const s_ControlSettings* controlSettings)
{
    // TODO: Make sure diva is authorized and either this is secondary, or diva set to run on primary
  
    divaStatus.ppO2_atm_percent[0]          = (uint8_t)systemStatus->ppO2_atm_percent[0];
    divaStatus.ppO2_atm_percent[1]          = (uint8_t)systemStatus->ppO2_atm_percent[1];
    divaStatus.ppO2_atm_percent[2]          = (uint8_t)systemStatus->ppO2_atm_percent[2];

    divaStatus.setpoint_atm_percent         = controlSettings->setPoint_centiAta;
    divaStatus.ppO2_avg_atm_percent         = systemStatus->ppO2_avg_atm_percent;

    divaStatus.diveLedBrightness_percent    = controlSettings->diveLedBrightness_percent;
    divaStatus.decoLedBrightness_percent    = controlSettings->decoLedBrightness_percent;
    divaStatus.flashlightBrightness_percent = controlSettings->flashlightBrightness_percent;
    divaStatus.whiteLedBrightness_percent   = 75; // TODO: put this in dive settings and save to flash so user can change it

    divaStatus.settings.all                 = controlSettings->settings.all;
    divaStatus.alerts.all                   = systemStatus->alerts.all;

    if( decoInfo.firstStop_meters > 0 )
        divaStatus.settings.flags.enableDecoLeds = controlSettings->settings.flags.enableDecoLeds;
    else
        divaStatus.settings.flags.enableDecoLeds = false;

    if( divaStatus.settings.flags.enableDecoLeds )
        divaStatus.decoState = updateDivaDecoState(systemStatus, controlSettings);

    memcpy(divaStatusPacket,(uint8_t*)&divaStatus,20);
    packet_sendPacket(3, 8, divaStatusPacket, 20);
}

