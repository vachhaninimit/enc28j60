#include <math.h>
#include "calibration.h"
#include "main.h"
#include "eeprom.h"

uint8_t     lc_index = 0;
calibration lc[3];

void 		calibration_init_adc(calibration *cal);
void 		calibration_init_filter(filter *filt, float mea_e, float est_e, float p, float intial_value);
void    calibration_get_adc(calibration *cal);
void 		calibration_update_filter(filter * filt);
void    calibration_process(calibration *cal);
int32_t calibration_net_adc(int32_t curAdc, int32_t tareAdc);
int32_t calibration_two_points(int32_t curAdc, calibration *cal);

void calibration_init(void)
{	

}
/*

*/
void calibration_init_adc(calibration *cal)
{

}
/*0

*/
void calibration_init_filter(filter *filt, float mea_e, float est_e, float p, float intial_value)
{

}
/*

*/
void calibration_process_adc(void)
{	

}
/*

*/
void calibration_get_adc(calibration *cal)
{

}
/*

*/
void calibration_update_filter(filter * filt)
{

}
/*

*/
void calibration_process(calibration *cal)
{

	
}
/*

*/
int32_t calibration_net_adc(int32_t curAdc, int32_t tareAdc)
{
	float tmp = curAdc;

	
	return (int32_t)tmp;
}
/*

*/
int32_t calibration_two_points(int32_t curAdc, calibration *cal)
{
	float tmp1 = curAdc;

	return (int32_t) tmp1;
}
/*
	END OF FILE
*/
