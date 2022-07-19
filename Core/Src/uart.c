/*
*/

#include <string.h>
#include "main.h"
#include "ethernetif.h"
#include "uart.h"
#include "eeprom.h"
#include "modbus_rtu.h"

/* Private variables -----------------------------------------------------------------------------------------------------------------------*/
UART_HandleTypeDef *urt;

GPIO_TypeDef       *gpio_en;
uint16_t            gpio_en_pin;

uint8_t 					  slave_id;

bool    						uart_process 	= false;

uint8_t 						tx_indx;
uint8_t 						tx_cnt;
uint8_t 						tx_buf[64] 		= {0};

uint8_t 						rx_char;
uint8_t 						rx_indx;
uint8_t 						rx_cnt;
uint8_t 						rx_buf[64] 		= {0};



uint16_t 						crc 					= 0xFFFF;

bool    		        start_timeout = 0;
uint8_t             timeout_count = 0;

/* Private macros-----------------------------------------------------------------------------------------------------------------------------*/
#define EN_RXTX(x) ( x ? HAL_GPIO_WritePin(gpio_en ,  gpio_en_pin,  GPIO_PIN_SET) : HAL_GPIO_WritePin(gpio_en ,  gpio_en_pin,  GPIO_PIN_RESET))

/* Private function proto-types --------------------------------------------------------------------------------------------------------------*/
uint16_t uart_crc16(uint8_t *puchMsg, uint16_t usDataLen);
void uart_send(bool use_crc, uint8_t byteCount);

void uart_init(UART_HandleTypeDef * ut, GPIO_TypeDef *gpio, uint16_t gpio_pin)
{
	urt         = ut;
	gpio_en     = gpio;
	gpio_en_pin = gpio_pin;

	slave_id    = eeprom_read_byte(14);
	if(slave_id == 0 || slave_id == 255)
	{
		slave_id = 1;
	}
	
	EN_RXTX(0);
	
	HAL_UART_Receive_IT(urt, &rx_char, 1);
}
/*

	ISR ROUTINES ********************************************************************************************************************************

*/
/*
	RX ISR---------------------------------------------------------------------------------------------------------------------------------------
*/
void uart_rx_isr(void)
{

}
/*
	TX ISR---------------------------------------------------------------------------------------------------------------------------------------
*/
void uart_tx_isr(void)
{
	
}
/*
	TIME-OUT ISR---------------------------------------------------------------------------------------------------------------------------------
*/
void uart_timeout_isr(void)
{
	
}
/*

	UART MESSAGE PROCESSING ROUTINES*************************************************************************************************************

*/
/*
	CRC CALCULATION ROUTINE----------------------------------------------------------------------------------------------------------------------
*/
uint16_t uart_crc16(uint8_t *puchMsg, uint16_t usDataLen) 
{ 
	uint8_t uchCRCHi = 0xFF ; 
	uint8_t uchCRCLo = 0xFF ; 

	return (uchCRCHi << 8 | uchCRCLo) ; 
}
/*
	UART PROCESS ROUTINE CALLED IN MAIN LOOP-----------------------------------------------------------------------------------------------------
*/
void uart_process_msg(void)
{	

}


void uart_send(bool use_crc, uint8_t byteCount)
{

}


