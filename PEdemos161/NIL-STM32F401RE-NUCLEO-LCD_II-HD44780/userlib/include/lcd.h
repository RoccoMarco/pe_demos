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
#define   LCD_INSTRUCTION_R                        ((uint8_t) 0)
#define   LCD_DATA_R                               ((uint8_t) 1)

/**
 * @brief   HD44780 max diplay data RAM address
 */
#define   LCD_DDRAM_MAX_ADDRESS                    ((uint8_t) 0X4F)

/**
 * @brief   HD44780 other bit masks
 */
#define   LCD_SET_CGRAM_ADDRESS                    ((uint8_t) 1<<6)
#define   LCD_SET_DDRAM_ADDRESS                    ((uint8_t) 1<<7)
#define   LCD_BUSY_FLAG                            ((uint8_t) 1<<7)

/**
 * @brief   Generic definition of right direction
 */
#define   LCD_RIGHT                                           TRUE

/**
 * @brief   Generic definition of left direction
 */
#define    LCD_LEFT                                           FALSE
/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

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

typedef struct {
  /**
   * @brief  port identifier
   */
  ioportid_t port;
  /**
   * @brief  pad identifier
   */
  uint8_t pad;
}HD44780_pin_t;

typedef struct {
  /**
   * @brief  Register selector pin
   */
  HD44780_pin_t RS;
  /**
   * @brief  Read/Write pin
   */
  HD44780_pin_t RW;
  /**
   * @brief  Enabler pin
   */
  HD44780_pin_t E;
  /**
   * @brief  Data pin
   *
   * @detail Data pin from 0 to 3 should be NULL in 4 bit mode
   */
  HD44780_pin_t Data[8];
  /**
   * @brief  Back-light anode pin
   */
  HD44780_pin_t A;
}HD44780_pins_t;

/**
 * @brief  LCD commands
 */
typedef enum {
  HD44780_CMD_Clear = 0x01,              /*!< Clears entire display and sets DDRAM address 0 in address counter */
  HD44780_CMD_Return = 0x02,             /*!< Sets DDRAM address counter to 0 and returns display to original position */
}HD44780_CMD_t;

/**
 * @brief  LCD entry mode set
 */
typedef enum {
  HD44780_EMS = 0x04,                    /*!< Entry mode set bit */
  HD44780_EMS_Dec = 0x00,                /*!< Sets cursor move direction as decreasing mode */
  HD44780_EMS_DecAndShift = 0x01,        /*!< Sets cursor move direction as decreasing mode and accompanies display shift */
  HD44780_EMS_Inc = 0x02,                /*!< Sets cursor move direction as increasing mode */
  HD44780_EMS_IncAndShift = 0x03,        /*!< Sets cursor move direction as increasing mode and accompanies display shift */
}HD44780_EMS_t;

/**
 * @brief  LCD display control
 */
typedef enum {
  HD44780_DC = 0x08,                     /*!< Display control bit */
  HD44780_DC_DisplayOff = 0x00,          /*!< Sets entire display off */
  HD44780_DC_DisplayOn = 0x04,           /*!< Sets entire display on */
  HD44780_DC_CursorOff = 0x00,           /*!< Sets entire cursor off */
  HD44780_DC_CursorOn = 0x02,            /*!< Sets entire display on */
  HD44780_DC_BlinkingOff = 0x00,         /*!< Sets entire blinking off */
  HD44780_DC_BlinkingOn = 0x01,          /*!< Sets entire blinking on */
}HD44780_DC_t;

/**
 * @brief  LCD display cursor or display shift
 */
typedef enum {
  HD44780_CODS = 0x10,                    /*!< Cursor or display shift bit. */
  HD44780_CODS_CursorLeft = 0x00,         /*!< Moves cursor to the left without changing DDRAM contents. */
  HD44780_CODS_CursorRight = 0x04,        /*!< Moves cursor to the right without changing DDRAM contents. */
  HD44780_CODS_ShiftLeft = 0x08,          /*!< Shifts display to the left without changing DDRAM contents. */
  HD44780_CODS_ShiftRight = 0x0C,         /*!< Shifts display to the right without changing DDRAM contents. */
}HD44780_CODS_t;

/**
 * @brief  LCD display settings
 */
typedef enum {
  HD44780_Set = 0x20,                     /*!< Display setting bit. */
  HD44780_Set_Font5x8Dots = 0x00,         /*!< Sets character font to 5x8 dots. */
  HD44780_Set_Font5x10Dots = 0x04,        /*!< Sets character font to 5x10 dots. */
  HD44780_Set_1Line = 0x00,               /*!< Sets number of display lines to 1. */
  HD44780_Set_2Lines = 0x08,              /*!< Sets number of display lines to 2. */
  HD44780_Set_DataLenght4bit = 0x00,      /*!< Sets interface data length to 4 bit. */
  HD44780_Set_DataLenght8bit = 0x10,      /*!< Sets interface data length to 8 bit. */
}HD44780_Set_t;
/** @}  */

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  LCD_UNINIT = 0,                   /**< Not initialized.                   */
  LCD_STOP = 1,                     /**< Stopped.                           */
  LCD_ACTIVE = 2,                   /**< Showing data.                      */
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
   *
   * @detail If !LCD_USE_BACKLIGHT is considered like boolean
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
