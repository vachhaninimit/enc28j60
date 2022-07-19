/*
	
	UDP QUERY - LCSB?
	
*/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "udp_server.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UDP_SERVER_PORT    60100   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    60200   /* define the UDP remote connection port */

uint8_t buffer_udp[32] = {0};
uint8_t len = 0;


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_server_init(void)
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_server_receive_callback, NULL);
      }
   }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{

  /* Connect to the remote client */
  udp_connect(upcb, addr, UDP_CLIENT_PORT);
    
  /* Tell the client that we have accepted it */	
	if(p->tot_len >= 5)
	{
		len = pbuf_copy_partial(p, buffer_udp, p->tot_len, 0);
		
		if(buffer_udp[0] == 'L' && buffer_udp[1] == 'C' && buffer_udp[2] == 'S' && buffer_udp[3] == 'B' && buffer_udp[4] == '?')
		{		
			len = sprintf((char *)buffer_udp, "%i", serial_number);
			p->tot_len = len;
			p->len = len;
			p->payload = buffer_udp;
			udp_send(upcb, p);
		}			
	}
		
  /* free the UDP connection, so we can accept new clients */
  udp_disconnect(upcb);
	
  /* Free the p buffer */
  pbuf_free(p);
   
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
