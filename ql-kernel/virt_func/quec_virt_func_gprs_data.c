/**  @file
  quec_virt_func_gprs_data.c

  @brief
  This file is used to run virt function.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
#include "at_response.h"
#include "at_engine.h"
#include "quec_cust_feature.h"
#include "quec_common.h"
#include "at_command.h"
#include "at_param.h"
#include "ql_api_osi.h"
#include "quec_at_engine.h"
#include "quec_log.h"
//#include "quec_rtos.h"
#include "ql_api_osi.h"
#include "ql_api_usbnet.h"

#define QUEC_VIRT_MODEM_FUNC_LOG(msg, ...)  custom_log("VIRT_MFUNC", msg, ##__VA_ARGS__)

//
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GPRS_DATA_TRANSFER
__attribute__((weak)) bool ql_gprs_data_transfer_get_support(void)
{
	return false;
}

__attribute__((weak)) unsigned char ql_gprs_data_transfer_get_sim_cid(void)
{
	return 0;
}
__attribute__((weak))  void ql_gprs_data_transfer_input(void *payload, unsigned short tot_len)
{    
}

#endif/*CONFIG_QUEC_PROJECT_FEATURE_GPRS_DATA_TRANSFER*/