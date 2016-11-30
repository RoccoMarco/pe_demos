/*
    PLAY Embedded - Copyright (C) 2006..2015 Rocco Marco Guglielmi

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
    Special thanks to Giovanni Di Sirio for teachings, his moral support and
    friendship. Note that some or every piece of this file could be part of
    the ChibiOS project that is intellectual property of Giovanni Di Sirio.
    Please refer to ChibiOS/RT license before use this file.
	
	For suggestion or Bug report - guglielmir@playembedded.org
 */

/**
 * @file    lcd.h
 * @brief   LCD Complex Driver header.
 *
 * @addtogroup LCD
 * @{
 */

#ifndef _LCD_H_
#define _LCD_H_

#include "hal.h"
#include "userconf.h"

#if USERLIB_USE_LCD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   HD44780 registers
 */
#define   LCD_INSTRUCTION_R             0x00
#define   LCD_DATA_R                    0x01

/**
 * @brief   HD44780 max diplay data RAM address
 */
#define   LCD_DDRAM_MAX_ADDRESS         0X4F

/**
 * @brief   HD44780 other bit masks
 */
#define   LCD_SET_CGRAM_ADDRESS         1 << 6
#define   LCD_SET_DDRAM_ADDRESS         1 << 7
#define   LCD_BUSY_FLAG                 1 << 7

/**
 * @brief   Generic definition of right direction
 */
#define   LCD_RIGHT                     TRUE

/**
 * @brief   Generic definition of left direction
 */
#define    LCD_LEFT                     FALSE

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Enables back-light APIs.
 *
 * @note    The default is @p TRUE.
 * @note    Enabling this option LCD requires a PWM driver.
 *
 */
#if !defined(LCD_USE_BACKLIGHT) || defined(__DOXYGEN__)
#define LCD_USE_BACKLIGHT               TRUE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if LCD_USE_BACKLIGHT && !HAL_USE_PWM
#error "LCD_USE_BACKLIGHT  requires HAL_USE_PWM"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @name    HD44780 data structures and types
 * @{ */

/**
 * @brief   HD44780 PIN-map;
 */
typedef struct {
  /**
   * @brief  Register selector PIN
   */
  ioline_t RS;
  /**
   * @brief  Read/Write PIN
   */
  ioline_t RW;
  /**
   * @brief  Enable PIN
   */
  ioline_t E;
  /**
   * @brief  Data PINs
   * @note   In four line mode PINs from D0 to D3 are ignored.
   */
  ioline_t Data[8];
  /**
   * @brief  Back-light anode pin
   */
  ioline_t A;
} HD44780_pins_t;

/**
 * @brief  LCD commands
 */
typedef enum {
  HD44780_CMD_Clear = 0x01,
  HD44780_CMD_Return = 0x02,
} HD44780_CMD_t;

/**
 * @brief  LCD entry mode set
 */
typedef enum {
  HD44780_EMS = 0x04,
  HD44780_EMS_Dec = 0x00,
  HD44780_EMS_DecAndShift = 0x01,
  HD44780_EMS_Inc = 0x02,
  HD44780_EMS_IncAndShift = 0x03,
} HD44780_EMS_t;

/**
 * @brief  LCD display control
 */
typedef enum {
  HD44780_DC = 0x08,
  HD44780_DC_DisplayOff = 0x00,
  HD44780_DC_DisplayOn = 0x04,
  HD44780_DC_CursorOff = 0x00,
  HD44780_DC_CursorOn = 0x02,
  HD44780_DC_BlinkingOff = 0x00,
  HD44780_DC_BlinkingOn = 0x01,
} HD44780_DC_t;

/**
 * @brief  LCD display cursor or display shift
 */
typedef enum {
  HD44780_CODS = 0x10,
  HD44780_CODS_CursorLeft = 0x00,
  HD44780_CODS_CursorRight = 0x04,
  HD44780_CODS_ShiftLeft = 0x08,
  HD44780_CODS_ShiftRight = 0x0C,
} HD44780_CODS_t;

/**
 * @brief  LCD display settings
 */
typedef enum {
  HD44780_Set = 0x20,
  HD44780_Set_Font5x8Dots = 0x00,
  HD44780_Set_Font5x10Dots = 0x04,
  HD44780_Set_1Line = 0x00,
  HD44780_Set_2Lines = 0x08,
  HD44780_Set_DataLenght4bit = 0x00,
  HD44780_Set_DataLenght8bit = 0x10,
} HD44780_Set_t;
/** @}  */

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  LCD_UNINIT = 0,
  LCD_STOP = 1,
  LCD_ACTIVE = 2,
} lcd_state_t;

/**
 * @brief   LCD configuration structure.
 */
typedef struct {
  /**
   * @brief  Pin map
   */
  HD44780_pins_t pins;
  /**
   * @brief  LCD entry mode set
   */
  HD44780_EMS_t entry_mode_set;
  /**
   * @brief  LCD display control
   */
  HD44780_DC_t display_control;
  /**
   * @brief  LCD display settings
   */
  HD44780_Set_t settings;
#if LCD_USE_BACKLIGHT
  /**
   * @brief  PWM driver for back-light managing
   */
  PWMDriver *pwmp;
  /**
   * @brief  PWM driver configuration
   */
  PWMConfig const *pwmcfgp;
  /**
   * @brief  PWM channel id
   */
  uint32_t channelid;
#endif
  /**
   * @brief  Initial Back-light percentage (from 0 to 100)
   * @note   If !LCD_USE_BACKLIGHT this is just true or false
   */
  uint32_t backlight;
} LCDConfig;

/**
 * @brief   Structure representing an LCD driver.
 */
typedef struct {
  /**
   * @brief Driver state.
   */
  lcd_state_t         state;
  /**
   * @brief  Current Back-light percentage (from 0 to 100)
   *
   * @detail When LCD_USE_BACKLIGHT is false, this is considered like boolean
   */
  uint32_t           backlight;
  /**
   * @brief Current configuration data.
   */
  const LCDConfig    *config;
} LCDDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern LCDDriver LCDD1;

#ifdef __cplusplus
extern "C" {
#endif
  void lcdInit(void);
  void lcdObjectInit(LCDDriver *lcdp);
  void lcdStart(LCDDriver *lcdp, const LCDConfig *config);
  void lcdStop(LCDDriver *lcdp);
  void lcdBacklightOn(LCDDriver *lcdp);
  void lcdBacklightOff(LCDDriver *lcdp);
  void lcdClearDisplay(LCDDriver *lcdp);
  void lcdReturnHome(LCDDriver *lcdp);
  void lcdSetAddress(LCDDriver *lcdp, uint8_t add);
  void lcdWriteString(LCDDriver *lcdp, char* string, uint8_t pos);
  void lcdDoDisplayShift(LCDDriver *lcdp, bool dir);
#if LCD_USE_BACKLIGHT
  void lcdSetBacklight(LCDDriver *lcdp, uint32_t perc);
  void lcdBacklightFadeOut(LCDDriver *lcdp);
  void lcdBacklightFadeIn(LCDDriver *lcdp);
#endif
#ifdef __cplusplus
}
#endif
#endif /* USERLIB_USE_LCD */
#endif /* _LCD_H_ */

/** @} */
