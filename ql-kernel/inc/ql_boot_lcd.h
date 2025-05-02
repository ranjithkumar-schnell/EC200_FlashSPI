/**  @file
  ql_boot_lcd.h

  @brief
  This file is used to define boot lcd api for different Quectel Project.

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
08/12/2020        Neo         Init version
=================================================================*/

#ifndef QL_BOOT_LCD_H
#define QL_BOOT_LCD_H



#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_BOOT_LCD_ERRCODE_BASE (0x8209<<16)

#define LCD_RED     0xf800
#define LCD_GREEN   0x07e0
#define LCD_BLUE    0x001f
        
#define LCD_WHITE   0xffff
#define LCD_BLACK   0x0000
        
#define LCD_YELLOW  0xffe0
#define LCD_PURPLE  0xf81f


/*===========================================================================
 * Enum
 ===========================================================================*/
typedef enum
{
    QL_BOOT_LCD_SUCCESS = 0,

    QL_BOOT_LCD_EXECUTE_ERR                 = 1|QL_BOOT_LCD_ERRCODE_BASE,
    QL_BOOT_LCD_INVALID_PARAM_ERR,
    QL_BOOT_LCD_INIT_ERR,
    QL_BOOT_LCD_BUFFER_CREATE_ERR,
    QL_BOOT_LCD_BLOCK_SET_ERR,
    QL_BOOT_LCD_OPERATION_ERR
}ql_boot_errcode_lcd;



/*===========================================================================
 * Functions declaration
 ===========================================================================*/
 /**
 * @brief Init the LCD moudle
 * @return
 *      QL_BOOT_LCD_INIT_ERR
 *      QL_BOOT_LCD_BUFFER_CREATE_ERR
 *      QL_BOOT_LCD_BLOCK_SET_ERR
 *      QL_BOOT_LCD_SUCCESS
 */
ql_boot_errcode_lcd ql_boot_lcd_init(void);

/**
 * @brief Clear screen background to a kind of color
 * @param color : background color
 * @return
 *      QL_BOOT_LCD_BLOCK_SET_ERR
 *      QL_BOOT_LCD_SUCCESS
 */
ql_boot_errcode_lcd ql_boot_lcd_clear_screen(uint16_t color);

/**
 * @brief LCD part area set
 * @param buffer  : write-in data buffer
 * @param start_x : horizontal coordinate x start location
 * @param start_y : vertical coordinate y start location
 * @param end_x   : horizontal coordinate x end location
 * @param end_x   : vertical coordinate y end location
 * @return
 *      QL_BOOT_LCD_INVALID_PARAM_ERR
 *      QL_BOOT_LCD_BLOCK_SET_ERR
 *      QL_BOOT_LCD_SUCCESS
 */
ql_boot_errcode_lcd ql_boot_lcd_write(uint16_t *buffer, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);

/**
 * @brief LCD display turn on
 * @return
 *      QL_BOOT_LCD_OPERATION_ERR
 *      QL_BOOT_LCD_SUCCESS
 */
ql_boot_errcode_lcd ql_boot_lcd_display_on(void);

/**
 * @brief LCD display turn off
 * @return
 *      QL_BOOT_LCD_OPERATION_ERR
 *      QL_BOOT_LCD_SUCCESS
 */
ql_boot_errcode_lcd ql_boot_lcd_display_off(void);

/**
 * @brief LCD backlight brightness one channel set function
 * @param level_brightness : range is 0~63, one step is 0.84mA
 *                           one channel current: mix is 1.68mA, max is 54.6mA
 *                           there is 4 channels
 *                           total current is (1.68 + 0.84 * level_brightness) * 4 (mA)
 * @return
 *      QL_BOOT_LCD_INVALID_PARAM_ERR
 *      QL_BOOT_LCD_SUCCESS
 */
ql_boot_errcode_lcd ql_boot_lcd_set_level_brightness(uint8_t level_brightness);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_BOOT_LCD_H */


