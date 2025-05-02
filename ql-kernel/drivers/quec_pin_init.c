/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------
=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include "quec_cust_feature.h"
#include "quec_common.h"

#include "hal_chip.h"
#include "quec_pin_cfg.h"
#include "ql_gpio_internal.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QUEC_GPIOINIT_LOG(msg, ...)  custom_log("QGPIO_Init", msg, ##__VA_ARGS__)

/*===========================================================================
 * Functions
 ===========================================================================*/
__attribute__((weak)) void quec_pin_cfg_init(void)
{
    uint8_t       index        = 0;
    uint8_t       pin_num      = 0;
    uint8_t       default_func = 0;
    uint8_t       gpio_func    = 0;
    ql_GpioNum    gpio_num     = 0;
    ql_GpioDir    gpio_dir     = 0;
    ql_PullMode   gpio_pull    = 0;
    ql_LvlMode    gpio_lvl     = 0;

    for( index = 0; index < ql_model_diff_ctx.pin_num; index++ )
    {
        //QUEC_GPIOINIT_LOG("pin%d=%d", index, quec_pin_cfg_map[index].pin_num);
        if (QUEC_PIN_NONE == quec_pin_cfg_map[index].pin_num)
        {
            QUEC_GPIOINIT_LOG("init exit %d!", index);
            break;
        }
        pin_num      = quec_pin_cfg_map[index].pin_num;
        default_func = quec_pin_cfg_map[index].default_func;
        gpio_func    = quec_pin_cfg_map[index].gpio_func;
        gpio_num     = quec_pin_cfg_map[index].gpio_num;
        gpio_dir     = quec_pin_cfg_map[index].gpio_dir;
        gpio_pull    = quec_pin_cfg_map[index].gpio_pull;
        gpio_lvl     = quec_pin_cfg_map[index].gpio_lvl;

        ql_pin_set_func_internal(pin_num, default_func);
        if( default_func == gpio_func )
        {
            ql_gpio_init(gpio_num, gpio_dir, gpio_pull, gpio_lvl);
        }
    }

    /* Power Domain ON */
    for( index = 1; index <= 3; index++ )
    {   /* index: 1=LCD 2=SD 3=SIM1 */
        halPmuSwitchPower(ql_power_domain_table[index].id, ql_power_domain_table[index].enabled, ql_power_domain_table[index].lp_enabled);
        halPmuSetPowerLevel(ql_power_domain_table[index].id, ql_power_domain_table[index].mv);
    }

}

#endif  /* CONFIG_QUEC_PROJECT_FEATURE_GPIO */

#ifdef CONFIG_BOARD_WITH_EXT_FLASH
#include "hal_iomux.h"

void quec_ext_flash_init(void)
{
    halIomuxSetFunction(HAL_IOMUX_FUN_SPI_FLASH1_CLK);
    halIomuxSetFunction(HAL_IOMUX_FUN_SPI_FLASH1_CS);
    halIomuxSetFunction(HAL_IOMUX_FUN_SPI_FLASH1_SIO_0);
    halIomuxSetFunction(HAL_IOMUX_FUN_SPI_FLASH1_SIO_1);
    halIomuxSetFunction(HAL_IOMUX_FUN_SPI_FLASH1_SIO_2);
    halIomuxSetFunction(HAL_IOMUX_FUN_SPI_FLASH1_SIO_3);
}
#endif

