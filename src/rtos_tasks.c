//Copyright (c) 2016 Steven Yan and Joshua Lewis Tyler
//Licensed under the MIT license
//See LICENSE.txt

//Tasks to run as FreeRTOS tasks

#include "rtos_tasks.h"


//Device header
#include <MKL46Z4.H>

//Standard headers
#include <stdint.h>
#include <stdbool.h>

//FreeRTOS libraries
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//User libraries
#include "uart.h" //UART Setup
#include "debug.h" //Debug print functions
#include "clock_config.h" //Clock configuration
#include "gpio.h" //To read buttons

//Heartbeat task to show that system is still alive
//Blink LEDs and send UART message every second
//Also send system up message on boot
//Hardcoded for green LED (PTD5) on KL-46Z dev board
void heartbeat(void *pvParameters)
{
	dbg_puts("Begin.\r\n");
	setLED1();
	clearLED2();
	while(1)
	{
		toggleLED1();
		toggleLED2();
		dbg_puts("Heartbeat\r\n");
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}


void vApplicationMallocFailedHook( void )
{
	dbg_puts("Malloc failed\r\n");
	while(1);
}


void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
	dbg_puts("Stack overflow\r\n");
	while(1);
}
