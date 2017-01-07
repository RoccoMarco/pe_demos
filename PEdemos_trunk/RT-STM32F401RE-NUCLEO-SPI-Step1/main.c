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
 *  Tested under ChibiOS 17.2.0, Project version 2.0.
 *  Please open readme.txt for changelog.
 */

#include "ch.h"
#include "hal.h"

#include "chprintf.h"

/*
 * Pointer to the sequential stream used by chprintf();
 */
BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

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

/*
 * Convert value in a string of '0' and '1'
 */
static void uint8tobitstring(uint8_t value, char string[9]) {

  unsigned kk;
  for(kk = 0; kk < 8; kk++) {
    (value & (1 << kk)) ? (string[7 - kk] = '1') : (string[7 - kk] = '0');
  }
  string[8] = '\0';
}

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

static uint8_t tx[2], nottx[2];
static uint16_t tmp;
static char string[2][9];

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
    spiSend(&SPID1, 2, nottx);
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
  chRegSetThreadName("Increaser");
  tx[0] = 0;
  tx[1] = 0;
  nottx[0] = ~tx[0];
  nottx[1] = ~tx[1];
  while(TRUE) {
    nottx[0] = ~tx[0];
    chThdSleepMilliseconds(100);
    tx[0]++;
    if(tx[0] == 0) {
      /*
       * Carry
       */
      tx[1]++;
      nottx[1] = ~tx[1];
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

  /*
   * Normal main() thread activity: printing last value as decimal and as binary.
   */
  while (TRUE) {

    uint8tobitstring(tx[0], string[0]);
    uint8tobitstring(tx[1], string[1]);
    tmp = (tx[1] << 8) + tx[0];
    chprintf(chp, "Last value: %d that is %s%s \r\n", tmp, string[1], string[0]);
    chThdSleepMilliseconds(150);
    /*
     * This is a special ANSI code that clear terminal and move cursor to
     * top left corner position. If your terminal is not ANSI capable remove
     * next chprintf().
     */
    chprintf(chp, "\033[2J\033[1;1H");
  }
}
