//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

#ifndef RTOS_TASKS_H
#define RTOS_TASKS_H

#include "FreeRTOS.h"
#include "task.h"

void heartbeat(void *pvParameters);
void ble(void *pvParameters);
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName );

#endif
