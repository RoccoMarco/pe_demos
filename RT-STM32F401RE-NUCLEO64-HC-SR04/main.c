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

#include "chprintf.h"

#define LINE_TRIGGER                PAL_LINE(GPIOB, 10U)
#define LINE_ECHO                   PAL_LINE(GPIOA, 8U)
/* Enable if your terminal supports ANSI ESCAPE CODE */
#define ANSI_ESCAPE_CODE_ALLOWED    TRUE

static BaseSequentialStream * chp = (BaseSequentialStream*) &SD2;

/*===========================================================================*/
/* ICU related code                                                          */
/*===========================================================================*/

#define ICU_TIM_FREQ                1000000
#define M_TO_CM                     100.0f
#define SPEED_OF_SOUND              343.2f

static float lastdistance = 0.0;

static void icuwidthcb(ICUDriver *icup) {

  icucnt_t width = icuGetWidthX(icup);
  lastdistance = (SPEED_OF_SOUND * width * M_TO_CM) / (ICU_TIM_FREQ * 2);
}

static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  ICU_TIM_FREQ,                                /* 1MHz ICU clock frequency.   */
  icuwidthcb,
  NULL,
  NULL,
  ICU_CHANNEL_1,
  0,
  0xFFFFFFFFU
};

/*===========================================================================*/
/* Generic code                                                              */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
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

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /* Initializes the ICU driver 1. The ICU input i PA8. */
  palSetLineMode(LINE_ECHO, PAL_MODE_ALTERNATE(1));
  icuStart(&ICUD1, &icucfg);
  icuStartCapture(&ICUD1);
  icuEnableNotifications(&ICUD1);

  palSetLineMode(LINE_TRIGGER, PAL_MODE_OUTPUT_PUSHPULL);

  while (true) {
    /* Triggering */
    palWriteLine(LINE_TRIGGER, PAL_HIGH);
    chThdSleepMicroseconds(10);
    palWriteLine(LINE_TRIGGER, PAL_LOW);
#if ANSI_ESCAPE_CODE_ALLOWED
    chprintf(chp, "\033[2J\033[1;1H");
#endif
    chprintf(chp, "Distance: %.2f cm\n\r", lastdistance);
    chThdSleepMilliseconds(100);
  }
  icuStopCapture(&ICUD1);
  icuStop(&ICUD1);
}
