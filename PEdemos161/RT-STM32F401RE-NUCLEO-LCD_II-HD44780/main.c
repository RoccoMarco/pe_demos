/*
    PLAY Embedded demos - Copyright (C) 2014-2016 Rocco Marco Guglielmi

    This file is part of PLAY Embedded demos.

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
    Tested under ChibiOS/RT 3.0.1, Project version 1.0
 */
#include "ch.h"
#include "hal.h"

#include "lcd.h"

static uint8_t ii;

/*===========================================================================*/
/* LCD configuration                                                         */
/*===========================================================================*/
static const PWMConfig pwmcfg = {
  100000,                                   /* 100kHz PWM clock frequency.  */
  100,                                      /* PWM period is 1000 cycles.    */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
};

 static const LCDConfig lcdcfg = {

  {
   {GPIOA, GPIOA_PIN4},                      /* RS Port and pin */
   {GPIOA, GPIOA_PIN1},                      /* RW Port and pin */
   {GPIOA, GPIOA_PIN0},                      /* E Port and pin */
   {
    {GPIOC, GPIOC_PIN0},
    {GPIOC, GPIOC_PIN1},
    {GPIOC, GPIOC_PIN2},
    {GPIOC, GPIOC_PIN3},
    {GPIOC, GPIOC_PIN4},
    {GPIOC, GPIOC_PIN5},
    {GPIOC, GPIOC_PIN6},
    {GPIOC, GPIOC_PIN7} /* Data Ports and pins */
   },
   {GPIOA, GPIOA_PIN8},                      /* A Port and pin */
  },
  HD44780_EMS_Inc,
  HD44780_DC_DisplayOn | HD44780_DC_CursorOff | HD44780_DC_BlinkingOff,
  HD44780_Set_Font5x10Dots | HD44780_Set_2Lines | HD44780_Set_DataLenght8bit,
  &PWMD1,                                    /* PWM Driver for back-light */
  &pwmcfg,                                   /* PWM driver configuration for back-light */
  0,                                         /* PWM channel */
  100,                                       /* Back-light */
};

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("LED blinker");
  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;
  chRegSetThreadName("Back_light handler");
  while (true) {
    if(palReadPad(GPIOC, GPIOC_BUTTON)){
      chThdSleepMilliseconds(50);
      if(!palReadPad(GPIOC, GPIOC_BUTTON)){
        if(LCDD1.backlight > 0)
          lcdBacklightFadeOut(&LCDD1);
        else
          lcdBacklightFadeIn(&LCDD1);
      }
    }
    chThdSleepMilliseconds(10);
  }
}
/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
  lcdInit();

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  /*
   * Setting RS, RW, E and A pin mode. Data pin are set internally since they
   * could be used as input or output.
   */
  palSetPadMode(GPIOA, GPIOA_PIN8, PAL_MODE_ALTERNATE(1));
  palSetPadMode(GPIOA, GPIOA_PIN0, PAL_MODE_OUTPUT_PUSHPULL |
                PAL_STM32_OSPEED_HIGHEST);
  palSetPadMode(GPIOA, GPIOA_PIN1, PAL_MODE_OUTPUT_PUSHPULL |
                PAL_STM32_OSPEED_HIGHEST);
  palSetPadMode(GPIOA, GPIOA_PIN4, PAL_MODE_OUTPUT_PUSHPULL |
                PAL_STM32_OSPEED_HIGHEST);
  chThdSleepMilliseconds(100);
  lcdStart(&LCDD1, &lcdcfg);
  lcdWriteString(&LCDD1, "PLAY            Learn", 0);
  lcdWriteString(&LCDD1, "Embedded        by doing",40);
  chThdSleepMilliseconds(2000);
  while (true) {
    for(ii = 0; ii < 16; ii++){
      lcdDoDisplayShift(&LCDD1, LCD_LEFT);
      chThdSleepMilliseconds(50);
    }
    chThdSleepMilliseconds(2000);
    for(ii = 0; ii < 16; ii++){
      lcdDoDisplayShift(&LCDD1, LCD_RIGHT);
      chThdSleepMilliseconds(50);
    }
    chThdSleepMilliseconds(2000);
  }
}
