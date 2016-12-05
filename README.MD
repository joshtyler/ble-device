# BLE Peripheral
This project implements a BLE Peripheral using a Freescale FRDM-KL46Z development board, and X-NUCLEO-IDB04A1 (BLUENRG) developement board.

Note that the X-NUCLEO-IDB04A1 board has been modified so that the pins are compatible with the KL46Z SPI port.
The modifications that were performed were:
Resistors R15, R10, and R12 were removed.
Resistors R13, R11, and R16 were populated with 0402 0 Ohm resistors.

The peripheral uses the accelerometer to provide data, and is compatible with the [ST BlueNRG demo available on the play store](https://play.google.com/store/apps/details?id=com.st.blunrg&hl=en_GB).

Note that this code includes [tinyprintf](https://github.com/cjlano/tinyprintf/blob/master/LICENSE.BSD-new).

The code compiles, and has been tested successfully, using Keil v5.21.1.0

##Code details
The software runs FreeRTOS, and uses message passing to communicate between an accelerometer read thread, and a bluetooth handling thread.

The accelerometer data is filtered using a moving average filter.

Bluetooth communication is handled using part of the STM32 BlueNRG stack. This is un-modified from the code provided by ST, with the exception of the following two lines added to list.c:
```c
#include <MKL46Z4.H>
#include <core_cm0plus.h>
```
This means that it would be easy to update the project if ST release an update to the bluetooth stack. The ST bluetooth stack has been made to be compatible with the Freescale microcontroller thanks to the code in the "STM32 compatibility layer" folder. This code implements board-specific functions using STM32 HAL style function signatures, using headers of the same name as the original ST files. 

The device broadcasts with the name BlueNRG to maintain compatiblitity with the ST example, however this can easily be modified by changing line 420 of sensor_service.c

##License
See LICENSE.txt


