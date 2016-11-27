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
 * @file    lcd.c
 * @brief   LCD complex driver code.
 *
 * @addtogroup LCD
 * @{
 */

#include "lcd.h"
#if USERLIB_USE_LCD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   LCDD1 driver identifier.
 */
LCDDriver LCDD1;

/*===========================================================================*/
/* Driver local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Sets pin for write
 *
 * @param[in] pins          HD44780 pins structure
 * @param[in] sets           HD44780 sets field
 */
static void hd44780SetForWrite(HD44780_pins_t pins, HD44780_Set_t sets){
  unsigned ii;
  if(sets & HD44780_Set_DataLenght8bit)
    for(ii = 0; ii < 8; ii++)
      if(pins.Data[ii].port != NULL)
        palSetPadMode(pins.Data[ii].port, pins.Data[ii].pad,
                      PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
  else
    for(ii = 4; ii < 8; ii++)
      palSetPadMode(pins.Data[ii].port, pins.Data[ii].pad,
                    PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

}

/**
 * @brief   Write a data into a register for the lcd
 *
 * @param[in] pins          HD44780 pins structure
 * @param[in] sets          HD44780 sets field
 * @param[in] reg           Register id
 * @param[in] value         Writing value
 */
static void hd44780WriteRegister(HD44780_pins_t pins, HD44780_Set_t sets,
                                 uint8_t reg, uint8_t value){

  unsigned ii;
  palWritePad(pins.RW.port, pins.RW.pad, PAL_LOW);
  if(reg == LCD_INSTRUCTION_R){
    palWritePad(pins.RS.port, pins.RS.pad, PAL_LOW);
  }
  else{
    palWritePad(pins.RS.port, pins.RS.pad, PAL_HIGH);
  }
  hd44780SetForWrite(pins, sets);

  if(sets & HD44780_Set_DataLenght8bit){

    for(ii = 0; ii < 8; ii++){
      if(pins.Data[ii].port != NULL){
        if(value & (1 << ii))
          palSetPad(pins.Data[ii].port, pins.Data[ii ].pad);
        else
          palClearPad(pins.Data[ii].port, pins.Data[ii].pad);
      }
    }
    palWritePad(pins.E.port, pins.E.pad, PAL_HIGH);
    osalThreadSleepMilliseconds(1);
    palWritePad(pins.E.port, pins.E.pad, PAL_LOW);
    osalThreadSleepMilliseconds(1);
  }
  else{
    for(ii = 0; ii < 4; ii++){
      if(value & (1 << (ii + 4)))
        palSetPad(pins.Data[(ii + 4)].port, pins.Data[(ii + 4)].pad);
      else
        palClearPad(pins.Data[(ii + 4)].port, pins.Data[(ii + 4)].pad);
    }
    palWritePad(pins.E.port, pins.E.pad, PAL_HIGH);
    osalThreadSleepMilliseconds(1);
    palWritePad(pins.E.port, pins.E.pad, PAL_LOW);
    osalThreadSleepMilliseconds(1);
    for(ii = 0; ii < 4; ii++){
      if(value & (1 << ii))
        palSetPad(pins.Data[(ii + 4)].port, pins.Data[(ii + 4)].pad);
      else
        palClearPad(pins.Data[(ii + 4)].port, pins.Data[(ii + 4)].pad);
    }
    palWritePad(pins.E.port, pins.E.pad, PAL_HIGH);
    osalThreadSleepMilliseconds(1);
    palWritePad(pins.E.port, pins.E.pad, PAL_LOW);
    osalThreadSleepMilliseconds(1);
  }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   LCD Complex Driver initialization.
 * @note    This function is implicitly invoked by @p playInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void lcdInit(void) {

  /* Object initialization */
  lcdObjectInit(&LCDD1);
}

/**
 * @brief   Initializes an instance.
 *
 * @param[out] lcdp         pointer to the @p LCDDriver object
 *
 * @init
 */
void lcdObjectInit(LCDDriver *lcdp){

  lcdp->state  = LCD_STOP;
  lcdp->config = NULL;
  lcdp->backlight = 0;
}

/**
 * @brief   Configures and activates the LCD Complex Driver  peripheral.
 *
 * @param[in] lcd   pointer to the @p LCDDriver object
 * @param[in] config    pointer to the @p LCDConfig object
 *
 * @api
 */
void lcdStart(LCDDriver *lcdp, const LCDConfig *config) {

  osalDbgCheck((lcdp != NULL) && (config != NULL));

  osalDbgAssert((lcdp->state == LCD_STOP) || (lcdp->state == LCD_ACTIVE),
              "lcdStart(), invalid state");
  lcdp->config = config;
#if LCD_USE_BACKLIGHT
  pwmStart(lcdp->config->pwmp, lcdp->config->pwmcfgp);
  pwmEnableChannel(lcdp->config->pwmp, lcdp->config->channelid,
                   PWM_PERCENTAGE_TO_WIDTH(lcdp->config->pwmp,
                                           lcdp->config->backlight * 100));

#else
  palWritePad(lcdp->config->pins.A.port, lcdp->config->pins.A.pad,
              lcdp->config->backlight ? PAL_HIGH : PAL_LOW);
#endif
  lcdp->backlight = lcdp->config->backlight;
  hd44780WriteRegister(lcdp->config->pins, HD44780_Set_DataLenght8bit,
                       LCD_INSTRUCTION_R,
                       HD44780_Set | HD44780_Set_DataLenght8bit);
  osalThreadSleepMilliseconds(5);
  hd44780WriteRegister(lcdp->config->pins, HD44780_Set_DataLenght8bit,
                       LCD_INSTRUCTION_R,
                       HD44780_Set | HD44780_Set_DataLenght8bit);
  osalThreadSleepMilliseconds(1);
  hd44780WriteRegister(lcdp->config->pins, HD44780_Set_DataLenght8bit,
                       LCD_INSTRUCTION_R,
                       HD44780_Set | HD44780_Set_DataLenght8bit);
  if(!(lcdp->config->settings & HD44780_Set_DataLenght8bit)){
    hd44780WriteRegister(lcdp->config->pins, HD44780_Set_DataLenght8bit,
                         LCD_INSTRUCTION_R,
                         HD44780_Set | HD44780_Set_DataLenght4bit);
  }
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R,
                       HD44780_Set | lcdp->config->settings);
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R, HD44780_DC);
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R, HD44780_CMD_Clear);
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R,
                       HD44780_EMS | lcdp->config->entry_mode_set);
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R,
                       HD44780_DC | lcdp->config->display_control);
  lcdp->state = LCD_ACTIVE;
}

/**
 * @brief   Deactivates the LCD Complex Driver  peripheral.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 *
 * @api
 */
void lcdStop(LCDDriver *lcdp) {

  osalDbgCheck(lcdp != NULL);

  osalDbgAssert((lcdp->state == LCD_STOP) || (lcdp->state == LCD_ACTIVE),
              "lcdStop(), invalid state");
#if LCD_USE_BACKLIGHT
  pwmStop(lcdp->config->pwmp);
#else
  palWritePad(lcdp->config->pins.A.port, lcdp->config->pins.A.pad, PAL_LOW);
#endif
  lcdp->backlight = 0;
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R, HD44780_CMD_Clear);
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R,
                       HD44780_DC | HD44780_DC_DisplayOff);
  lcdp->state = LCD_STOP;
}

/**
 * @brief   Turn on back-light.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 *
 * @api
 */
void lcdBacklightOn(LCDDriver *lcdp) {

  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE), "lcdBacklightOn(), invalid state");
#if LCD_USE_BACKLIGHT
  pwmEnableChannel(lcdp->config->pwmp, lcdp->config->channelid,
                   PWM_PERCENTAGE_TO_WIDTH(lcdp->config->pwmp, 10000));

#else
  palWritePad(lcdp->config->pins.A.port, lcdp->config->pins.A.pad, PAL_HIGH);
#endif
  lcdp->backlight = 100;
}

/**
 * @brief   Turn off back-light.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 *
 * @api
 */
void lcdBacklightOff(LCDDriver *lcdp) {

  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE), "lcdBacklightOff(), invalid state");
#if LCD_USE_BACKLIGHT
  pwmDisableChannel(lcdp->config->pwmp, lcdp->config->channelid);

#else
  palWritePad(lcdp->config->pins.A.port, lcdp->config->pins.A.pad, PAL_LOW);
#endif
  lcdp->backlight = 0;
}

/**
 * @brief   Clears display and return cursor in the first position.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 *
 * @api
 */
void lcdClearDisplay(LCDDriver *lcdp){

  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE), "lcdClearDisplay(), invalid state");
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings, LCD_INSTRUCTION_R, HD44780_CMD_Clear);
}

/**
 * @brief   Return cursor in the first position.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 *
 * @api
 */
void lcdReturnHome(LCDDriver *lcdp){

  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE), "lcdReturnHome(), invalid state");
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings, LCD_INSTRUCTION_R, HD44780_CMD_Return);
}

/**
 * @brief   Set DDRAM address position leaving data unchanged.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 * @param[in] add       DDRAM address (from 0 to LCD_DDRAM_MAX_ADDRESS)
 *
 * @api
 */
void lcdSetAddress(LCDDriver *lcdp, uint8_t add){

  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE),
                "lcdSetAddress(), invalid state");
  if(add > LCD_DDRAM_MAX_ADDRESS)
    return;
  hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                       LCD_INSTRUCTION_R, LCD_SET_DDRAM_ADDRESS | add);
}

/**
 * @brief   Writes string starting from a certain position.
 *
 * @detail  If string lenght exceeds, then is cutted
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 * @param[in] string    string to write
 * @param[in] pos       position for cursor (from 0 to LCD_DDRAM_MAX_ADDRESS)
 *
 * @api
 */
void lcdWriteString(LCDDriver *lcdp, char* string, uint8_t pos){
  int32_t iteration;
  osalDbgCheck((lcdp != NULL) && (string != NULL));
  osalDbgAssert((lcdp->state == LCD_ACTIVE),
                "lcdWriteString(), invalid state");

  iteration = LCD_DDRAM_MAX_ADDRESS - pos + 1;
  if(iteration > 0){
    lcdSetAddress(lcdp, pos);
    while((*string != '\0') && (iteration > 0)){
      hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings,
                           LCD_DATA_R, *string);
      string++;
      iteration--;
    }
  }
  else
    return;
}

/**
 * @brief   Makes a shift according to an arbitrary direction
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 * @param[in] dir       direction (LCD_RIGHT or LCD_LEFT)
 *
 * @api
 */
void lcdDoDisplayShift(LCDDriver *lcdp, bool dir){

  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE),
                "lcdDoDisplayShift(), invalid state");
  if(dir == LCD_RIGHT){
    hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings, LCD_INSTRUCTION_R,
                         HD44780_CODS | HD44780_CODS_ShiftRight);
  }
  else{
    hd44780WriteRegister(lcdp->config->pins, lcdp->config->settings, LCD_INSTRUCTION_R,
                         HD44780_CODS | HD44780_CODS_ShiftLeft);
  }
}

#if LCD_USE_BACKLIGHT
/**
 * @brief   Set back-light percentage.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 * @param[in] perc      back-light percentage (from 0 to 100)
 *
 * @api
 */
void lcdSetBacklight(LCDDriver *lcdp, uint32_t perc){

  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE), "lcdSetBacklight(), invalid state");
  pwmEnableChannel(lcdp->config->pwmp, lcdp->config->channelid,
                   PWM_PERCENTAGE_TO_WIDTH(lcdp->config->pwmp, perc * 100));
  lcdp->backlight = perc;
}

/**
 * @brief   Shift back-light from current value to 0.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 *
 * @api
 */
void lcdBacklightFadeOut(LCDDriver *lcdp){
  uint32_t curr = lcdp->backlight;
  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE),
                "lcdBacklightFadeOut(), invalid state");
  while(curr != 0){
    curr--;
    lcdSetBacklight(lcdp, curr);
    osalThreadSleepMilliseconds(10);
  }
}

/**
 * @brief   Shift back-light from current value to 100.
 *
 * @param[in] lcdp      pointer to the @p LCDDriver object
 *
 * @api
 */
void lcdBacklightFadeIn(LCDDriver *lcdp){
  uint32_t curr = lcdp->backlight;
  osalDbgCheck(lcdp != NULL);
  osalDbgAssert((lcdp->state == LCD_ACTIVE),
                "lcdBacklightFadeIn(), invalid state");
  while(curr != 100){
    curr++;
    lcdSetBacklight(lcdp, curr);
    osalThreadSleepMilliseconds(10);
  }
}
#endif
#endif /* USERLIB_USE_LCD */

/** @} */
