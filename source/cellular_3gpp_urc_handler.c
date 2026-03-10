/*
 * FreeRTOS-Cellular-Interface v1.3.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 */

/**
 * @brief FreeRTOS Cellular Library URC handler implemenation with 3GPP URC.
 */


#ifndef CELLULAR_DO_NOT_USE_CUSTOM_CONFIG
    /* Include custom config file before other headers. */
    #include "cellular_config.h"
#endif
#include "cellular_config_defaults.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "cellular_platform.h"
#include "cellular_types.h"
#include "cellular_internal.h"
#include "cellular_common_internal.h"
#include "cellular_common_api.h"

/*-----------------------------------------------------------*/

#define CELLULAR_REG_POS_URC_MODE     ( 1U )    /* Only specified when queried, not in URC */
#define CELLULAR_REG_POS_STAT         ( 2U )
#define CELLULAR_REG_POS_LAC_TAC      ( 3U )
#define CELLULAR_REG_POS_CELL_ID      ( 4U )
#define CELLULAR_REG_POS_RAT          ( 5U )
#define CELLULAR_REG_POS_REJ_TYPE     ( 6U )
#define CELLULAR_REG_POS_REJ_CAUSE    ( 7U )
#define CELLULAR_REG_POS_URC_ACTIVE_TIME ( 8U )
#define CELLULAR_REG_POS_URC_PERIODIC_TAU_RAU ( 9U )

#define CELLULAR_REG_URC_MODE_DISABLED                  ( 0U )
#define CELLULAR_REG_URC_MODE_STAT_ENABLED              ( 1U )
#define CELLULAR_REG_URC_MODE_STAT_LOCATION_ENABLED     ( 2U )
#define CELLULAR_REG_URC_MODE_STAT_LOCATION_PSM_ENABLED ( 4U )
#define CELLULAR_REG_URC_MODE_UNKNOWN                   ( 0xFFU )

/*-----------------------------------------------------------*/
static CellularATError_t _parseUrcModeInRegStatus( const char * pToken,
                                                   uint8_t * urcMode );
static CellularPktStatus_t _parseRegStatusInRegStatusParsing( CellularContext_t * pContext,
                                                              CellularNetworkRegType_t regType,
                                                              const char * pToken,
                                                              cellularAtData_t * pLibAtData );
static CellularPktStatus_t _parseLacTacInRegStatus( CellularNetworkRegType_t regType,
                                                    const char * pToken,
                                                    cellularAtData_t * pLibAtData,
                                                    bool allowEmpty );
static CellularPktStatus_t _parseCellIdInRegStatus( const char * pToken,
                                                    cellularAtData_t * pLibAtData,
                                                    bool allowEmpty );
static CellularPktStatus_t _parseRatInfoInRegStatus( const char * pToken,
                                                     cellularAtData_t * pLibAtData,
                                                     bool allowEmpty );
static CellularPktStatus_t _parseRejectTypeInRegStatus( CellularNetworkRegType_t regType,
                                                        const char * pToken,
                                                        cellularAtData_t * pLibAtData,
                                                        bool allowEmpty );
static CellularPktStatus_t _parseRejectCauseInRegStatus( CellularNetworkRegType_t regType,
                                                         const char * pToken,
                                                         cellularAtData_t * pLibAtData,
                                                         bool allowEmpty );
static CellularPktStatus_t _parseActiveTimeInRegStatus( const char * pToken,
                                                        cellularAtData_t * pLibAtData,
                                                        bool allowEmpty );
static CellularPktStatus_t _parsePeriodicTauInRegStatus( const char * pToken,
                                                         cellularAtData_t * pLibAtData,
                                                         bool allowEmpty );
static CellularPktStatus_t _regStatusSwitchParsingFunc( CellularContext_t * pContext,
                                                        uint8_t i,
                                                        CellularNetworkRegType_t regType,
                                                        bool isUrc,
                                                        uint8_t urcMode,
                                                        const char * pToken,
                                                        cellularAtData_t * pLibAtData );
static void _regStatusGenerateLog( char * pRegPayload,
                                   CellularNetworkRegType_t regType );
static void _regStatusGenerateEvent( const CellularContext_t * pContext,
                                     CellularNetworkRegType_t regType,
                                     const cellularAtData_t * pLibAtData );
static bool _Cellular_RegEventStatus( const cellularAtData_t * pLibAtData,
                                      CellularNetworkRegType_t regType,
                                      CellularNetworkRegistrationStatus_t prevCsRegStatus,
                                      CellularNetworkRegistrationStatus_t prevPsRegStatus );

/*-----------------------------------------------------------*/

static CellularATError_t _parseUrcModeInRegStatus( const char * pToken,
                                                   uint8_t * urcMode )
{
    int32_t var = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

    if( urcMode == NULL )
    {
        atCoreStatus = CELLULAR_AT_BAD_PARAMETER;
    }
    else
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &var );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( var >= 0 ) && ( var <= UINT8_MAX ) )
            {
                *urcMode = ( uint8_t ) var;
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
                LogError( ( "Error in processing URC mode in reg status. Token '%s'", pToken ) );
            }
        }
    }

    return atCoreStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseRegStatusInRegStatusParsing( CellularContext_t * pContext,
                                                              CellularNetworkRegType_t regType,
                                                              const char * pToken,
                                                              cellularAtData_t * pLibAtData )
{
    int32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    CellularNetworkRegistrationStatus_t regStatus = REGISTRATION_STATUS_UNKNOWN;

    if( ( regType != CELLULAR_REG_TYPE_CREG ) && ( regType != CELLULAR_REG_TYPE_CEREG ) &&
        ( regType != CELLULAR_REG_TYPE_CGREG ) )
    {
        packetStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue < ( int32_t ) REGISTRATION_STATUS_MAX ) )
            {
                /* MISRA Ref 10.5.1 [Essential type casting] */
                /* More details at: https://github.com/FreeRTOS/FreeRTOS-Cellular-Interface/blob/main/MISRA.md#rule-105 */
                /* coverity[misra_c_2012_rule_10_5_violation] */
                regStatus = ( CellularNetworkRegistrationStatus_t ) tempValue;
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    if( packetStatus == CELLULAR_PKT_STATUS_OK )
    {
        if( regType == CELLULAR_REG_TYPE_CREG )
        {
            pLibAtData->csRegStatus = regStatus;
        }
        else
        {
            pLibAtData->psRegStatus = regStatus;
        }

        if( regStatus == REGISTRATION_STATUS_REGISTERED_HOME )
        {
            LogDebug( ( "Network registration : HOME" ) );
        }
        else if( regStatus == REGISTRATION_STATUS_ROAMING_REGISTERED )
        {
            LogDebug( ( "Network registration : ROAMING" ) );
        }
        else if( regStatus == REGISTRATION_STATUS_REGISTRATION_DENIED )
        {
            /* clear the atlib data if the registration failed. */
            LogDebug( ( "Network registration : DEINED" ) );
            _Cellular_InitAtData( pContext, 1 );
        }
        else
        {
            /* clear the atlib data if the registration failed. */
            LogDebug( ( "Network registration : OTHERS" ) );
            _Cellular_InitAtData( pContext, 1 );
        }
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseLacTacInRegStatus( CellularNetworkRegType_t regType,
                                                    const char * pToken,
                                                    cellularAtData_t * pLibAtData,
                                                    bool allowEmpty )
{
    int32_t tempValue = 0;
    uint16_t var = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool skipParsing = false;

    if( ( pToken != NULL ) && ( pToken[0] == '\0' ) )
    {
        if( allowEmpty == true )
        {
            skipParsing = true;
        }
        else
        {
            LogDebug( ( "Unexpected empty LAC/TAC in Registration Status" ) );
        }
    }

    if( skipParsing != true )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 16, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= UINT16_MAX ) )
            {
                var = ( uint16_t ) tempValue;
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /* Parsing Location area code for CREG or CGREG. */
            if( ( regType == CELLULAR_REG_TYPE_CREG ) || ( regType == CELLULAR_REG_TYPE_CGREG ) )
            {
                pLibAtData->lac = ( uint16_t ) var;
            }
            /* Parsing Tracking area code for CEREG. */
            else if( regType == CELLULAR_REG_TYPE_CEREG )
            {
                pLibAtData->tac = ( uint16_t ) var;
            }
            else
            {
                /* Empty else MISRA 15.7 */
            }
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseCellIdInRegStatus( const char * pToken,
                                                    cellularAtData_t * pLibAtData,
                                                    bool allowEmpty )
{
    uint32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool skipParsing = false;

    if( ( pToken != NULL ) && ( pToken[0] == '\0' ) )
    {
        if( allowEmpty == true )
        {
            skipParsing = true;
        }
        else
        {
            LogDebug( ( "Unexpected empty Cell ID in Registration Status" ) );
        }
    }

    if( skipParsing != true )
    {
        atCoreStatus = Cellular_ATStrtoui( pToken, 16, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pLibAtData->cellId = tempValue;
        }
        else
        {
            LogError( ( "Error in processing Cell Id. Token '%s'", pToken ) );
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseRatInfoInRegStatus( const char * pToken,
                                                     cellularAtData_t * pLibAtData,
                                                     bool allowEmpty )
{
    int32_t var = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool skipParsing = false;

    if( ( pToken != NULL ) && ( pToken[0] == '\0' ) )
    {
        if( allowEmpty == true )
        {
            skipParsing = true;
        }
        else
        {
            LogDebug( ( "Unexpected empty RAT Info in Registration Status" ) );
        }
    }

    if( skipParsing != true )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &var );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( var >= ( int32_t ) CELLULAR_RAT_MAX )
            {
                atCoreStatus = CELLULAR_AT_ERROR;
                LogError( ( "Error in processing RAT. Token '%s'", pToken ) );
            }
            else if( ( var == ( int32_t ) CELLULAR_RAT_GSM ) || ( var == ( int32_t ) CELLULAR_RAT_EDGE ) ||
                     ( var == ( int32_t ) CELLULAR_RAT_CATM1 ) || ( var == ( int32_t ) CELLULAR_RAT_NBIOT ) )
            {
                /* MISRA Ref 10.5.1 [Essential type casting] */
                /* More details at: https://github.com/FreeRTOS/FreeRTOS-Cellular-Interface/blob/main/MISRA.md#rule-105 */
                /* coverity[misra_c_2012_rule_10_5_violation] */
                pLibAtData->rat = ( CellularRat_t ) var;
            }
            else if( var == ( int32_t ) CELLULAR_RAT_LTE )
            {
                /* Some cellular module use 7 : CELLULAR_RAT_LTE to indicate CAT-M1. */
                pLibAtData->rat = ( CellularRat_t ) CELLULAR_RAT_LTE;
            }
            else
            {
                pLibAtData->rat = CELLULAR_RAT_INVALID;
            }
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseRejectTypeInRegStatus( CellularNetworkRegType_t regType,
                                                        const char * pToken,
                                                        cellularAtData_t * pLibAtData,
                                                        bool allowEmpty )
{
    int32_t tempValue = 0;
    uint8_t rejType = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool skipParsing = false;

    if( ( pToken != NULL ) && ( pToken[0] == '\0' ) )
    {
        if( allowEmpty == true )
        {
            skipParsing = true;
        }
        else
        {
            LogDebug( ( "Unexpected empty Reject Type in Registration Status" ) );
        }
    }

    if( skipParsing != true )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= ( int32_t ) UINT8_MAX ) )
            {
                rejType = ( uint8_t ) tempValue;
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( regType == CELLULAR_REG_TYPE_CREG )
            {
                /* Reject Type is only stored if the registration status is denied. */
                if( pLibAtData->csRegStatus == REGISTRATION_STATUS_REGISTRATION_DENIED )
                {
                    pLibAtData->csRejectType = rejType;
                }
            }
            else if( ( regType == CELLULAR_REG_TYPE_CGREG ) || ( regType == CELLULAR_REG_TYPE_CEREG ) )
            {
                if( pLibAtData->psRegStatus == REGISTRATION_STATUS_REGISTRATION_DENIED )
                {
                    pLibAtData->psRejectType = rejType;
                }
            }
            else
            {
                /* Empty else MISRA 15.7 */
            }
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseRejectCauseInRegStatus( CellularNetworkRegType_t regType,
                                                         const char * pToken,
                                                         cellularAtData_t * pLibAtData,
                                                         bool allowEmpty )
{
    int32_t tempValue = 0;
    uint8_t rejCause = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool skipParsing = false;

    if( ( pToken != NULL ) && ( pToken[0] == '\0' ) )
    {
        if( allowEmpty == true )
        {
            skipParsing = true;
        }
        else
        {
            LogDebug( ( "Unexpected empty Reject Cause in Registration Status" ) );
        }
    }

    if( skipParsing != true )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= ( int32_t ) UINT8_MAX ) )
            {
                rejCause = ( uint8_t ) tempValue;
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( regType == CELLULAR_REG_TYPE_CREG )
            {
                if( pLibAtData->csRegStatus == REGISTRATION_STATUS_REGISTRATION_DENIED )
                {
                    pLibAtData->csRejCause = rejCause;
                }
            }
            else if( ( regType == CELLULAR_REG_TYPE_CGREG ) || ( regType == CELLULAR_REG_TYPE_CEREG ) )
            {
                if( pLibAtData->psRegStatus == REGISTRATION_STATUS_REGISTRATION_DENIED )
                {
                    pLibAtData->psRejCause = rejCause;
                }
            }
            else
            {
                /* Empty else MISRA 15.7 */
            }
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseActiveTimeInRegStatus( const char * pToken,
                                                        cellularAtData_t * pLibAtData,
                                                        bool allowEmpty )
{
    int32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool skipParsing = false;

    if( ( pToken != NULL ) && ( pToken[0] == '\0' ) )
    {
        if( allowEmpty == true )
        {
            pLibAtData->activeTimeValue = 0xFFFFFFFF;
            skipParsing = true;
        }
        else
        {
            LogWarn( ( "Unexpected empty Active Time in Registration Status" ) );
        }
    }

    if( skipParsing != true )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 2, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= UINT8_MAX ) )
            {
                pLibAtData->activeTimeValue = ( uint32_t ) tempValue;
            }
            else
            {
                LogError( ( "Error in processing Active Time value. Token '%s'", pToken ) );
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parsePeriodicTauInRegStatus( const char * pToken,
                                                         cellularAtData_t * pLibAtData,
                                                         bool allowEmpty )
{
    int32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool skipParsing = false;

    if( ( pToken != NULL ) && ( pToken[0] == '\0' ) )
    {
        if( allowEmpty == true )
        {
            pLibAtData->periodicTauValue = 0xFFFFFFFF;
            skipParsing = true;
        }
        else
        {
            LogWarn( ( "Unexpected empty Periodic TAU in Registration Status" ) );
        }
    }

    if( skipParsing != true )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 2, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= UINT8_MAX ) )
            {
                pLibAtData->periodicTauValue = ( uint32_t ) tempValue;
            }
            else
            {
                LogError( ( "Error in processing Periodic TAU value. Token '%s'", pToken ) );
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _regStatusSwitchParsingFunc( CellularContext_t * pContext,
                                                        uint8_t i,
                                                        CellularNetworkRegType_t regType,
                                                        bool isUrc,
                                                        uint8_t urcMode,
                                                        const char * pToken,
                                                        cellularAtData_t * pLibAtData )
{
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    bool isUrcOrUrcModePSM = ( ( isUrc == true ) || ( urcMode == CELLULAR_REG_URC_MODE_STAT_LOCATION_PSM_ENABLED ) );

    if( ( urcMode != CELLULAR_REG_URC_MODE_UNKNOWN ) && ( isUrc == true ) )
    {
        LogWarn( ( "Unexpected known URC mode and isURC in Registration Status, i: %hhu, regType: %d, urcMode: %hhu", i, regType, urcMode ) );
    }

    if( ( urcMode == CELLULAR_REG_URC_MODE_STAT_LOCATION_PSM_ENABLED ) && ( regType == CELLULAR_REG_TYPE_CREG ) )
    {
        LogWarn( ( "Unexpected PSM URC mode in CREG Registration Status, i: %hhu", i ) );
    }

    switch( i )
    {
        case CELLULAR_REG_POS_URC_MODE:
            LogError( ( "Unexpected URC Mode Position in Registration: %s",
                        ( ( pToken != NULL ) ? pToken : "<null>" ) ) ) ;
            break;

        /* Parsing network Registration status in CREG or CGREG or CEREG response. */
        case CELLULAR_REG_POS_STAT:
            packetStatus = _parseRegStatusInRegStatusParsing( pContext, regType, pToken, pLibAtData );
            break;

        /* Parsing Location Area Code (LAC) or Tracking Area Code (TAC). */
        case CELLULAR_REG_POS_LAC_TAC:
            packetStatus = _parseLacTacInRegStatus( regType, pToken, pLibAtData, isUrcOrUrcModePSM /* allowEmpty= */ );
            break;

        /* Parsing Cell ID. */
        case CELLULAR_REG_POS_CELL_ID:
            packetStatus = _parseCellIdInRegStatus( pToken, pLibAtData, isUrcOrUrcModePSM /* allowEmpty= */ );
            break;

        /* Parsing RAT Information. */
        case CELLULAR_REG_POS_RAT:
            packetStatus = _parseRatInfoInRegStatus( pToken, pLibAtData, isUrcOrUrcModePSM /* allowEmpty= */ );
            break;

        /* Parsing Reject Type. */
        case CELLULAR_REG_POS_REJ_TYPE:
            packetStatus = _parseRejectTypeInRegStatus( regType, pToken, pLibAtData, isUrcOrUrcModePSM /* allowEmpty= */ );
            break;

        /* Parsing the Reject Cause. */
        case CELLULAR_REG_POS_REJ_CAUSE:
            packetStatus = _parseRejectCauseInRegStatus( regType, pToken, pLibAtData, isUrcOrUrcModePSM /* allowEmpty= */ );
            break;

        /* Parsing the URC Active Time */
        case CELLULAR_REG_POS_URC_ACTIVE_TIME:
            if( regType == CELLULAR_REG_TYPE_CGREG )
            {
                /* Not currently supported */
                LogWarn( ( "CGREG Active Time Unsupported in Registration Status" ) );
            }
            else if( regType == CELLULAR_REG_TYPE_CREG )
            {
                LogError( ( "Unexpected Active Time in CREG Registration Status" ) );
            }
            else
            {
                packetStatus = _parseActiveTimeInRegStatus( pToken, pLibAtData, true /* allowEmpty= */ );
            }
            break;

        /* Parsing the URC Periodic TAU / RAU */
        case CELLULAR_REG_POS_URC_PERIODIC_TAU_RAU:
            if( regType == CELLULAR_REG_TYPE_CGREG )
            {
                /* Not currently supported */
                LogWarn( ( "CGREG Periodic RAU Unsupported in Registration Status" ) );
            }
            else if( regType == CELLULAR_REG_TYPE_CREG )
            {
                LogError( ( "Unexpected Periodic TAU / RAU in CREG Registration Status" ) );
            }
            else
            {
                packetStatus = _parsePeriodicTauInRegStatus( pToken, pLibAtData, true /* allowEmpty= */ );
            }
            break;

        default:
            LogDebug( ( "Unknown Parameter Position in Registration URC: %hhu", i ) );
            break;
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

static void _regStatusGenerateLog( char * pRegPayload,
                                   CellularNetworkRegType_t regType )
{
    ( void ) pRegPayload;

    if( regType == CELLULAR_REG_TYPE_CREG )
    {
        LogDebug( ( "URC: CREG: %s", pRegPayload ) );
    }
    else if( regType == CELLULAR_REG_TYPE_CGREG )
    {
        LogDebug( ( "URC: CGREG: %s", pRegPayload ) );
    }
    else if( regType == CELLULAR_REG_TYPE_CEREG )
    {
        LogDebug( ( "URC: CEREG: %s", pRegPayload ) );
    }
    else
    {
        LogDebug( ( " URC: Unknown " ) );
    }
}

/*-----------------------------------------------------------*/

static void _regStatusGenerateEvent( const CellularContext_t * pContext,
                                     CellularNetworkRegType_t regType,
                                     const cellularAtData_t * pLibAtData )
{
    CellularServiceStatus_t serviceStatus;

    ( void ) memset( &serviceStatus, 0, sizeof( CellularServiceStatus_t ) );
    serviceStatus.rat = pLibAtData->rat;
    serviceStatus.csRegistrationStatus = pLibAtData->csRegStatus;
    serviceStatus.psRegistrationStatus = pLibAtData->psRegStatus;
    serviceStatus.csRejectionCause = pLibAtData->csRejCause;
    serviceStatus.csRejectionType = pLibAtData->csRejectType;
    serviceStatus.psRejectionCause = pLibAtData->psRejCause;
    serviceStatus.psRejectionType = pLibAtData->psRejectType;

    /* Data should be obtained from COPS commmand. User should obtain with APIs. */
    ( void ) strcpy( serviceStatus.plmnInfo.mcc, "FFF" );
    ( void ) strcpy( serviceStatus.plmnInfo.mnc, "FFF" );
    ( void ) strcpy( serviceStatus.operatorName, "FFF" );
    serviceStatus.operatorNameFormat = OPERATOR_NAME_FORMAT_NOT_PRESENT;
    serviceStatus.networkRegistrationMode = REGISTRATION_MODE_UNKNOWN;

    if( pContext->cbEvents.networkRegistrationCallback != NULL )
    {
        if( regType == CELLULAR_REG_TYPE_CREG )
        {
            _Cellular_NetworkRegistrationCallback( pContext, CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION, &serviceStatus );
        }
        else
        {
            _Cellular_NetworkRegistrationCallback( pContext, CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION, &serviceStatus );
        }
    }
}

/*-----------------------------------------------------------*/

/* check registration event change to decide to generate event. */
static bool _Cellular_RegEventStatus( const cellularAtData_t * pLibAtData,
                                      CellularNetworkRegType_t regType,
                                      CellularNetworkRegistrationStatus_t prevCsRegStatus,
                                      CellularNetworkRegistrationStatus_t prevPsRegStatus )
{
    bool retEventChanged = false;

    switch( regType )
    {
        case CELLULAR_REG_TYPE_CREG:

            if( pLibAtData->csRegStatus != prevCsRegStatus )
            {
                retEventChanged = true;
            }

            break;

        case CELLULAR_REG_TYPE_CGREG:
        case CELLULAR_REG_TYPE_CEREG:

            if( pLibAtData->psRegStatus != prevPsRegStatus )
            {
                retEventChanged = true;
            }

            break;

        default:
            LogInfo( ( "_Cellular_RegEventStatus : unknown reg type " ) );
            break;
    }

    return retEventChanged;
}

/*-----------------------------------------------------------*/

CellularPktStatus_t _Cellular_ParseRegStatus( CellularContext_t * pContext,
                                              char * pRegPayload,
                                              bool isUrc,
                                              CellularNetworkRegType_t regType )
{
    uint8_t i = 0, urcMode = CELLULAR_REG_URC_MODE_UNKNOWN;
    char * pRegStr = NULL, * pToken = NULL;
    cellularAtData_t * pLibAtData = NULL;
    CellularPktStatus_t packetStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularNetworkRegistrationStatus_t prevCsRegStatus = REGISTRATION_STATUS_UNKNOWN;
    CellularNetworkRegistrationStatus_t prevPsRegStatus = REGISTRATION_STATUS_UNKNOWN;

    if( pContext == NULL )
    {
        packetStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( pRegPayload == NULL )
    {
        packetStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pLibAtData = &pContext->libAtData;

        pRegStr = pRegPayload;

        // FUTURE: Remove once satisfied with Registration Status parsing, currently at error level to guarantee logging
        LogError( ( "%s: '%s'",
                    ( ( regType == CELLULAR_REG_TYPE_CREG ) ? "CREG" :
                        ( ( regType == CELLULAR_REG_TYPE_CEREG ) ? "CEREG" :
                            ( ( regType == CELLULAR_REG_TYPE_CGREG ) ? "CGREG" :
                                ( ( regType == CELLULAR_REG_TYPE_UNKNOWN ) ? "unknown" : "<invalid>" ) ) ) ),
                    ( ( pRegPayload != NULL ) ? pRegPayload : "<null>" ) ) );

        atCoreStatus = Cellular_ATRemoveAllDoubleQuote( pRegStr );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pRegStr );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATGetNextTok( &pRegStr, &pToken );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /* If not URC then first token will be the URC mode (<n>) */
            if( isUrc != true )
            {
                atCoreStatus = _parseUrcModeInRegStatus( pToken, &urcMode );

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    /* Get next token after URC mode */
                    atCoreStatus = Cellular_ATGetNextTok( &pRegStr, &pToken );
                }
            }

            i++;
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /* Backup the previous regStatus. */
            prevCsRegStatus = pLibAtData->csRegStatus;
            prevPsRegStatus = pLibAtData->psRegStatus;

            while( pToken != NULL )
            {
                i++;
                packetStatus = _regStatusSwitchParsingFunc( pContext, i, regType, isUrc,
                                                            urcMode, pToken, pLibAtData );

                if( packetStatus != CELLULAR_PKT_STATUS_OK )
                {
                    LogError( ("Failed to parse item %hhu: '%s', regType: %d, urcMode: %hhu, isURC: %s",
                                i, ( (pToken != NULL) ? pToken : "<null>" ), regType, urcMode,
                                ( ( isUrc == true ) ? "true" : "false" ) ) );
                }

                /* Getting next token to parse. */
                if( Cellular_ATGetNextTok( &pRegStr, &pToken ) != CELLULAR_AT_SUCCESS )
                {
                    break;
                }
            }
        }

        /* Generate logs. */
        if( isUrc == true )
        {
            _regStatusGenerateLog( pRegPayload, regType );
        }

        /* If Registration Status changed, generate the event. */
        if( ( _Cellular_RegEventStatus( pLibAtData, regType, prevCsRegStatus, prevPsRegStatus ) == true ) )
        {
            _regStatusGenerateEvent( pContext, regType, pLibAtData );
        }

        if( atCoreStatus != CELLULAR_AT_SUCCESS )
        {
            packetStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
        }
    }

    return packetStatus;
}

/*-----------------------------------------------------------*/

CellularPktStatus_t Cellular_CommonUrcProcessCreg( CellularContext_t * pContext,
                                                   char * pInputLine )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext != NULL )
    {
        _Cellular_LockAtDataMutex( pContext );
        pktStatus = _Cellular_ParseRegStatus( pContext, pInputLine, true, CELLULAR_REG_TYPE_CREG );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            LogDebug( ( "Creg Parse failure" ) );
        }

        _Cellular_UnlockAtDataMutex( pContext );
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

CellularPktStatus_t Cellular_CommonUrcProcessCgreg( CellularContext_t * pContext,
                                                    char * pInputLine )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext != NULL )
    {
        _Cellular_LockAtDataMutex( pContext );
        pktStatus = _Cellular_ParseRegStatus( pContext, pInputLine, true, CELLULAR_REG_TYPE_CGREG );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            LogDebug( ( "Cgreg Parse failure" ) );
        }

        _Cellular_UnlockAtDataMutex( pContext );
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

CellularPktStatus_t Cellular_CommonUrcProcessCereg( CellularContext_t * pContext,
                                                    char * pInputLine )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext != NULL )
    {
        _Cellular_LockAtDataMutex( pContext );
        pktStatus = _Cellular_ParseRegStatus( pContext, pInputLine, true, CELLULAR_REG_TYPE_CEREG );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            LogDebug( ( "Cereg Parse failure" ) );
        }

        _Cellular_UnlockAtDataMutex( pContext );
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/
