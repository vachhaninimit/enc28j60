#include "main.h"
#include "uart.h"
#include "modbus_rtu.h"
#include "ethernetif.h"
#include "eeprom.h"
#include "calibration.h"

uint16_t 					  add;
uint16_t 						qty;

bool modbus_rtu_read_holding_registers(uint8_t * src, uint8_t * dest);
bool modbus_rtu_write_single_register(uint8_t * src, uint8_t * dest);
bool modbus_rtu_write_multiple_registers(uint8_t * src, uint8_t * dest);
	
bool modbus_rtu_process(uint8_t * src, uint8_t * dest, uint16_t len)
{
	bool _return_value = 0;
	
	return _return_value;
}

bool modbus_rtu_read_holding_registers(uint8_t * src, uint8_t * dest)
{	
	bool _return_value1 = 0;

	
	return _return_value1;
}
bool modbus_rtu_write_single_register(uint8_t * src, uint8_t * dest)
{
	bool _return_value2 = 0;
	

	return _return_value2;
}

bool modbus_rtu_write_multiple_registers(uint8_t * src, uint8_t * dest)
{
	bool _return_value3  = 0;
	

	
	return _return_value3;
}

/*
*/
