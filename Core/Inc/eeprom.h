#ifndef __eeprom_h
#define __eeprom_h

#include "stm32l1xx_hal.h"
#include <stdint.h>
#include "calibration.h"

HAL_StatusTypeDef eeprom_write_byte(uint16_t addr, uint8_t val);
uint8_t           eeprom_read_byte(uint16_t addr);
void              eeprom_read_network_settings(void);
void 							eeprom_write_calibration_variable(uint8_t chnl, uint8_t variable_number, int32_t variable_value);
void              eeprom_write_calibration_step(uint8_t chnl, uint8_t variable_number, int32_t adc_value, int32_t variable_value);
void 							eeprom_read_calibration_variables(uint8_t chnl, calibration *cl);
void 							eeprom_write_calibration_tare(uint8_t chnl, int32_t adc_value);
void 							eeprom_backup_calibration(void);
void              eeprom_recall_calibration(void);
#endif
