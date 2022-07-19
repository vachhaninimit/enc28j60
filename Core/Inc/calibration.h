#ifndef __calibration_h
#define __calibration_h

#include <stdbool.h>
#include <stdint.h>
#include "stm32l1xx_hal.h"

typedef struct 
{
	float _current_measure;
	float _err_measure;
  float _err_estimate;
  float _q;
  float _current_estimate;
  float _last_estimate;
  float _kalman_gain;
	int32_t x;
}filter;

typedef struct 
{
	GPIO_TypeDef *gpio_adc_data;
	uint16_t      gpio_adc_data_pin; 
	
	GPIO_TypeDef *gpio_adc_clk;
	uint16_t      gpio_adc_clk_pin; 
	
	uint8_t dp;
	uint8_t lc;
	uint8_t steps;
	int32_t range;
	int32_t t_adc;

	int32_t st_wt[11];
	int32_t st_ad[11];
	int32_t c_adc;
	int32_t c_wt;
	
	bool    invert;
	bool    compression;
	filter  filt;
}calibration;

extern calibration lc[];

void calibration_init(void);
void calibration_process_adc(void);
	
#endif
/*
*/

