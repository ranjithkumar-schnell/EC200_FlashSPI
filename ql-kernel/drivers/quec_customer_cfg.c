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
#include "quec_customer_cfg.h"
#include "ql_uart.h"
#include "quec_pin_index.h"
#include "hal_chip.h"
#include "quec_cust_feature.h"

/*===========================================================================
 * Customer set Map
 ===========================================================================*/
const ql_model_diff_ctx_s ql_model_diff_ctx = 
{
#ifdef CONFIG_QL_PROJECT_DEF_EC200U
    3, 1, UART_PIN_MAX, QUEC_PIN_CFG_MAX, QL_INPUT_HPMIC_L, false,
#elif defined CONFIG_QL_PROJECT_DEF_EC600U
#ifndef CONFIG_QL_OPEN_EXPORT_PKG
    4, 0, UART_PIN_MAX, QUEC_PIN_CFG_MAX, QL_INPUT_HPMIC_L, false,
#else
    4, 1, UART_PIN_MAX, QUEC_PIN_CFG_MAX, QL_INPUT_HPMIC_L, false,
#endif
#elif defined CONFIG_QL_PROJECT_DEF_EG700U
    4, 1, UART_PIN_MAX, QUEC_PIN_CFG_MAX, QL_INPUT_HPMIC_L, false,
#elif defined CONFIG_QL_PROJECT_DEF_EG500U
    4, 1, UART_PIN_MAX, QUEC_PIN_CFG_MAX, QL_INPUT_MAINMIC, false,
#elif defined CONFIG_QL_PROJECT_DEF_EG915U
    2, 0, UART_PIN_MAX, QUEC_PIN_CFG_MAX, QL_INPUT_MAINMIC, true,
#endif
};

const ql_model_partinfo_ctx_s ql_model_partinfo_ctx = 
{
    CONFIG_APP_FLASH_ADDRESS,
    CONFIG_APP_FLASH_SIZE,
    CONFIG_APPIMG_FLASH_ADDRESS,
    CONFIG_APPIMG_FLASH_SIZE,
    CONFIG_FS_SYS_FLASH_ADDRESS,
    CONFIG_FS_SYS_FLASH_SIZE,
    CONFIG_FS_MODEM_FLASH_ADDRESS,
    CONFIG_FS_MODEM_FLASH_SIZE,
};

const ql_model_feature_ctx_s ql_model_feature_ctx = {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOLTE
    .volte_enabled = true,
#else
    .volte_enabled = false,
#endif
    .app_ram_offset     = CONFIG_APP_RAM_OFFSET,
    .app_total_ram_size = CONFIG_APP_TOTAL_RAM_SIZE,

#ifdef CONFIG_QUEC_PROJECT_FEATURE_QDSIM
    .dual_sim_enabled = true,
    .sim_count        = 2,
    .ap_ifc_dma_count = 3,                  //double sim,ap can use 3 ifc dma channel
#else
    .dual_sim_enabled = false,
    .sim_count        = 1,
    .ap_ifc_dma_count = 5,                  //single sim,ap can use 5 ifc dma channel
#endif
};

const ql_uart_func_s ql_uart_pin_func[] = {
#ifdef CONFIG_QL_PROJECT_DEF_EC200U
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 3, QUEC_PIN_UART2_RXD, 3},
    {QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4},
#elif defined CONFIG_QL_PROJECT_DEF_EC600U
 #ifndef __QUEC_OEM_VER_LD__
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    //{QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4},
 #else
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    {QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 6, QUEC_PIN_UART3_RXD, 6},
 #endif
#elif defined CONFIG_QL_PROJECT_DEF_EG700U
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    {QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4},
#elif defined CONFIG_QL_PROJECT_DEF_EG500U
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
    //{QL_UART_PORT_3, QUEC_PIN_UART3_TXD, 4, QUEC_PIN_UART3_RXD, 4},
#elif defined CONFIG_QL_PROJECT_DEF_EG915U
    {QL_UART_PORT_2, QUEC_PIN_UART2_TXD, 1, QUEC_PIN_UART2_RXD, 1},
#endif
    {QL_PORT_NONE, -1, -1, -1, -1},
};

const ql_cam_func_s ql_cam_pin_func[] =
{
    {QUEC_PIN_CAM_I2C_SCL   , 0},
    {QUEC_PIN_CAM_I2C_SDA   , 0},
    {QUEC_PIN_CAM_RSTL      , 0},
    {QUEC_PIN_CAM_PWDN      , 0},
    {QUEC_PIN_CAM_REFCLK    , 0},
    {QUEC_PIN_CAM_SPI_DATA0 , 0},
    {QUEC_PIN_CAM_SPI_DATA1 , 0},
    {QUEC_PIN_CAM_SPI_SCK   , 0},
    {QUEC_PIN_NONE, -1},
};

const ql_lcd_func_s ql_lcd_pin_func[] =
{
    {QUEC_PIN_LCD_SPI_SIO , 0},
    {QUEC_PIN_LCD_SPI_SDC , 0},
    {QUEC_PIN_LCD_SPI_CLK , 0},
    {QUEC_PIN_LCD_SPI_CS  , 0},
    {QUEC_PIN_LCD_SPI_SEL , 0},
    {QUEC_PIN_LCD_FMARK   , 0},
    {QUEC_PIN_NONE, -1},
};
    
#ifdef CONFIG_QL_PROJECT_DEF_EC200U_EU_AA
    uint16 quec_gnss_cfg_table[]={0x01,0xffff,0xffff,0x101,0xffff,0x11,0xffff,0x10};
#else
    uint16 quec_gnss_cfg_table[]={0x01,0xffff,0xffff,0xffff,0xffff,0x11,0xffff,0x10};
#endif

/*==================================================================================
 * Power Domain Set
 * description:
 *              Configurable power domain is LCD/SD/SIM1,
 *              corresponding name is V_LCD/V_MMC/V_SIM2 in pinmux table.
 *              if customer have need what change this 3 power domain,
 *              we can alter power value in the following table.
 * parameters:
 *              id(optional power domain): (don't modify!!!)
 *                                         HAL_POWER_VIBR
 *                                         HAL_POWER_LCD
 *                                         HAL_POWER_SD
 *                                         HAL_POWER_SIM1
 *              enabled   : work mode enable power
 *              lp_enabled: sleep mode enable power
 *              mv: power domain value's range(units:mV)
 *                  POWER_LEVEL_1700MV ~ POWER_LEVEL_3200MV
 *                  step: 100MV
 *                  min_V: HAL_POWER_VIBR is: 2.8V
 *                         others         is: 1.6125V
 * matters need attention:
 *              if LCD analog voltage(use LCD func)
 *              comes from the VIBR power domain of module what like EC600U/EG700U,
 *              VIBR power domain must be great than or equal to LCD power domain.
 ===================================================================================*/
const ql_power_domain_s ql_power_domain_table[] =
{  /*       id         enabled  lp_enabled         mv          */
/* VIBR */
#if (defined CONFIG_QL_PROJECT_DEF_EC600U) || (defined CONFIG_QL_PROJECT_DEF_EG700U)
    /* for open series(csdk/ccsdk) customer */
    {  HAL_POWER_VIBR,  true ,   false,    POWER_LEVEL_2800MV  }, //<----- can alter power value when customers need
#else //others analog voltage is customer design
    {  HAL_POWER_NONE,  false,   false,    POWER_LEVEL_UNUSED  },
#endif

/* LCD */
#ifdef __QUEC_OEM_VER_AC__
    {  HAL_POWER_LCD ,  true ,   true ,    POWER_LEVEL_3200MV  },
#elif (defined CONFIG_QUEC_PROJECT_FEATURE_GNSS)
    //select 1.9V power supply for GNSS
    {  HAL_POWER_LCD ,  true ,   true,    POWER_LEVEL_1900MV  },
#else
    /* for open series(csdk/ccsdk) customer */
    {  HAL_POWER_LCD ,  true ,   true,    POWER_LEVEL_1800MV  }, //<----- can alter power value when customers need
#endif

/* SD */
#ifndef CONFIG_QL_OPEN_EXPORT_PKG /* standard series */
 #ifdef __QUEC_OEM_VER_LD__
    {  HAL_POWER_SD  ,  true ,   true ,    POWER_LEVEL_3200MV  },
 #else
  #if (defined CONFIG_QUEC_PROJECT_FEATURE_SDMMC) || (defined CONFIG_QUEC_PROJECT_FEATURE_EMMC)
    {  HAL_POWER_NONE,  false,   false,    POWER_LEVEL_UNUSED  },
  #else /* public */
    {  HAL_POWER_SD  ,  true ,   true ,    POWER_LEVEL_1800MV  },
  #endif
 #endif
#else /* open series */
    /* for open series(csdk/ccsdk) customer */
    {  HAL_POWER_SD  ,  true ,   true ,    POWER_LEVEL_1800MV  }, //<----- can alter power value when customers need
#endif

/* SIM1 */
#ifdef CONFIG_QUEC_PROJECT_FEATURE_QDSIM
    // SIM1 power domain self-adaption when double sim
    {  HAL_POWER_NONE,  false,   false,    POWER_LEVEL_UNUSED  },
#else
 #ifdef CONFIG_QL_PROJECT_DEF_EG915U
    // EG915U is support double sim single standby
    {  HAL_POWER_NONE,  false,   false,    POWER_LEVEL_UNUSED  },
 #else
    /* for open series(csdk/ccsdk) customer */
    {  HAL_POWER_SIM1,  true ,   true ,    POWER_LEVEL_1800MV  }, //<----- can alter power value when customers need
 #endif
#endif

};


