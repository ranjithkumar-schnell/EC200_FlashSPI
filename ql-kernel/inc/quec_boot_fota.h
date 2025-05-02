/**  @file
  ql_boot_spi_flash.h

  @brief
  This file is used to define boot spi flash api for different Quectel Project.

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
17/12/2020        Fei         Init version
=================================================================*/


#ifndef QL_BOOT_SPI_FLASH_H
#define QL_BOOT_SPI_FLASH_H

#include "quec_boot_pff.h"

#ifdef __cplusplus
extern "C" {
#endif
#define QL_FILE_MAX_PATH_LEN         (172)

/*****************************************************************
* Function: quec_boot_fota_init
*
* Description:
* FOTA initialization operations
* 
* Parameters:
*	NULL
*
* Return:
* 	NULL
*
*****************************************************************/
void quec_boot_fota_init();

quec_boot_fs_type_e quec_check_path_type(const char *path,int *offset);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_BOOT_SPI_FLASH_H */




