#ifndef __ethernetif_h
#define __ethernetif_h

#include <stdbool.h>
#include "stm32l1xx_hal.h"
#include "lwip/err.h"
#include "lwip/netif.h"

extern bool     dhcp_enable;

extern uint8_t  ipadd1;
extern uint8_t  ipadd2;
extern uint8_t  ipadd3;
extern uint8_t  ipadd4;

extern uint8_t  netmask1;
extern uint8_t  netmask2;
extern uint8_t  netmask3;
extern uint8_t  netmask4;

extern uint8_t  gateway1;
extern uint8_t  gateway2;
extern uint8_t  gateway3;
extern uint8_t  gateway4;

/* Exported types ------------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif);
void ethernetif_set_peripherals(SPI_HandleTypeDef * spi, GPIO_TypeDef *gp, uint16_t gp_pin);
void ethernetif_dhcp_periodic_handle(void);
void ethernetif_input(void);
void ethernet_transmit(void);

#endif
