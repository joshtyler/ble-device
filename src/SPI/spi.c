//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

// Basic polling driver for Freescale KL46 family microcontroller

#include <MKL46Z4.H> //For device registers
#include "spi.h"
#include "gpio.h"

void spi_init(void)
{
	//Enable SPI1 clock
	SIM->SCGC4 |= SIM_SCGC4_SPI1_MASK;
	
	//Setup SPI Pins
	//Port, pin, output, pullen, pullup, alt
	init_pin(SPI1_PORT, SPI1_MISO_PIN, false, false, false, SPI1_ALT);
	init_pin(SPI1_PORT, SPI1_MOSI_PIN, true, false, false, SPI1_ALT);
	init_pin(SPI1_PORT, SPI1_CS_PIN, true, false, false, 1); //Note change to GPIO alt
	setSpiCs();
	init_pin(SPI1_PORT, SPI1_CLK_PIN, true, false, false, SPI1_ALT);
	
	//Disable SPI1 before configuration
	SPI1->C1 &= ~SPI_C1_SPE_MASK;
	
	//Set SPI1 to master
	//Set CPOL=0 (Required by BlueNRG)
	//Set CPHA=0 (Required by BlueNRG)
	//Set SS to be GPIO (To ensure we meet the correct timing)
	//Set output direction to be MSB first
	SPI1->C1 = SPI_C1_MSTR(1U) | SPI_C1_CPOL(0) | SPI_C1_CPHA(0) | SPI_C1_SSOE(0) | SPI_C1_LSBFE(0);

	
	//Set SS to be the SS output (not GPIO)
	//Disable bi-directional mode
	//Continue SPI clocks in wait mode (We don't care about power saving yet
	//Set SPI to 8 bit mode
  SPI1->C2 = SPI_C2_MODFEN(1) | SPI_C2_BIDIROE(1) | SPI_C2_SPISWAI(0) | SPI_C2_SPC0(0) | SPI_C2_SPIMODE(0);


	//Set baudrate divisor to 16
	//Set baudrate prescaler to 2
	//This is for 500bps from 24MHz clock
	SPI1->BR = SPI_BR_SPR(3) | SPI_BR_SPPR(2);
	
	//Enable SPI
	SPI1->C1 |= SPI_C1_SPE_MASK;

}

void spi_transfer(unsigned int length, uint8_t *txData, uint8_t *rxData)
{
	//Disable and re-enable SPI to clear S register
  SPI1->C1 &= ~SPI_C1_SPE_MASK;
  SPI1->C1 |= SPI_C1_SPE_MASK;
	
	unsigned int i;
	
	volatile int j;
	for(i = 0; i < length; i++)
	{	
		//Send data
		while((SPI1->S & SPI_S_SPTEF_MASK) == 0);
		SPI1->DL = txData[i];
		
		//Receive data
		while ((SPI1->S & SPI_S_SPRF_MASK) == 0);
		rxData[i] = SPI1->DL;
	}
	
}
