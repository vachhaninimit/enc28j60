#include "eeprom.h"
#include "ethernetif.h"

uint16_t lc0_base_add          = 15;

uint16_t lc1_base_add          = 116;


uint16_t lc2_base_add          = 217;


/* Private defines -----------------------------------------------------------*/

#define EEPROM_BASE_ADDR				 0x08080000	

HAL_StatusTypeDef eeprom_status = HAL_OK;

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef eeprom_write_byte(uint16_t addr, uint8_t val);
HAL_StatusTypeDef eeprom_write_long(uint16_t addr, int32_t val);
int32_t           eeprom_read_long(uint16_t addr);
void              eeprom_read_calibration_lookup(uint8_t chnl, calibration *cl);
	
HAL_StatusTypeDef eeprom_write_byte(uint16_t addr, uint8_t val)
{
	eeprom_status = HAL_OK;
	
	HAL_FLASHEx_DATAEEPROM_Unlock();
		
	eeprom_status = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE , EEPROM_BASE_ADDR + addr, val);
	
	HAL_FLASHEx_DATAEEPROM_Lock();
	
	return eeprom_status;
}
/*

*/
HAL_StatusTypeDef eeprom_write_long(uint16_t addr, int32_t val)
{
	eeprom_status = HAL_OK;
	
	HAL_FLASHEx_DATAEEPROM_Unlock();
		
	eeprom_status = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE , EEPROM_BASE_ADDR + addr, (val >> 24));
	eeprom_status = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE , EEPROM_BASE_ADDR + addr + 1, (val >> 16));
	eeprom_status = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE , EEPROM_BASE_ADDR + addr + 2, (val >> 8));
	eeprom_status = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE , EEPROM_BASE_ADDR + addr + 3, val & 0xFF);
	
	HAL_FLASHEx_DATAEEPROM_Lock();
	
	return eeprom_status;
}
/*

*/
uint8_t eeprom_read_byte(uint16_t addr)
{
	uint8_t tmp = 0;	
	tmp = *(__IO uint8_t*)(EEPROM_BASE_ADDR + addr);

	return tmp;
}
/*

*/
int32_t eeprom_read_long(uint16_t addr)
{
	int32_t tmp = 0;	
	tmp = *(__IO uint8_t*)(EEPROM_BASE_ADDR + addr);
	tmp = (tmp << 8)| *(__IO uint8_t*)(EEPROM_BASE_ADDR + addr + 1);
	tmp = (tmp << 8)| *(__IO uint8_t*)(EEPROM_BASE_ADDR + addr + 2);
	tmp = (tmp << 8)| *(__IO uint8_t*)(EEPROM_BASE_ADDR + addr + 3);
	return tmp;
}
/*

*/
void eeprom_read_network_settings(void)
{
	uint8_t temp_byte = eeprom_read_byte(1);
	
	if(temp_byte > 0x00)
	{

	}
}
/*

*/
void eeprom_read_calibration_variables(uint8_t chnl, calibration *cl)
{

}
/*

*/
void eeprom_read_calibration_lookup(uint8_t chnl, calibration *cl)
{

}
/*

*/
void eeprom_write_calibration_variable(uint8_t chnl, uint8_t variable_number, int32_t variable_value)
{

}
/*

*/
void eeprom_write_calibration_tare(uint8_t chnl, int32_t adc_value)
{

}
/*

*/
void eeprom_write_calibration_step(uint8_t chnl, uint8_t variable_number, int32_t adc_value, int32_t variable_value)
{

}
/*

*/
void eeprom_backup_calibration(void)
{	

}
/*

*/
void eeprom_recall_calibration(void)
{	

}
