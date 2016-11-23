//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

// Basic polling driver for Freescale KL46 family microcontroller

#ifndef SPI_H
#define SPI_H

#include <stdint.h>

//Define SPI PINS
#define SPI1_PORT PORTD
#define SPI1_GPIO PTD
#define SPI1_ALT 2
#define SPI1_MISO_PIN 7
#define SPI1_MOSI_PIN 6
#define SPI1_CS_PIN 4
#define SPI1_CLK_PIN 5

//Setup SPI 1
void spi_init(void);
void spi_transfer(unsigned int length, uint8_t *txData, uint8_t *rxData);

#endif
