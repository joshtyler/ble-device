//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

// Compatibility layer to use STM32_BlueNRG Stack

#ifndef CLOCK_H
#define CLOCK_H

//Use FreeRTOS ticks rather than hardware timer as STM used
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

//Clock ticks per second
const uint32_t CLOCK_SECOND = 1000/portTICK_RATE_MS;

typedef uint32_t tClockTime;

static inline tClockTime Clock_Time(void) { return xTaskGetTickCount(); };


#endif
