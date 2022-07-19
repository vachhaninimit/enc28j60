#include "main.h"
#include "handle_dhcp.h"


//#ifdef USE_DHCP
	#define          MAX_DHCP_TRIES 4
	        uint32_t DHCPfineTimer  = 0;
	__IO    uint8_t  DHCP_state     = DHCP_OFF;
//#endif

void handle_dhcp_process(struct netif *netif);
	
void handle_dhcp(struct netif *netif)
{
  if (netif_is_link_up(netif))
 {
 	//#ifdef USE_DHCP
   /* Update DHCP state machine */
    DHCP_state = DHCP_START;
	 //#endif
 }
 else
  {  
		//#ifdef USE_DHCP
			/* Update DHCP state machine */
			DHCP_state = DHCP_LINK_DOWN;
		//#endif
  } 
}

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_notify_conn_changed1(struct netif *netif)
{
//#ifndef USE_DHCP
//  ip_addr_t ipaddr;
//  ip_addr_t netmask;
//  ip_addr_t gw;
//#endif
  
  if(netif_is_link_up(netif))
  {    
		//#ifdef USE_DHCP
				/* Update DHCP state machine */
				DHCP_state = DHCP_START;
		//#else
    //IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    //IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
    //IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);  
    
    //netif_set_addr(netif, &ipaddr , &netmask, &gw);  
    
//#endif /* USE_DHCP */   
    
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(netif);     
  }
  else
  {
//#ifdef USE_DHCP
    /* Update DHCP state machine */
    DHCP_state = DHCP_LINK_DOWN;
//#endif /* USE_DHCP */
    
    /*  When the netif link is down this function must be called.*/
    netif_set_down(netif);
  }
}

//#ifdef USE_DHCP
/**
  * @brief  DHCP_Process_Handle
  * @param  None
  * @retval None
  */
 
void handle_dhcp_process(struct netif *netif)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  struct dhcp *dhcp;   
  
  switch (DHCP_state)
  {
    case DHCP_START:
    {
      ip_addr_set_zero_ip4(&netif->ip_addr);
      ip_addr_set_zero_ip4(&netif->netmask);
      ip_addr_set_zero_ip4(&netif->gw);
      DHCP_state = DHCP_WAIT_ADDRESS;
      dhcp_start(netif);
    }
    break;
    
  case DHCP_WAIT_ADDRESS:
    {
      if (dhcp_supplied_address(netif)) 
      { 
        DHCP_state = DHCP_ADDRESS_ASSIGNED;				
      }
      else
      {
        dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    
        /* DHCP timeout */
        if (dhcp->tries > MAX_DHCP_TRIES)
        {
          DHCP_state = DHCP_TIMEOUT;
          
          /* Stop DHCP */
          dhcp_stop(netif);
          
          /* Static address used */
//          IP_ADDR4      ( &ipaddr,   ipadd1,   ipadd2,   ipadd3,   ipadd4);
//          IP_ADDR4      (&netmask, netmask1, netmask2, netmask3, netmask4);
//          IP_ADDR4      (     &gw, gateway1, gateway2, gateway3, gateway4);
          netif_set_addr(   netif,  &ipaddr, &netmask, &gw);
        }
      }
    }
    break;
		
  case DHCP_LINK_DOWN:
    {
      /* Stop DHCP */
      dhcp_stop(netif);
      DHCP_state = DHCP_OFF; 
    }
    break;
		
  default: break;
  }
}
/**
  * @brief  DHCP periodic check
  * @param  localtime the current LocalTime value
  * @retval None
  */
void handle_dhcp_periodic_handle(struct netif *netif)
{  
  /* Fine DHCP periodic process every 500ms */
  if (HAL_GetTick() - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
  {
    DHCPfineTimer =  HAL_GetTick();
    /* process DHCP state machine */
    handle_dhcp_process(netif);    
  }
}
//#endif

