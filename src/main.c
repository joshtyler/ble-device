//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

#define NDEBUG

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
#include "tinyprintf.h" //For printf functionality
#include "spi.h"

#define STACK_SIZE		( ( unsigned short ) 128 )
	
static inline void printf_putc(void* p, char c) { uart_putchar(c); }; //Definition for tiny_printf library


int main(void)
{
	//Setup clocking
	BOARD_BootClockRUN();
	SystemCoreClockUpdate();
	
	//Setup printf
	init_printf(NULL,printf_putc);
	
	//Setup peripherals
	gpio_init();
	uart_init(115200);
	spi_init();
	
	//Test SPI
	while(1)
	{
		uint8_t txData[5] = {0,1,2,3,4};
		uint8_t rxData[5] = {0xFF,0xFF,0xFF,0xFF,0xFF};
		spi_transfer(5,txData,rxData);
	}
	

	//Heartbeat task
	//Blink LED and send UART message to indicate that we're still alive
	xTaskCreate(heartbeat, (const char *)"Heartbeat", STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	
	vTaskStartScheduler();

	return 0;
}


