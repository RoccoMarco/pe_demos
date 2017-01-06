/*
    PLAY Embedded demos - Copyright (C) 2014-2017 Rocco Marco Guglielmi

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
 *  Tested under ChibiOS 16.1.4, Project version 1.2.
 *  Please open readme.txt for changelog.
 */

#include "ch.h"
#include "hal.h"

#include "chprintf.h"

BaseSequentialStream* chp = (BaseSequentialStream*) &SD2;

static bool button_pressed = FALSE;

static THD_WORKING_AREA(waThd1, 128);
static THD_FUNCTION(Thd1, arg) {

  (void) arg;
  chRegSetThreadName("Button Check");

  while(TRUE) {
    /*
     * Checking if button is pressed
     */
    if(palReadPad(GPIOC, GPIOC_BUTTON)) {
      chThdSleepMilliseconds(50);
      /*
       * Checking if button is released
       */
      if(!palReadPad(GPIOC, GPIOC_BUTTON)) {
        /*
         * At that point we could assert that button has been pushed
         * We raise a flag
         */
        button_pressed = TRUE;
      }
    }
    chThdSleepMilliseconds(1);
  }
}

static THD_WORKING_AREA(waThd2, 128);
static THD_FUNCTION(Thd2, arg) {

  (void) arg;
  chRegSetThreadName("Printer");

  /*
   * Starting Serial Driver 2 with default configuration.
   */
  sdStart(&SD2, NULL);
  while(TRUE) {
    if(button_pressed) {
      /*
       * Toggling LED status
       */
      palTogglePad(GPIOA, GPIOA_LED_GREEN);
      /*
       * Printing on a Base Sequential Stream that is SD2 (Internally connected
       * to our USB)
       */
      chprintf(chp, "Hello PLAYembedded.org\n\r");
      /*
       * Lowering the flag
       */
      button_pressed = FALSE;
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

  /*
   * Here we are creating two threads.
   */
  chThdCreateStatic(waThd1, sizeof(waThd1), NORMALPRIO + 1, Thd1, NULL);
  chThdCreateStatic(waThd2, sizeof(waThd2), NORMALPRIO - 1, Thd2, NULL);

  /*
   * Normal main() thread activity: nothing.
   */
  while (TRUE) {

    chThdSleepMilliseconds(500);
  }
}
