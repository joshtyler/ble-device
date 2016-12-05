//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

//Device header
#include <MKL46Z4.H>

//Standard headers
#include <stdint.h>
#include <stdbool.h>

//FreeRTOS libraries
#include "FreeRTOS.h"
#include "list.h"
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
#include "hci.h"
#include "iic.h"

#define MIN_STACK_SIZE		( ( unsigned short ) 128 )
	
//Queue to send accelerometer data to bluetooth task
//Defined in rtos_tasks.c
extern xQueueHandle accelQueue;
	
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
	
	//Setup IIC in Kinetis SDK style
	BOARD_I2C_ReleaseBus();
	BOARD_I2C_ConfigurePins();
	
	//Setup HCI
	HCI_Init();
	
	//Queue to send accelerometer data to bluetooth task
	//Only holds one item to ensure it's up to date
	accelQueue = xQueueCreate(1, sizeof(accelData_t));
	
	//Heartbeat task
	//Blink LED and send UART message to indicate that hardware is still alive
	xTaskCreate(heartbeat, (const char *)"Heartbeat", MIN_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY +1, NULL);
	
	//BLE task
	xTaskCreate(ble, (const char *)"BLE",  4*MIN_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	
	//Accelerometer task
	xTaskCreate(accelRead, (const char *)"Accel",  4*MIN_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	
	vTaskStartScheduler();

	return 0;
}


