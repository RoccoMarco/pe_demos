/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

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

#ifndef _USBCFG_H_
#define _USBCFG_H_

#if HAL_USE_SERIAL_USB || defined(__DOXYGEN__)
/*
 * DP resistor control is not possible on the STM32F3-Discovery, using stubs
 * for the connection macros.
 */
#if !defined (usb_lld_connect_bus)
#define usb_lld_connect_bus(usbp)
#endif

#if !defined (usb_lld_disconnect_bus)
#define usb_lld_disconnect_bus(usbp)
#endif

extern SerialUSBDriver SDU1;
extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;


#endif /* HAL_USE_SERIAL_USB */
#endif  /* _USBCFG_H_ */

/** @} */
