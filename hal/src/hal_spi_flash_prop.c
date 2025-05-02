

#include "hal_spi_flash.h"
#include "osi_compiler.h"
#include "hal_spi_flash.h"
#include "hal_chip.h"
#include "hwregs.h"
#include "osi_profile.h"
#include "osi_byte_buf.h"
#include "hal_spi_flash_defs.h"
#include <alloca.h>
/**
 * Flash property table. It is not necessary to put it in ram.
 */
halSpiFlash_t ql_SpiFlashProps[] = {
#include "hal_spi_flash_prop.h"
};
int ql_SpiFlashPropsLen = OSI_ARRAY_SIZE(ql_SpiFlashProps);

bool quec_spi_flash_props_init(halSpiFlash_t **props_ptr,int *props_len)
{
	if(props_ptr == NULL || props_len == NULL)
	{
		return false;
	}
	
	*props_ptr = ql_SpiFlashProps;
	*props_len = ql_SpiFlashPropsLen;
	return true;	
}






