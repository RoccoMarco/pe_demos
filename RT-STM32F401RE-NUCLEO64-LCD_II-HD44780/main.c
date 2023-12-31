/*
    PLAY Embedded demos - Copyright (C) 2014...2019 Rocco Marco Guglielmi

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

#include "ch.h"
#include "hal.h"

#include "lcd.h"

#define LINE_RS                     PAL_LINE(GPIOA, 4U)
#define LINE_RW                     PAL_LINE(GPIOA, 1U)
#define LINE_E                      PAL_LINE(GPIOA, 0U)
#define LINE_A                      PAL_LINE(GPIOA, 8U)

/* Data PIN are connected from PC0 to PC7 */
#if !LCD_USE_4_BIT_MODE
#define LINE_D0                     PAL_LINE(GPIOC, 0U)
#define LINE_D1                     PAL_LINE(GPIOC, 1U)
#define LINE_D2                     PAL_LINE(GPIOC, 2U)
#define LINE_D3                     PAL_LINE(GPIOC, 3U)
#endif
#define LINE_D4                     PAL_LINE(GPIOC, 4U)
#define LINE_D5                     PAL_LINE(GPIOC, 5U)
#define LINE_D6                     PAL_LINE(GPIOC, 6U)
#define LINE_D7                     PAL_LINE(GPIOC, 7U)

/*===========================================================================*/
/* LCD configuration                                                         */
/*===========================================================================*/

#if LCD_USE_DIMMABLE_BACKLIGHT
static const PWMConfig pwmcfg = {
  100000,                                   /* 100kHz PWM clock frequency.   */
  100,                                      /* PWM period is 1000 cycles.    */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0,
  0
};
#endif

static const lcd_pins_t lcdpins = {
  LINE_RS,
  LINE_RW,
  LINE_E,
  LINE_A,
  {
#if !LCD_USE_4_BIT_MODE
    LINE_D0,
    LINE_D1,
    LINE_D2,
    LINE_D3,
#endif
    LINE_D4,
    LINE_D5,
    LINE_D6,
    LINE_D7
  }
};

static const LCDConfig lcdcfg = {
  LCD_CURSOR_OFF,                           /* Cursor disabled               */
  LCD_BLINKING_OFF,                         /* Blinking disabled             */
  LCD_SET_FONT_5X10,                        /* Font 5x10                     */
  LCD_SET_2LINES,                           /* 2 lines                       */
  &lcdpins,                                 /* Pin map                       */
#if LCD_USE_DIMMABLE_BACKLIGHT
  &PWMD1,                                   /* PWM Driver for back-light     */
  &pwmcfg,                                  /* PWM driver cfg for back-light */
  0,                                        /* PWM channel                   */
#endif                                      
  100,                                      /* Back-light                    */
};

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("LED blinker");
  while (true) {
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}

/*
 * Backlight handler.
 */
static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;
  chRegSetThreadName("Backlight handler");
  /* Configuring Anode PIN as TIM1_CH1 or GPIO depending on configuration. */
#if LCD_USE_DIMMABLE_BACKLIGHT
  palSetLineMode(LINE_A, PAL_MODE_ALTERNATE(1));
#else
  palSetLineMode(LINE_A, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
#endif

  /* Enabling event on falling edge of LINE_BUTTON signal.*/
  palEnableLineEvent(LINE_BUTTON, PAL_EVENT_MODE_FALLING_EDGE);

  /* Performing shift continuously. */
  while (true) {

    /* Waiting for an event on LINE_BUTTON edge.*/
    palWaitLineTimeout(LINE_BUTTON, TIME_INFINITE);

    /* Action depends on current backlight and LCD configuration. */
    if(LCDD1.backlight > 0) {
#if LCD_USE_DIMMABLE_BACKLIGHT
      lcdBacklightFadeOut(&LCDD1);
#else
      lcdBacklightOff(&LCDD1);
#endif
    }
    else {
#if LCD_USE_DIMMABLE_BACKLIGHT
      lcdBacklightFadeIn(&LCDD1);
#else
      lcdBacklightOn(&LCDD1);
#endif
    }
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

  /* Creating blinker thread and backlight thread. */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);

  /* Initializing LDC driver. */
  lcdInit();

  /* Configuring RW, RS and E PIN as Output Push Pull. */
  palSetLineMode(LINE_RW, PAL_MODE_OUTPUT_PUSHPULL |
                PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(LINE_RS, PAL_MODE_OUTPUT_PUSHPULL |
                PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(LINE_E, PAL_MODE_OUTPUT_PUSHPULL |
                PAL_STM32_OSPEED_HIGHEST);

  /* Starting LDC driver. */
  lcdStart(&LCDD1, &lcdcfg);

  /* Writing some default strings. */
  lcdWriteString(&LCDD1, "PLAY            Learn", 0);
  lcdWriteString(&LCDD1, "Embedded        by doing",40);
  chThdSleepMilliseconds(2000);

  /* Performing shift continuously. */
  while (true) {
    unsigned ii;
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
