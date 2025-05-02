/**  @file
  quec_boot_pff.h

  @brief
  This file is used to define boot uart api for different Quectel Project.

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
24/01/2021        Neo         Init version
=================================================================*/

#ifndef QUEC_BOOT_PFF_H
#define QUEC_BOOT_PFF_H

 //#include "diskio.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * Macro Definition
 ===========================================================================*/


/*===========================================================================
 * Enum
 ===========================================================================*/
typedef enum
{
    QUEC_BOOT_FS_NONE            = -1,

    QUEC_BOOT_SFFS_EXT           = 0,
    QUEC_BOOT_FAT_SDMMC          = 1,
    QUEC_BOOT_FAT_EXNAND_FLASH,
    QUEC_BOOT_FAT_EXTNOR_FLASH
}quec_boot_fs_type_e;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/

//DRESULT ql_boot_fat_disk_read(BYTE *buff, DWORD sector, UINT count);
int quec_boot_mount(void);
void quec_boot_read_sdfile(void);
int quec_pfats_file_size(void);
void quec_get_real_path(const char* path,char** realpath);



#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_BOOT_PFF_H */


