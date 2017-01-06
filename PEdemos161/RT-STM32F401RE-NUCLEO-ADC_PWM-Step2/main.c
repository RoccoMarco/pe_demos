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
#include "math.h"

BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;
static int32_t mean;
static bool flag = FALSE;
static float lastvalue;
static uint32_t percentage[3], tmp;
/*===========================================================================*/
/* ADC related code                                                          */
/*===========================================================================*/

/*
 * In this demo we want to use a single channel to sample voltage across
 * the potentiometer.
 */
#define MY_NUM_CH                                              1
#define MY_SAMPLING_NUMBER                                    10

static adcsample_t sample_buff[MY_NUM_CH * MY_SAMPLING_NUMBER];

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 10 samples of 1 channel, SW triggered.
 * Channels:    IN0.
 */
static const ADCConversionGroup my_conversion_group = {
  FALSE,                            /*NOT CIRCULAR*/
  MY_NUM_CH,                        /*NUMB OF CH*/
  NULL,                             /*NO ADC CALLBACK*/
  NULL,                             /*NO ADC ERROR CALLBACK*/
  0,                                /* CR1 */
  ADC_CR2_SWSTART,                  /* CR2 */
  0,                                /* SMPR1 */
  ADC_SMPR2_SMP_AN0(ADC_SAMPLE_3),  /* SMPR2 */
  ADC_SQR1_NUM_CH(MY_NUM_CH),       /* SQR1 */
  0,                                /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0)   /* SQR3 */
};
/*===========================================================================*/
/* PWM related code                                                          */
/*===========================================================================*/

static void pwmpcb(PWMDriver *pwmp) {

  unsigned kk;
  (void)pwmp;
  for(kk = 0; kk < 3; kk++)
    if(percentage[kk])
      palClearPad(GPIOB, 13 + kk);
}

static void pwmc1cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, 13);
}

static void pwmc2cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, 14);
}

static void pwmc3cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, 15);
}

static PWMConfig pwmcfg = {
  1000000,                                    /* 1MHz PWM clock frequency.   */
  10000,                                       /* Initial PWM period 10ms.   */
  pwmpcb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
   {PWM_OUTPUT_ACTIVE_HIGH, pwmc2cb},
   {PWM_OUTPUT_ACTIVE_HIGH, pwmc3cb},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
};
/*===========================================================================*/
/* Common functions                                                          */
/*===========================================================================*/

/*
 * Retrieve the integer part of value
 */
static int32_t ftomod(float value){
  if (value >= 0)
    return (int32_t) value;
  else
    return (int32_t) -1 * value;
}

/*
 * Retrieve the decimal part of value
 */
static uint32_t ftodp(float value) {

  if (value >= 0)
    return (uint32_t) ((value - ftomod (value)) * 1000);
  else
    return (uint32_t) ((-value - ftomod (value)) * 1000);
}

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

static THD_WORKING_AREA(waThd1, 256);
static THD_FUNCTION(Thd1, arg) {

  unsigned jj, ii, kk;
  (void) arg;
  chRegSetThreadName("Led handler");
  pwmStart(&PWMD1, &pwmcfg);

  while(TRUE) {
    tmp = lastvalue * 90;
    ii = tmp / 90;
    jj = tmp % 90;
    if (ii == 0) {
      percentage[0] = 10000 * sin(jj * 3.14 / 180);
      percentage[1] = 0;
      percentage[2] = 10000 * cos(jj * 3.14 / 180);
     }
     else if (ii == 1) {
       percentage[0] = 10000 * cos(jj * 3.14 / 180);
       percentage[1] = 10000 * sin(jj * 3.14 / 180);
       percentage[2] = 0;
     }
     else if (ii == 2) {
       percentage[0] = 0;
       percentage[1] = 10000 * cos(jj * 3.14 / 180);
       percentage[2] = 10000 * sin(jj * 3.14 / 180);
     }

    for(kk = 0; kk < 3; kk++) {
      if(percentage[kk] < 10){
        pwmDisableChannel(&PWMD1, kk);
        palSetPad(GPIOB, kk + 13);
      }
      else if(percentage[kk] > 9990){
        pwmDisableChannel(&PWMD1, kk);
        palClearPad(GPIOB, kk + 13);
      }
      else{
        pwmEnableChannel(&PWMD1, kk, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, percentage[kk]));
        pwmEnableChannelNotification(&PWMD1, 0);
        pwmEnableChannelNotification(&PWMD1, 1);
        pwmEnableChannelNotification(&PWMD1, 2);
        pwmEnablePeriodicNotification(&PWMD1);
      }
    }

    chThdSleepMilliseconds(1);
  }
}

static THD_WORKING_AREA(waThd2, 512);
static THD_FUNCTION(Thd2, arg) {
  unsigned ii;
  (void) arg;
  chRegSetThreadName("Led handler");
  /*
   * Activates the ADC1 driver.
   */
  adcStart(&ADCD1, NULL);
  while(TRUE) {
    adcConvert(&ADCD1, &my_conversion_group, sample_buff, MY_SAMPLING_NUMBER);

    /* Making mean of sampled values.*/
    mean = 0;
    for (ii = 0; ii < MY_NUM_CH * MY_SAMPLING_NUMBER; ii++) {
      mean += sample_buff[ii];
    }
    mean /= MY_NUM_CH * MY_SAMPLING_NUMBER;
    lastvalue = (float)mean * 3 / 4096;
    flag = TRUE;
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
   * Setting up analog inputs used by the demo.
   */
  palSetPadMode(GPIOA, 0, PAL_MODE_INPUT_ANALOG);

  palSetGroupMode(GPIOB, PAL_PORT_BIT(13) | PAL_PORT_BIT(14) | PAL_PORT_BIT(15),
                  0, PAL_MODE_OUTPUT_PUSHPULL);
  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  chThdCreateStatic(waThd1, sizeof(waThd1), NORMALPRIO + 1, Thd1, NULL);
  chThdCreateStatic(waThd2, sizeof(waThd2), NORMALPRIO + 2, Thd2, NULL);

  /*
   * Normal main() thread activity, in this demo it checks flag status. If flag
   * is true, last value is printed and then flag is lowered. If error is true
   * an error message is printed.
   */
  while (TRUE) {

    if (flag) {
      chprintf(chp, "Last value: %d.%03.d V \r\n", ftomod(lastvalue),
               ftodp(lastvalue));
      chprintf(chp, "Red: %d Green: %d Blue: %d  \r\n", percentage[0] / 100,
               percentage[1] / 100, percentage[2] / 100);
      flag = FALSE;
      chThdSleepMilliseconds(150);
      chprintf(chp, "\033[2J\033[1;1H");
    }
    chThdSleepMilliseconds(1);
  }
}
