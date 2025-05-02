/**  @file
  quec_at_engine.h

  @brief
  This file is used to define at command engine functions for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2021 Quectel Wireless Solution, Co., Ltd.
  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------



------------     -------     -------------------------------------------------------------------------------
=================================================================*/

#ifndef QUEC_AT_ENGINE_H
#define QUEC_AT_ENGINE_H


#include "at_engine.h"
#include "at_command.h"
#include "quec_proj_config.h"


#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
*  							Macro
**************************************************************************/


/*************************************************************************
*  							Enum
**************************************************************************/
typedef enum ATCI_RESULT_CODE
{
    ATCI_RESULT_CODE_NULL, 
    ATCI_RESULT_CODE_OK,   
    ATCI_RESULT_CODE_ERROR, 
    ATCI_RESULT_CODE_CME_ERROR, 
    ATCI_RESULT_CODE_CMS_ERROR, 
    ATCI_RESULT_CODE_MAX
}_AtciResultCode; 


/*************************************************************************
*  							Struct
**************************************************************************/
 
 
/*************************************************************************
*	                        Variable
**************************************************************************/


/*************************************************************************
*	                        Function
**************************************************************************/
/**********************************************************************************************************************
    output your AT cmd response sentence segment.
//length: the min is 2
//padding   :   if need add \r\n at the begin and/or end of the string, the begin padding is effected by atv
//		and set as 1 for default
***********************************************************************************************************************/
int  quec_atCmdRespInfoNText(atCmdEngine_t *engine, const char *text, size_t length, unsigned char padding);

/**********************************************************************************************************************
    response at cmd with your result code like _AtciResultCode, and report code is your err code.
***********************************************************************************************************************/
void quec_atCmdResp(atCmdEngine_t *engine, _AtciResultCode resultCode, uint report_code);

/**********************************************************************************************************************
    resultCode:   ATCI_RESULT_CODE_OK  ATCI_RESULT_CODE_ERROR  ATCI_RESULT_CODE_CME_ERROR  ATCI_RESULT_CODE_CMS_ERROR
    report_code:  CMD_RC_OK,  CMD_RC_ERROR, CMD_RC_CONNECT, CMD_RC_BUSY ......
	rsp_buffer:   the buffer to be send out , if no buffer to send , input NULL
	padding   :   if need add \r\n at the begin and/or end of the string, the begin padding is effected by atv
		and set as 1 for default
***********************************************************************************************************************/
void quec_atResp(atCmdEngine_t *engine, _AtciResultCode resultCode, uint report_code, char *rsp_buffer , unsigned char padding);

void quec_atCmdRespOutputPrompt(atCmdEngine_t *engine, int formatcfg);
void _atDeviceUserialInit_Ext(unsigned port);



#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_AT_ENGINE_H */






