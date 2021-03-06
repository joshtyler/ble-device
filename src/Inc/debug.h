//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

//Functions to allow debug data to be output over UART
//Only compile the functions in if NDEBUG is not defined

#ifndef DEBUG_H
#define DEBUG_H

#include "uart.h"
#include "tinyprintf.h"

//Conditionally include string functionality if debug is enabled
#ifdef NDEBUG
	#define dbg_puts(str)
	#define dbg_putchar(c)
	#define dbg_printf(s,...)
#else
	#define dbg_puts(str) uart_puts(str)
	#define dbg_putchar(c) uart_putchar(c)
	#define dbg_printf tfp_printf
#endif


#endif
