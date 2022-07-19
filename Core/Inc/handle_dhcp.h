#ifndef __HANDLE_DHCP_H__
#define __HANDLE_DHCP_H__

#include "lwip/dhcp.h"

#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

void handle_dhcp(struct netif *netif);
void handle_dhcp_periodic_handle(struct netif *netif);
	
#endif /* __MINIMAL_ECHO_H */



