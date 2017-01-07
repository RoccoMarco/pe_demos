*****************************************************************************
** PLAY Embedded demos - HD44780 on STM32 using ChibiOS/NIL                **
*****************************************************************************

*** About ***
This demo runs on STM32 Nucleo-64 F401RE. The MCU drives a LCD 16x2 based 
on HD44780. The used RTOS is ChibiOS/NIL

*** Related article ***
For more information read the article "How to use an HD44780 based Liquid 
Crystal Display".
http://www.playembedded.org/blog/en/2015/07/04/hd44780-lcdii-and-chibioshal/

*****************************************************************************
*** Releases and Change Log                                               ***
*****************************************************************************

*** Change log 1.0 ***
 - Project created.

*** Change log 1.1 ***
 - Replaced Port and Pad with line.
 - Minor fix indent.

*** Change log 2.0 ***
 - Improved LCD start up by adding initialization by instructions.
 - Improved code and tested both 4-bit and 8 bit-mode.
 - Now demos compiles and work with every user configuration.
 
*** Change log 2.1 ***
 - Minor comment fix.

*** Change log 2.2 ***
 - Created this file and moved change-log here.
 - Updated license.

*** Change log 3.0 ***
 - Tested under ChibiOS 17.2.1.