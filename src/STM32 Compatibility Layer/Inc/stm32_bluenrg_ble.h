//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

// Compatibility layer to use STM32_BlueNRG Stack

#ifndef STM32_BLUENRG_BLE_H
#define STM32_BLUENRG_BLE_H

#include <stdint.h>
#include "gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "spi.h"
#include "hal.h" //For SPI enable and disable

//This function reports the status of the BlueNRG Data Present pin
//Returns 1 if the pin is high
//Returns 0 if the pin is low
inline uint8_t BlueNRG_DataPresent(void) { return readBluenrgSpiIrq(); };

//Clear the SPI interrupt
inline void Clear_SPI_EXTI_Flag(void) { clearBluergIrq(); };


int32_t BlueNRG_SPI_Read_All(SPI_HandleTypeDef *hspi, uint8_t *buffer, uint8_t buff_size);

/**
  * @brief  Transmit and Receive an amount of data in blocking mode 
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer to be
  * @param  Size: amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */

typedef enum 
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;
static inline HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
	spi_transfer(Size, pTxData, pRxData);
	return HAL_OK;
}
														 
void Hal_Write_Serial(const void* data1, const void* data2, int32_t n_bytes1, int32_t n_bytes2);

int32_t BlueNRG_SPI_Write(SPI_HandleTypeDef *hspi, uint8_t* data1,
                          uint8_t* data2, uint8_t Nb_bytes1, uint8_t Nb_bytes2);

#endif
