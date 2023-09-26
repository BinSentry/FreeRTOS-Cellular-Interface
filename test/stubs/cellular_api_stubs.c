//
// Created by mvermeer on 01/06/23.
//

/* The config header is always included first. */
#ifndef CELLULAR_DO_NOT_USE_CUSTOM_CONFIG
#include "cellular_config.h"
#endif
#include "cellular_config_defaults.h"
#include "cellular_api.h"
#include <stddef.h>
#include <string.h>

uint32_t fakeCellularHandle = 0;

CellularError_t Cellular_Init( CellularHandle_t * pCellularHandle,
                               const CellularCommInterface_t * pCommInterface ) {
    if (pCellularHandle != NULL) {
        *pCellularHandle = (CellularHandle_t)&fakeCellularHandle;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_Cleanup( CellularHandle_t cellularHandle ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetRatPriority( CellularHandle_t cellularHandle,
                                         const CellularRat_t * pRatPriorities,
                                         uint8_t ratPrioritiesLength ) {
    return CELLULAR_SUCCESS;
}


CellularError_t Cellular_GetRatPriority( CellularHandle_t cellularHandle,
                                         CellularRat_t * pRatPriorities,
                                         uint8_t ratPrioritiesLength,
                                         uint8_t * pOutputRatPrioritiesLength ) {
    if (pRatPriorities != NULL && ratPrioritiesLength > 0 && pOutputRatPrioritiesLength != NULL) {
        pRatPriorities[0] = CELLULAR_RAT_LTE;
        *pOutputRatPrioritiesLength = 1;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_RfOn( CellularHandle_t cellularHandle ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_RfOff( CellularHandle_t cellularHandle ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetSimCardStatus( CellularHandle_t cellularHandle,
                                           CellularSimCardStatus_t * pSimCardStatus ) {
    if (cellularHandle != NULL && pSimCardStatus != NULL) {
        pSimCardStatus->simCardLockState = CELLULAR_SIM_CARD_READY;
        pSimCardStatus->simCardState = CELLULAR_SIM_CARD_INSERTED;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetSimCardInfo( CellularHandle_t cellularHandle,
                                         CellularSimCardInfo_t * pSimCardInfo ) {
    if (cellularHandle != NULL && pSimCardInfo != NULL) {
        strncpy(pSimCardInfo->iccid, "8942310022006040012F", sizeof(pSimCardInfo->iccid));
        strncpy(pSimCardInfo->imsi, "295050915435001", sizeof(pSimCardInfo->imsi));
        strncpy(pSimCardInfo->plmn.mcc, "302", sizeof(pSimCardInfo->plmn.mcc));
        strncpy(pSimCardInfo->plmn.mnc, "220", sizeof(pSimCardInfo->plmn.mnc));
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetModemInfo( CellularHandle_t cellularHandle,
                                       CellularModemInfo_t * pModemInfo ) {
    if (cellularHandle != NULL && pModemInfo != NULL) {
        strncpy(pModemInfo->hardwareVersion, "", sizeof(pModemInfo->hardwareVersion));
        strncpy(pModemInfo->firmwareVersion, "BG770AGLAAR01A05", sizeof(pModemInfo->firmwareVersion));
        strncpy(pModemInfo->serialNumber, "", sizeof(pModemInfo->serialNumber));
        strncpy(pModemInfo->imei, "863593057981722", sizeof(pModemInfo->imei));
        strncpy(pModemInfo->manufactureId, "Quectel", sizeof(pModemInfo->manufactureId));
        strncpy(pModemInfo->modelId, "BG770A-GL", sizeof(pModemInfo->modelId));
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetRegisteredNetwork( CellularHandle_t cellularHandle,
                                               CellularPlmnInfo_t * pNetworkInfo ) {
    if (cellularHandle != NULL && pNetworkInfo != NULL) {
        strncpy(pNetworkInfo->mcc, "302", sizeof(pNetworkInfo->mcc));
        strncpy(pNetworkInfo->mnc, "220", sizeof(pNetworkInfo->mnc));
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetNetworkTime( CellularHandle_t cellularHandle,
                                         CellularTime_t * pNetworkTime ) {
    if (cellularHandle != NULL && pNetworkTime != NULL) {
        pNetworkTime->year = 2023;
        pNetworkTime->month = 6;
        pNetworkTime->day = 1;
        pNetworkTime->hour = 11;
        pNetworkTime->minute = 11;
        pNetworkTime->second = 11;
        pNetworkTime->dst = 1;
        pNetworkTime->timeZone = -16;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetSignalInfo( CellularHandle_t cellularHandle,
                                        CellularSignalInfo_t * pSignalInfo ) {
    if (cellularHandle != NULL && pSignalInfo != NULL) {
        pSignalInfo->rssi = -50;
        pSignalInfo->rsrp = -1;
        pSignalInfo->rsrq = -10;
        pSignalInfo->sinr = -1;
        pSignalInfo->ber = 0;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetServiceStatus( CellularHandle_t cellularHandle,
                                           CellularServiceStatus_t * pServiceStatus ) {
    if (cellularHandle != NULL && pServiceStatus != NULL) {
        memset(pServiceStatus, 0, sizeof(*pServiceStatus));
        pServiceStatus->rat = CELLULAR_RAT_LTE;
        pServiceStatus->networkRegistrationMode = REGISTRATION_MODE_AUTO;
        pServiceStatus->csRegistrationStatus = REGISTRATION_STATUS_ROAMING_REGISTERED;
        pServiceStatus->psRegistrationStatus = REGISTRATION_STATUS_ROAMING_REGISTERED;
        strncpy(pServiceStatus->plmnInfo.mcc, "302", sizeof(pServiceStatus->plmnInfo.mcc));
        strncpy(pServiceStatus->plmnInfo.mnc, "220", sizeof(pServiceStatus->plmnInfo.mnc));
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetPdnConfig( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       const CellularPdnConfig_t * pPdnConfig ) {
    if (cellularHandle != NULL && pPdnConfig != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetPdnStatus( CellularHandle_t cellularHandle,
                                       CellularPdnStatus_t * pPdnStatusBuffers,
                                       uint8_t numStatusBuffers,
                                       uint8_t * pNumStatus ) {
    if (cellularHandle != NULL && pPdnStatusBuffers != NULL && numStatusBuffers > 0 && pNumStatus != NULL) {
        pPdnStatusBuffers[1].state = 1; // activated
        pPdnStatusBuffers[1].contextId = CELLULAR_PDN_CONTEXT_ID;
        pPdnStatusBuffers[1].ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
        strncpy(pPdnStatusBuffers[1].ipAddress.ipAddress, "192.168.1.25", sizeof(pPdnStatusBuffers[1].ipAddress.ipAddress));
        pPdnStatusBuffers[1].pdnContextType = CELLULAR_PDN_CONTEXT_IPV4;
        *pNumStatus = 1;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_ActivatePdn( CellularHandle_t cellularHandle,
                                      uint8_t contextId ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_DeactivatePdn( CellularHandle_t cellularHandle,
                                        uint8_t contextId ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetIPAddress( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       char * pBuffer,
                                       uint32_t bufferLength )  {
    if (cellularHandle != NULL && pBuffer != NULL && bufferLength > 0) {
        if (contextId == CELLULAR_PDN_CONTEXT_ID) {
            strncpy(pBuffer, "192.168.1.25", bufferLength);
            pBuffer[bufferLength - 1] = '\0';
        } else {
            pBuffer[0] = '\0';
        }

        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetDns( CellularHandle_t cellularHandle,
                                 uint8_t contextId,
                                 const char * pPrimaryDnsServerAddress,
                                 const char * pSecondaryDnsServerAddress ) {
    if (cellularHandle != NULL && pPrimaryDnsServerAddress != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_RegisterUrcNetworkRegistrationEventCallback( CellularHandle_t cellularHandle,
                                                                      CellularUrcNetworkRegistrationCallback_t networkRegistrationCallback,
                                                                      void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_RegisterUrcPdnEventCallback( CellularHandle_t cellularHandle,
                                                      CellularUrcPdnEventCallback_t pdnEventCallback,
                                                      void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_RegisterUrcSignalStrengthChangedCallback( CellularHandle_t cellularHandle,
                                                                   CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback,
                                                                   void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_RegisterModemEventCallback( CellularHandle_t cellularHandle,
                                                     CellularModemEventCallback_t modemEventCallback,
                                                     void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_RegisterUrcGenericCallback( CellularHandle_t cellularHandle,
                                                     CellularUrcGenericCallback_t genericCallback,
                                                     void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetPsmSettings( CellularHandle_t cellularHandle,
                                         CellularPsmSettings_t * pPsmSettings ) {
    if (cellularHandle != NULL && pPsmSettings != NULL) {
        memset(pPsmSettings, 0, sizeof(*pPsmSettings));
        pPsmSettings->mode = 1;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetPsmSettings( CellularHandle_t cellularHandle,
                                         const CellularPsmSettings_t * pPsmSettings ) {
    if (cellularHandle != NULL && pPsmSettings != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetPsmConfigSettings( CellularHandle_t cellularHandle,
                                               CellularPsmConfigSettings_t * pPsmConfigSettings ) {
    if (cellularHandle != NULL && pPsmConfigSettings != NULL) {
        pPsmConfigSettings->psmVersion = 0xF;
        pPsmConfigSettings->threshold = 20;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetPsmConfigSettings( CellularHandle_t cellularHandle,
                                               const CellularPsmConfigSettings_t * pPsmConfigSettings ) {
    if (cellularHandle != NULL && pPsmConfigSettings != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetEidrxSettings( CellularHandle_t cellularHandle,
                                           CellularEidrxSettingsList_t * pEidrxSettingsList ) {
    if (cellularHandle != NULL && pEidrxSettingsList != NULL) {
        pEidrxSettingsList->count = 1;
        memset(pEidrxSettingsList->eidrxList, 0, sizeof(CellularEidrxSettings_t) * pEidrxSettingsList->count);
        pEidrxSettingsList->eidrxList[0].rat = CELLULAR_RAT_LTE;
        pEidrxSettingsList->eidrxList[0].mode = 0;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetEidrxSettings( CellularHandle_t cellularHandle,
                                           const CellularEidrxSettings_t * pEidrxSettings ) {
    if (cellularHandle != NULL && pEidrxSettings != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_ATCommandRaw( CellularHandle_t cellularHandle,
                                       const char * pATCommandPrefix,
                                       const char * pATCommandPayload,
                                       CellularATCommandType_t atCommandType,
                                       CellularATCommandResponseReceivedCallback_t responseReceivedCallback,
                                       void * pData,
                                       uint16_t dataLen ) {
    if (cellularHandle != NULL && pATCommandPrefix != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

static uint32_t fakeSocketContext = 0;
CellularError_t Cellular_CreateSocket( CellularHandle_t cellularHandle,
                                       uint8_t pdnContextId,
                                       CellularSocketDomain_t socketDomain,
                                       CellularSocketType_t socketType,
                                       CellularSocketProtocol_t socketProtocol,
                                       CellularSocketHandle_t * pSocketHandle ) {
    if (cellularHandle != NULL && pSocketHandle != NULL) {
        *pSocketHandle = (CellularSocketHandle_t)&fakeSocketContext;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_CreateSSLSocket( CellularHandle_t cellularHandle,
                                          uint8_t pdnContextId,
                                          uint8_t sslContextId,
                                          CellularSocketDomain_t socketDomain,
                                          CellularSocketHandle_t * pSocketHandle ) {
    if (cellularHandle != NULL && pSocketHandle != NULL) {
        *pSocketHandle = (CellularSocketHandle_t)&fakeSocketContext;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketConnect( CellularHandle_t cellularHandle,
                                        CellularSocketHandle_t socketHandle,
                                        CellularSocketAccessMode_t dataAccessMode,
                                        const CellularSocketAddress_t * pRemoteSocketAddress ) {
    if (cellularHandle != NULL && pRemoteSocketAddress != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketSend( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     const uint8_t * pData,
                                     uint32_t dataLength,
                                     uint32_t * pSentDataLength ) {
    if (cellularHandle != NULL && pData != NULL) {
        if (pSentDataLength != NULL) {
            *pSentDataLength = dataLength;
        }

        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

/**
 * @brief Receive data on a connected socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[out] pBuffer The buffer to receive the data into.
 * @param[in] bufferLength Length of the buffer pBuffer.
 * @param[out] pReceivedDataLength Out parameter to provide the of the actual
 * data received in the buffer pBuffer. Note that it may be less than
 * bufferLength.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketRecv( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     uint8_t * pBuffer,
                                     uint32_t bufferLength,
                                     uint32_t * pReceivedDataLength ) {
    if (cellularHandle != NULL && pBuffer != NULL && pReceivedDataLength != NULL) {
        *pReceivedDataLength = 0;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketClose( CellularHandle_t cellularHandle,
                                      CellularSocketHandle_t socketHandle,
                                      bool removeSocketOnError ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetSocketLastResultCode( CellularHandle_t cellularHandle,
                                                  uint32_t * lastResultCode ) {
    if (cellularHandle != NULL && lastResultCode != NULL) {
        *lastResultCode = 0;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetSocketReceiveStats( CellularHandle_t cellularHandle,
                                                CellularSocketHandle_t socketHandle,
                                                CellularSocketReceiveStatistics_t * receiveStatistics ) {
    if (cellularHandle != NULL && receiveStatistics != NULL) {
        receiveStatistics->totalReceiveLength = 1000;
        receiveStatistics->haveReadLength = 900;
        receiveStatistics->unreadLength = 100;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetHostByName( CellularHandle_t cellularHandle,
                                        uint8_t contextId,
                                        const char * pcHostName,
                                        char * pResolvedAddress ) {
    if (cellularHandle != NULL && pcHostName != NULL && pResolvedAddress != NULL) {
        strncpy(pResolvedAddress, "192.168.1.25", CELLULAR_IP_ADDRESS_MAX_SIZE);
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketSetSockOpt( CellularHandle_t cellularHandle,
                                           CellularSocketHandle_t socketHandle,
                                           CellularSocketOptionLevel_t optionLevel,
                                           CellularSocketOption_t option,
                                           const uint8_t * pOptionValue,
                                           uint32_t optionValueLength ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketSetSSLOpt( CellularHandle_t cellularHandle,
                                          uint8_t sslContextId,
                                          CellularSSLContextOption_t option,
                                          const uint8_t * pOptionValue,
                                          uint32_t optionValueLength ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketRegisterSocketOpenCallback( CellularHandle_t cellularHandle,
                                                           CellularSocketHandle_t socketHandle,
                                                           CellularSocketOpenCallback_t socketOpenCallback,
                                                           void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketRegisterDataReadyCallback( CellularHandle_t cellularHandle,
                                                          CellularSocketHandle_t socketHandle,
                                                          CellularSocketDataReadyCallback_t dataReadyCallback,
                                                          void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SocketRegisterClosedCallback( CellularHandle_t cellularHandle,
                                                       CellularSocketHandle_t socketHandle,
                                                       CellularSocketClosedCallback_t closedCallback,
                                                       void * pCallbackContext ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_UploadFileToModem( CellularHandle_t cellularHandle,
                                            const char * pcFilename,
                                            const uint8_t * pFile,
                                            uint32_t fileLength,
                                            CellularFileUploadResult_t * fileUploadResult ) {
    if (cellularHandle != NULL && pcFilename != NULL && pFile != NULL) {
        if (fileUploadResult != NULL) {
            fileUploadResult->uploadedFileLength = fileLength;
            fileUploadResult->xorChecksum = 0xFFFF;
        }

        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_DeleteFileOnModem( CellularHandle_t cellularHandle,
                                            const char * pcFilename ) {
    if (cellularHandle != NULL && pcFilename != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetModemFileCRC32( CellularHandle_t cellularHandle,
                                            const char * pcFilename,
                                            uint32_t * crc32 ) {
    if (cellularHandle != NULL && pcFilename != NULL && crc32 != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetModuleFlowControlSetting( CellularHandle_t cellularHandle,
                                                      CellularModuleCommFlowControl_t * flowControl ) {
    if (cellularHandle != NULL && flowControl != NULL) {
        *flowControl = CELLULAR_MODULE_COMM_FLOW_CONTROL_RTS_CTS;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetModuleFlowControlSetting( CellularHandle_t cellularHandle,
                                                      CellularModuleCommFlowControl_t * flowControl ) {
    if (cellularHandle != NULL && flowControl != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_GetModuleBaudRateSetting( CellularHandle_t cellularHandle,
                                                   uint32_t * baudRate ) {
    if (cellularHandle != NULL && baudRate != NULL) {
        *baudRate = 230400;
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetModuleBaudRateSetting( CellularHandle_t cellularHandle,
                                                   uint32_t baudRate ) {
    if (cellularHandle != NULL && baudRate != 0) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_PowerDown( CellularHandle_t cellularHandle,
                                    CellularPowerDownMode_t powerDownMode ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}

CellularError_t Cellular_SetPSMEntry( CellularHandle_t cellularHandle,
                                      CellularPSMEnterMode_t psmEnterMode ) {
    if (cellularHandle != NULL) {
        return CELLULAR_SUCCESS;
    } else {
        return CELLULAR_BAD_PARAMETER;
    }
}
