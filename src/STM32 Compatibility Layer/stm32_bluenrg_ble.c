//Copyright (c) 2016 Joshua Lewis Tyler
//See LICENSE.txt

// Compatibility layer to use STM32_BlueNRG Stack

#include "stm32_bluenrg_ble.h"
#include "hal.h" //For SPI enable and disable
#include "gpio.h"

#define HEADER_SIZE 5
#define MAX_BUFFER_SIZE 255
#define TIMEOUT_DURATION 15

//N.B. These refer to the external SPI interrrupt pin. Not an interrupt driven SPI driver
void Disable_SPI_IRQ(void) { disableBluenrgSpiIrq(); };
void Enable_SPI_IRQ(void) { enableBluenrgSpiIrq(); };


/**
 * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
 * @param  hspi     : SPI handle
 * @param  buffer   : Buffer where data from SPI are stored
 * @param  buff_size: Buffer size
 * @retval int32_t  : Number of read bytes
 */
int32_t BlueNRG_SPI_Read_All(SPI_HandleTypeDef *hspi, uint8_t *buffer,
                             uint8_t buff_size)
{
  uint16_t byte_count;
  uint8_t len = 0;
  uint8_t char_ff = 0xff;
  volatile uint8_t read_char;

  uint8_t header_master[HEADER_SIZE] = {0x0b, 0x00, 0x00, 0x00, 0x00};
  uint8_t header_slave[HEADER_SIZE];

	volatile int i;
	clearSpiCs();
	for(i=0;i<100;i++)__NOP(); //Dumb delay to meet timing

  /* Read the header */  
  HAL_SPI_TransmitReceive(hspi, header_master, header_slave, HEADER_SIZE, TIMEOUT_DURATION);
  	
  if (header_slave[0] == 0x02) {
    /* device is ready */
    byte_count = (header_slave[4]<<8)|header_slave[3];
  
    if (byte_count > 0) {
  
      /* avoid to read more data that size of the buffer */
      if (byte_count > buff_size){
        byte_count = buff_size;
      }
  
      for (len = 0; len < byte_count; len++){                                               
        HAL_SPI_TransmitReceive(hspi, &char_ff, (uint8_t*)&read_char, 1, TIMEOUT_DURATION); 
        buffer[len] = read_char;                                                            
      }                                                                                     
      
    }    
  }
	for(i=0;i<100;i++)__NOP();
	setSpiCs();
	for(i=0;i<500;i++)__NOP(); //This delay is extra long to allow chip to de-asset IRQ
	
	return len;
}
														 
/**
 * @brief  Writes data to a serial interface.
 * @param  data1   :  1st buffer
 * @param  data2   :  2nd buffer
 * @param  n_bytes1: number of bytes in 1st buffer
 * @param  n_bytes2: number of bytes in 2nd buffer
 * @retval None
 */
void Hal_Write_Serial(const void* data1, const void* data2, int32_t n_bytes1,
                      int32_t n_bytes2)
{
  while(1){
    if(BlueNRG_SPI_Write(0, (uint8_t *)data1,(uint8_t *)data2, n_bytes1, n_bytes2)==0)
			break;
  }

}

/**
 * @brief  Writes data from local buffer to SPI.
 * @param  hspi     : SPI handle
 * @param  data1    : First data buffer to be written
 * @param  data2    : Second data buffer to be written
 * @param  Nb_bytes1: Size of first data buffer to be written
 * @param  Nb_bytes2: Size of second data buffer to be written
 * @retval Number of read bytes
 */
int32_t BlueNRG_SPI_Write(SPI_HandleTypeDef *hspi, uint8_t* data1,
                          uint8_t* data2, uint8_t Nb_bytes1, uint8_t Nb_bytes2)
{   
  
  unsigned char header_master[HEADER_SIZE] = {0x0a, 0x00, 0x00, 0x00, 0x00};
  unsigned char header_slave[HEADER_SIZE]  = {0xaa, 0x00, 0x00, 0x00, 0x00};
  
  unsigned char read_char_buf[MAX_BUFFER_SIZE];
	
	int32_t ret_val = 0;

	volatile int i;
	clearSpiCs();
	for(i=0;i<100;i++)__NOP(); //Dumb delay to meet timing

  /* Exchange header */  
  HAL_SPI_TransmitReceive(hspi, header_master, header_slave, HEADER_SIZE, TIMEOUT_DURATION);
  
	//This has been added to correct a bug in the code!
	//Previously only the lower half of the buffer was being checked!
	uint16_t byte_count = (header_slave[2]<<8)|header_slave[1];

  if (header_slave[0] == 0x02) {
    /* SPI is ready */
    if (byte_count >= (Nb_bytes1+Nb_bytes2)) {
  
      /*  Buffer is big enough */
      if (Nb_bytes1 > 0) {
        HAL_SPI_TransmitReceive(hspi, data1, read_char_buf, Nb_bytes1, TIMEOUT_DURATION);
      }
      if (Nb_bytes2 > 0) {
        HAL_SPI_TransmitReceive(hspi, data2, read_char_buf, Nb_bytes2, TIMEOUT_DURATION);
      }

    } else {
      /* Buffer is too small */
      ret_val = -2;
    }
  } else {
    /* SPI is not ready */
    ret_val = -1;
  }

	for(i=0;i<100;i++)__NOP();
	setSpiCs();
	for(i=0;i<100;i++)__NOP();
	
  return ret_val;
}
