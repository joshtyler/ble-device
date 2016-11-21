//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

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
#include "rtos_tasks.h"
#include "uart.h" //UART Setup
#include "debug.h" //Debug print functions
#include "clock_config.h" //Clock configuration
#include "gpio.h" //To read buttons

#define STACK_SIZE		( ( unsigned short ) 128 )
	

int main(void)
{
	//Setup clocking
	BOARD_BootClockRUN();
	SystemCoreClockUpdate();
	
	//Setup peripherals
	gpio_init();
	uart_init(115200);

	//Heartbeat task
	//Blink LED and send UART message *at lowest priority* to indicate that we're still alive
	xTaskCreate(heartbeat, (const char *)"Heartbeat", STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	

	vTaskStartScheduler();

	return 0;
}


