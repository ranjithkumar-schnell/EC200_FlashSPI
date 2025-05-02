/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/

#ifndef QUEC_CUSTOMER_CFG_H
#define QUEC_CUSTOMER_CFG_H

#include "quec_common.h"
#include "ql_uart.h"
#include "ql_audio.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * Macro
 ===========================================================================*/
#if defined CONFIG_QL_PROJECT_DEF_EG500U || defined CONFIG_QL_PROJECT_DEF_EG915U
#define UART_PIN_MAX  (1)  //EG500U/EG915U only UART2 //different uart num, so don't used in core code directly
#elif defined CONFIG_QL_PROJECT_DEF_EC600U
 #ifndef __QUEC_OEM_VER_LD__
#define UART_PIN_MAX  (1)  //EC600U only UART2 //different uart num, so don't used in core code directly
 #else
#define UART_PIN_MAX  (2)  //UART2 UART3 //different uart num, so don't used in core code directly
 #endif
#else
#define UART_PIN_MAX  (2)  //UART2 UART3 //different uart num, so don't used in core code directly
#endif
#define CAM_PIN_MAX   (8)  //fixed pin num
#define LCD_PIN_MAX   (6)  //fixed pin num

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct
{
    uint8_t        adc_num;
    uint8_t        sdmmc_enabled;
    uint8_t        ex_uart_num;
    uint8_t        pin_num; //=QUEC_PIN_CFG_MAX
    AudInputType_e hp_input_type;
    bool           dual_sim_single_standby;
}ql_model_diff_ctx_s;

typedef struct
{
    unsigned int        ap_addr; 
    unsigned int        ap_size; 
    unsigned int        appimg_addr; 
    unsigned int        appimg_size; 
    unsigned int        fs_addr; 
    unsigned int        fs_size;
    unsigned int        fs_modem_addr;
    unsigned int        fs_modem_size;
}ql_model_partinfo_ctx_s;

typedef struct
{
    bool volte_enabled;
    bool dual_sim_enabled;
    uint8_t sim_count;
    uint32_t app_ram_offset;
    uint32_t app_total_ram_size;
    uint8_t ap_ifc_dma_count;           //ap can use ifc dma channel number
} ql_model_feature_ctx_s;

typedef struct
{
    ql_uart_port_number_e port;
    uint8_t tx_pin;
    uint8_t tx_func;
    uint8_t rx_pin;
    uint8_t rx_func;
} ql_uart_func_s;

typedef struct
{
    uint8_t pin;
    uint8_t func;
} ql_cam_func_s;

typedef ql_cam_func_s ql_lcd_func_s;

/**
 * Power domain set
 */
typedef struct
{
    uint32_t id;          //configurable power domain
    bool     enabled;     //work mode
    bool     lp_enabled;  //sleep mode
    uint32_t mv;          //power domain value(units:mV)
} ql_power_domain_s;


/*===========================================================================
 * Extern
 ===========================================================================*/
extern const ql_model_diff_ctx_s ql_model_diff_ctx;
extern const ql_model_partinfo_ctx_s ql_model_partinfo_ctx;
extern const ql_model_feature_ctx_s ql_model_feature_ctx;
extern const ql_uart_func_s ql_uart_pin_func[];
extern const ql_cam_func_s ql_cam_pin_func[];
extern const ql_lcd_func_s ql_lcd_pin_func[];
extern const ql_power_domain_s ql_power_domain_table[];


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_CUSTOMER_CFG_H */

