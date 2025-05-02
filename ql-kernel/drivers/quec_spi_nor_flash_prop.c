/**  @file
  quec_spi_nor_flash_prop.h

  @brief
  This file is used to define spi NOR FLASH api for open Quectel Project.

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
#include "quec_spi_nor_flash_prop.h"

/* 4-line SPI NOR Flash supported model list,can add by yourself*/
static quec_spi_nor_flash_info_s quec_spi_nor_flash_props[] = 
{
    { 
        // GD25LE64E
        // GD25LE64C
        .mid = 0x1760c8,
        .capacity = 0x800000,
        .mode = QUEC_NOR_FLASH_MODE_16BIT,
    },
    {
        // XM25QU64B
        .mid = 0x175020,
        .capacity = 0x800000,
        .mode = QUEC_NOR_FLASH_MODE_8BIT,
    },
    {
        // XM25QU128C
        .mid = 0x184120,
        .capacity = 0x1000000,
        .mode = QUEC_NOR_FLASH_MODE_8BIT,
    }, 
    {
        // W25Q64JWSSIM
        .mid = 0x1780ef,
        .capacity = 0x800000,
        .mode = QUEC_NOR_FLASH_MODE_8BIT,
    },
};

static unsigned int quec_spi_nor_flash_prop_len = sizeof(quec_spi_nor_flash_props)/sizeof(quec_spi_nor_flash_props[0]);

bool quec_spi_nor_props_init(quec_spi_nor_flash_info_s **props_ptr,unsigned int *props_len)
{
	if(props_ptr == NULL || props_len == NULL)
	{
		return false;
	}
	
	*props_ptr = quec_spi_nor_flash_props;
	*props_len = quec_spi_nor_flash_prop_len;
	return true;	
}


