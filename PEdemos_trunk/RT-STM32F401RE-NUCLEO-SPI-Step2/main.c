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
 *  Tested under ChibiOS 17.6.x Stable, Project version 2.0.
 *  Please open readme.txt for changelog.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define NUM_GAME            4
#define NAME_LEN            15
/*
 * Pointer to the sequential stream used by chprintf();
 */
BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

static char string[NUM_GAME][NAME_LEN] = {"SHIFT", "BOOM", "POLICE", "POLICE2" };
/*
 * Current game index;
 */
static uint8_t curr = 0;

/*
 * Games timeout;
 */
static uint32_t timeout = 7000;
/*===========================================================================*/
/* SPI related code                                                          */
/*===========================================================================*/

/*
 * Pin of SPI Driver 1
 */
#define PORT_SPI1_CS                           GPIOB
#define PIN_SPI1_CS                            6
#define PORT_SPI1_SCK                          GPIOA
#define PIN_SPI1_SCK                           5
#define PORT_SPI1_MISO                         GPIOA
#define PIN_SPI1_MISO                          6
#define PORT_SPI1_MOSI                         GPIOA
#define PIN_SPI1_MOSI                          7

/*
 * Configuration for the SPI driver
 */
static const SPIConfig spi_cfg = {
  NULL,
  PORT_SPI1_CS,                                   /*   port of CS   */
  PIN_SPI1_CS  ,                                  /*   pin of CS    */
  SPI_CR1_BR_0,                                   /*   CR1 register */
  0                                               /*   CR2 register */
};
/*===========================================================================*/
/* Common functions                                                          */
/*===========================================================================*/

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

static uint16_t tx, nottx;

static THD_WORKING_AREA(waThd1, 256);
static THD_FUNCTION(Thd1, arg) {


  (void) arg;
  chRegSetThreadName("SPI thread");

  /*
   * Starting the SPID1 using spi_cfg as configuration.
   */
  spiStart(&SPID1, &spi_cfg);
  while(TRUE) {
    /*
     * Lowering Chip Select.
     */
    spiSelect(&SPID1);
    /*
     * Sending two words.
     */
    spiSend(&SPID1, 2, &nottx);
    /*
     * Raising Chip Select.
     */
    spiUnselect(&SPID1);
    chThdSleepMilliseconds(1);
  }
  /*
   * Stopping the SPID1.
   */
  spiStop(&SPID1);
}

static THD_WORKING_AREA(waThd2, 256);
static THD_FUNCTION(Thd2, arg) {


  (void) arg;
  chRegSetThreadName("Game handler");

  while(TRUE) {

    chThdSleepMilliseconds(timeout);
    /*
     * After timeout increasing current game index
     */
    curr = (curr + 1) % NUM_GAME;
  }
}

static THD_WORKING_AREA(waThd3, 256);
static THD_FUNCTION(Thd3, arg) {


  (void) arg;
  chRegSetThreadName("Array creator");
  unsigned jj;
  while(TRUE) {
    switch (curr) {
    default:
    case 0:
      /*
       * SHIFT
       */
      for (jj = 0; jj < 15; jj++) {
        tx = 1 << jj;
        nottx = ~tx;
        chThdSleepMilliseconds(80);
      }
      for (jj = 16; jj > 0; jj--) {
        tx = 1 << (jj - 1);
        nottx = ~tx;
        chThdSleepMilliseconds(80);
      }
      break;
    case 1:
      /*
       * BOOM
       */
      for (jj = 0; jj < 8; jj++) {
        tx = (1 << jj) | ((1 << 15) >> (jj));
        nottx = ~tx;
        chThdSleepMilliseconds(200);
      }
      for (jj = 0; jj < 5; jj++) {
        tx = 0xFFFF;
        nottx = ~tx;
        chThdSleepMilliseconds(100);
        tx = nottx;
        nottx = ~tx;
        chThdSleepMilliseconds(100);
      }
      break;
    case 2:
      /*
       * POLICE
       */
      for (jj = 0; jj < 6; jj++) {
        tx = 0x00FF;
        nottx = ~tx;
        chThdSleepMilliseconds(30);
        tx = 0x0000;
        nottx = ~tx;
        chThdSleepMilliseconds(30);
      }
      for (jj = 0; jj < 6; jj++) {
        tx = 0xFF00;
        nottx = ~tx;
        chThdSleepMilliseconds(30);
        tx = 0x0000;
        nottx = ~tx;
        chThdSleepMilliseconds(30);
      }
      chThdSleepMilliseconds(500);
      break;
    case 3:
      /*
       * POLICE2
       */
      for (jj = 0; jj < 6; jj++) {
        tx = 0x00CC;
        nottx = ~tx;
        chThdSleepMilliseconds(100);
        tx = 0x0033;
        nottx = ~tx;
        chThdSleepMilliseconds(100);
      }
      for (jj = 0; jj < 6; jj++) {
        tx = 0xCC00;
        nottx = ~tx;
        chThdSleepMilliseconds(100);
        tx = 0x3300;
        nottx = ~tx;
        chThdSleepMilliseconds(100);
      }
      tx = 0x0000;
      nottx = ~tx;
      chThdSleepMilliseconds(200);
      break;
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

  /*
   * SPID1 I/O pins setup.(Overwriting board.h configurations)
   */
   palSetPadMode(PORT_SPI1_SCK, PIN_SPI1_SCK,
                 PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);    /* New SCK */
   palSetPadMode(PORT_SPI1_MISO, PIN_SPI1_MISO,
                 PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);    /* New MISO*/
   palSetPadMode(PORT_SPI1_MOSI, PIN_SPI1_MOSI,
                 PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);    /* New MOSI*/
   palSetPadMode(PORT_SPI1_CS, PIN_SPI1_CS,
                 PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); /* New CS*/

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);


  chThdCreateStatic(waThd1, sizeof(waThd1), NORMALPRIO + 1, Thd1, NULL);
  chThdCreateStatic(waThd2, sizeof(waThd2), NORMALPRIO - 1, Thd2, NULL);
  chThdCreateStatic(waThd3, sizeof(waThd3), NORMALPRIO - 2, Thd3, NULL);
  /*
   * Normal main() thread activity: printing last value as decimal and as binary.
   */
  while (TRUE) {

    chprintf(chp, "GAME: %s \r\n", string[curr]);
    chThdSleepMilliseconds(150);
    /*
     * This is a special ANSI code that clear terminal and move cursor to
     * top left corner position. If your terminal is not ANSI capable remove
     * next chprintf().
     */
    chprintf(chp, "\033[2J\033[1;1H");
  }
}
