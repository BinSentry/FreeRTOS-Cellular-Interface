/*
 * FreeRTOS memory safety proofs with CBMC.
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE   OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Standard includes. */
#include <stdint.h>

/* Cellular APIs includes. */
#include "cellular_config_defaults.h"
#include "cellular_types.h"
#include "cellular_common_internal.h"
#include "cellular_common_api.h"

#define ensure_memory_is_valid( px, length )    ( px != NULL ) && __CPROVER_w_ok( ( px ), length )

/* Extern the com interface in comm_if_windows.c */
extern CellularCommInterface_t CellularCommInterface;

/****************************************************************
* The signature of the function under test.
****************************************************************/

CellularError_t Cellular_CommonATCommandRaw( CellularHandle_t cellularHandle,
                                             const char * pATCommandPrefix,
                                             const char * pATCommandPayload,
                                             CellularATCommandType_t atCommandType,
                                             CellularATCommandResponseReceivedCallback_t responseReceivedCallback,
                                             void * pData,
                                             uint16_t dataLen );

/****************************************************************
* The proof of Cellular_CommonATCommandRaw
****************************************************************/
void harness()
{
    CellularHandle_t pHandle = NULL;
    char * pATCommandPrefix;
    char * pATCommandPayload;
    char * pData;
    uint16_t lDataLen;
    uint32_t lATCommandPrefixLen;
    uint32_t lATCommandPayload;
    CellularATCommandType_t atCommandType;

    pATCommandPrefix = safeMalloc( lATCommandPrefixLen );
    pATCommandPayload = safeMalloc( lATCommandPayload );
    pData = safeMalloc( lDataLen );

    /****************************************************************
    * Initialize the member of Cellular_CommonInit.
    ****************************************************************/
    Cellular_CommonInit( nondet_bool() ? NULL : &pHandle, &CellularCommInterface );

    if( ( pHandle == NULL ) ||
        ( ( pHandle != NULL ) && ensure_memory_is_valid( pHandle, sizeof( CellularContext_t ) ) ) )
    {
        Cellular_CommonATCommandRaw( pHandle,
                                     pATCommandPrefix,
                                     pATCommandPayload,
                                     atCommandType,
                                     NULL,
                                     pData,
                                     lDataLen );
    }
}