#ifndef __modbus_rtu_h
#define __modbus_rtu_h

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

bool modbus_rtu_process(uint8_t * src, uint8_t * dest, uint16_t len);



#endif
