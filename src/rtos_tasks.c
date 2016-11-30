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
#include "iic.h"
#include "filter.h"

#include "osal.h"
#include "sensor_service.h"
#include "debug.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"

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

#define IDB04A1 0
#define IDB05A1 1
#define BDADDR_SIZE 6
#define PRINTF dbg_printf

extern volatile uint8_t set_connectable;
extern volatile int connected;
extern AxesRaw_t axes_data;

void ble(void *pvParameters)
{
	const char *name = "BlueNRG";
  uint8_t SERVER_BDADDR[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x03};
  uint8_t bdaddr[BDADDR_SIZE];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
	uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */
  
  uint8_t  hwVersion;
  uint16_t fwVersion;
  
  int ret;  
	
	
	//Reset blueNRG
	clearBluenrgReset();
	vTaskDelay(10/portTICK_RATE_MS);
	setBluenrgReset();
	vTaskDelay(100/portTICK_RATE_MS);
	
	getBlueNRGVersion(&hwVersion, &fwVersion);
	
	//Reset blueNRG again, otherwise we can't change address
	clearBluenrgReset();
	vTaskDelay(10/portTICK_RATE_MS);
	setBluenrgReset();
	vTaskDelay(100/portTICK_RATE_MS);
	
	//Copied from example project
  if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
    bnrg_expansion_board = IDB05A1; 
    /*
     * Change the MAC address to avoid issues with Android cache:
     * if different boards have the same MAC address, Android
     * applications unless you restart Bluetooth on tablet/phone
     */
    SERVER_BDADDR[5] = 0x02;
  }

  /* The Nucleo board must be configured as SERVER */
  Osal_MemCpy(bdaddr, SERVER_BDADDR, sizeof(SERVER_BDADDR));
  
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);
  if(ret){
    PRINTF("Setting BD_ADDR failed.\n");
  }
  
  ret = aci_gatt_init();    
  if(ret){
    PRINTF("GATT_Init failed.\n");
  }

  if (bnrg_expansion_board == IDB05A1) {
    ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }
  else {
    ret = aci_gap_init_IDB04A1(GAP_PERIPHERAL_ROLE_IDB04A1, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }

  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GAP_Init failed.\n");
  }

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   strlen(name), (uint8_t *)name);

  if(ret){
    PRINTF("aci_gatt_update_char_value failed.\n");            
    while(1);
  }
  
  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL,
                                     7,
                                     16,
                                     USE_FIXED_PIN_FOR_PAIRING,
                                     123456,
                                     BONDING);
  if (ret == BLE_STATUS_SUCCESS) {
    PRINTF("BLE Stack Initialized.\n");
  }
  
  PRINTF("SERVER: BLE Stack Initialized\n");
  
  ret = Add_Acc_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Acc service added successfully.\n");
  else
    PRINTF("Error while adding Acc service.\n");
  
  ret = Add_Environmental_Sensor_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Environmental Sensor service added successfully.\n");
  else
    PRINTF("Error while adding Environmental Sensor service.\n");

  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,4);
	//End copied from example project
	
	while(1)
	{
		HCI_Process();
		User_Process(&axes_data);
	}
}

//This is not a freeRTOS task, but a helper task
void User_Process(AxesRaw_t* p_axes)
{
	static filterHandle_t x_filt, y_filt, z_filt;
	
  if(set_connectable){
    setConnectable();
    set_connectable = FALSE;
  }  

    if(connected)
    {
			//Read accel
			int16_t x,y,z;
			readAccel(&x, &y, &z);
			PRINTF("RAW: X=%6d Y=%6d Z=%6d\r\n", x, y, z);
			movingAverageAddSample(&x_filt, x);
			movingAverageAddSample(&y_filt, y);
			movingAverageAddSample(&z_filt, z);
			
      /* Update acceleration data */
      p_axes->AXIS_X = x_filt.curVal/2;
      p_axes->AXIS_Y = y_filt.curVal/2;
      p_axes->AXIS_Z = z_filt.curVal/2;
      PRINTF("ACC: X=%6d Y=%6d Z=%6d\r\n", p_axes->AXIS_X, p_axes->AXIS_Y, p_axes->AXIS_Z);
      Acc_Update(p_axes);
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
