/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>

#include "ethernetif.h"
#include "enc28j60.h"
#include "main.h"
#include "eeprom.h"

#include "lwip/opt.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Network interface name */
#define IFNAME0 'e'
#define IFNAME1 'n'

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ENC_HandleTypeDef   EncHandle;
SPI_HandleTypeDef * hspi_eth;
GPIO_TypeDef      * CS_GPIO;
uint16_t            CS_GPIO_PIN;

bool      dhcp_enable = 1;

ip_addr_t ipaddr;
ip_addr_t netmask;
ip_addr_t gw;
	
uint8_t  ipadd1       = 192;
uint8_t  ipadd2       = 168;
uint8_t  ipadd3       = 1;
uint8_t  ipadd4       = 200;

uint8_t  netmask1     = 255;
uint8_t  netmask2     = 255;
uint8_t  netmask3     = 255;
uint8_t  netmask4     = 0;

uint8_t  gateway1     = 192;
uint8_t  gateway2     = 168;
uint8_t  gateway3     = 1;
uint8_t  gateway4     = 1;

uint8_t  macaddress[6]= { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x07};

#define  DHCP_OFF                   (uint8_t) 0
#define  DHCP_START                 (uint8_t) 1
#define  DHCP_WAIT_ADDRESS          (uint8_t) 2
#define  DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define  DHCP_TIMEOUT               (uint8_t) 4
#define  DHCP_LINK_DOWN             (uint8_t) 5

#define  MAX_DHCP_TRIES 4

uint32_t DHCPfineTimer  = 0;

__IO uint8_t  DHCP_state     = DHCP_OFF;
	
struct netif gnetif;

#ifdef USE_PROTOTHREADS
static struct pt transmit_pt;
#endif

void ethernetif_config(bool use_dhcp);
void ethernetif_dhcp_process(struct netif *netif);
void ethernetif_update_config(struct netif *netif);	
void ethernetif_notify_conn_changed(struct netif *netif);

void ethernetif_set_peripherals(SPI_HandleTypeDef * spi, GPIO_TypeDef *gp, uint16_t gp_pin)
{
	hspi_eth 		    = spi;
	CS_GPIO 	  	  = gp;
	CS_GPIO_PIN 		= gp_pin;
	
	eeprom_read_network_settings();
	
	dhcp_enable = eeprom_read_byte(0);
	
	eeprom_write_byte(0, dhcp_enable);
	
	ethernetif_config(dhcp_enable);
}
/*


*/
void ethernetif_config(bool use_dhcp)
{  
	if(use_dhcp)
	{
		ip_addr_set_zero_ip4(&ipaddr);
		ip_addr_set_zero_ip4(&netmask);
		ip_addr_set_zero_ip4(&gw);
	}
	else
	{	
		IP_ADDR4( &ipaddr,   ipadd1,   ipadd2,   ipadd3,   ipadd4);
		IP_ADDR4(&netmask, netmask1, netmask2, netmask3, netmask4);
		IP_ADDR4(     &gw, gateway1, gateway2, gateway3, gateway4);
	}

  /* add the network interface */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /*  Registers the default network interface */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
		
		if(use_dhcp)
		{
			DHCP_state = DHCP_START;
		}
		else
		{
			DHCP_state = DHCP_LINK_DOWN;
		}
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernetif_update_config);	
	
	
}
/*

*/
static void ethernetif_low_level_init(struct netif *netif)
{


  /* Initialize transmit protothread */
	#ifdef USE_PROTOTHREADS
		PT_INIT(&transmit_pt);
	#endif

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  macaddress[0];
  netif->hwaddr[1] =  macaddress[1];
  netif->hwaddr[2] =  macaddress[2];
  netif->hwaddr[3] =  macaddress[3];
  netif->hwaddr[4] =  macaddress[4];
  netif->hwaddr[5] =  macaddress[5];

  EncHandle.Init.MACAddr = netif->hwaddr;
  EncHandle.Init.DuplexMode = ETH_MODE_HALFDUPLEX;
  EncHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  EncHandle.Init.InterruptEnableBits =  EIE_LINKIE;	

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
	
//	#ifdef LWIP_IGMP
//		netif->flags |= NETIF_FLAG_IGMP;
//	#endif	
	
  /* Start the EN28J60 module */
  if (ENC_Start(&EncHandle)) 
	{
    /* Set the MAC address */
    ENC_SetMacAddr(&EncHandle);

    /* Set netif link flag */
    netif->flags |= NETIF_FLAG_LINK_UP;
  }
}
/*

*/
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    /* TODO use netif to check if we are the right ethernet interface */
  err_t errval;
  struct pbuf *q;
  uint32_t framelength = 0;

	#ifdef USE_PROTOTHREADS
		if (EncHandle.transmitLength != 0) 
		{
			while (PT_SCHEDULE(ENC_Transmit(&transmit_pt, &EncHandle))) 
			{/* Wait for end of previous transmission */}
		}
	#endif

  /* Prepare ENC28J60 Tx buffer */
  errval = ENC_RestoreTXBuffer(&EncHandle, p->tot_len);
  if (errval != ERR_OK) 
	{
      return errval;
  }

  /* copy frame from pbufs to driver buffers and send packet */
  for(q = p; q != NULL; q = q->next) 
	{
    ENC_WriteBuffer(q->payload, q->len);
    framelength += q->len;
  }

  if (framelength != p->tot_len) 
	{
     return ERR_BUF;
  }

  EncHandle.transmitLength = p->tot_len;
  /* Actual transmission is triggered in main loop */

  return ERR_OK;
}

/*

*/
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p = NULL;
  struct pbuf *q;
  uint16_t len;
  uint8_t *buffer;
  uint32_t bufferoffset = 0;

  if (!ENC_GetReceivedFrame(&EncHandle)) 
	{
    return NULL;
  }

  /* Obtain the size of the packet and put it into the "len" variable. */
  len = EncHandle.RxFrameInfos.length;
  buffer = (uint8_t *)EncHandle.RxFrameInfos.buffer;

  if (len > 0)
  {
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  }

  if (p != NULL)
  {
    bufferoffset = 0;

    for(q = p; q != NULL; q = q->next)
    {
      /* Copy data in pbuf */
      memcpy( (uint8_t*)((uint8_t*)q->payload), (uint8_t*)((uint8_t*)buffer + bufferoffset), q->len);
      bufferoffset = bufferoffset + q->len;
    }
  }

  return p;
}

/*

*/
void ethernetif_input(void)
{
  err_t err;
  struct pbuf *p;
	
	struct netif *netif = &gnetif;
	
  /* move received packet into a new pbuf */
  p = low_level_input(netif);

  /* no packet could be read, silently ignore this */
  if (p == NULL) return;

  /* entry point to the LwIP stack */
  err = netif->input(p, netif);

  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }
}

/*

*/
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

	#if LWIP_NETIF_HOSTNAME
		/* Initialize interface hostname */
		netif->hostname = "stm32idisco";
	#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  ethernetif_low_level_init(netif);

  return ERR_OK;
}

/*

*/
u32_t sys_now(void)
{
  return HAL_GetTick();
}
/*

*/
void ethernetif_set_link(struct netif *netif)
{
	/* Handle ENC28J60 interrupt */
	ENC_IRQHandler(&EncHandle);

	/* Check whether the link is up or down*/
	if(((EncHandle.LinkStatus) & PHSTAT2_LSTAT)!= 0) 
	{
		netif_set_link_up(netif);
	} else 
	{
		netif_set_link_down(netif);
	}

	/* Reenable interrupts */
	ENC_EnableInterrupts(EIE_INTIE);
}

/*

*/
void ethernetif_update_config(struct netif *netif)
{
  if(netif_is_link_up(netif)) 
	{
		/* Restart the EN28J60 module */
		ethernetif_low_level_init(netif);
  }

  ethernetif_notify_conn_changed(netif);
}
/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_notify_conn_changed(struct netif *netif)
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
/*

*/
void ethernet_transmit(void) {
#ifdef USE_PROTOTHREADS
    ENC_Transmit(&transmit_pt, &EncHandle);
#else
    ENC_Transmit(&EncHandle);
#endif
}
/*

*/
uint8_t ENC_SPI_SendWithoutSelection(uint8_t command)
{
    HAL_SPI_TransmitReceive(hspi_eth, &command, &command, 1, HAL_MAX_DELAY);
    return command;
}
/*

*/
uint8_t ENC_SPI_Send(uint8_t command)
{
	/* Select ENC28J60 module */	
	HAL_GPIO_WritePin(CS_GPIO, CS_GPIO_PIN, GPIO_PIN_RESET);
	up_udelay(1);
	HAL_SPI_TransmitReceive(hspi_eth, &command, &command, 1, HAL_MAX_DELAY);
	/* De-select ENC28J60 module */
	HAL_GPIO_WritePin(CS_GPIO, CS_GPIO_PIN, GPIO_PIN_SET);
	up_udelay(1);	
	return command;
}
/*

*/
void ENC_SPI_SendBuf(uint8_t *master2slave, uint8_t *slave2master, uint16_t bufferSize)
{
	/* Select ENC28J60 module */	
	HAL_GPIO_WritePin(CS_GPIO, CS_GPIO_PIN, GPIO_PIN_RESET);
	up_udelay(1);

	/* Transmit or receuve data */
	if (slave2master == NULL) 
	{
		if (master2slave != NULL) 
		{
			HAL_SPI_Transmit(hspi_eth, master2slave, bufferSize, HAL_MAX_DELAY);
		}
	} 
	else if (master2slave == NULL) 
	{
		HAL_SPI_Receive(hspi_eth, slave2master, bufferSize, HAL_MAX_DELAY);
	} 
	else 
	{
		HAL_SPI_TransmitReceive(hspi_eth, master2slave, slave2master, bufferSize, HAL_MAX_DELAY);
	}

	/* De-select ENC28J60 module */
	HAL_GPIO_WritePin(CS_GPIO , CS_GPIO_PIN, GPIO_PIN_SET);
	up_udelay(1);
}
/*

*/
void ENC_SPI_Select(bool select)
{
    /* Select or de-select ENC28J60 module */
    if (select) {
        HAL_GPIO_WritePin(CS_GPIO , CS_GPIO_PIN, GPIO_PIN_RESET);
        up_udelay(1);
    } else {
        HAL_GPIO_WritePin(CS_GPIO, CS_GPIO_PIN, GPIO_PIN_SET);
        up_udelay(1);
    }
}




void ethernetif_dhcp_periodic_handle(void)
{
	if(dhcp_enable)
	{
		/* Fine DHCP periodic process every 500ms */
		if (HAL_GetTick() - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
		{
			DHCPfineTimer =  HAL_GetTick();
			/* process DHCP state machine */
			ethernetif_dhcp_process(&gnetif);    
		}
	}
}
void ethernetif_dhcp_process(struct netif *netif)
{
  struct dhcp *dhcp;   
  
  switch (DHCP_state)
  {
    case DHCP_START:
    {
      DHCP_state = DHCP_WAIT_ADDRESS;
      dhcp_start(netif);
    }
    break;
    
  case DHCP_WAIT_ADDRESS:
    {
      if(dhcp_supplied_address(netif)) 
      { 
        DHCP_state = DHCP_ADDRESS_ASSIGNED;			
				ipadd1   = netif->ip_addr.addr & 0xFF;
				ipadd2   = netif->ip_addr.addr >> 8& 0xFF;
				ipadd3   = netif->ip_addr.addr >> 16& 0xFF;
				ipadd4   = netif->ip_addr.addr >> 24& 0xFF;
			
				netmask1 = netif->netmask.addr & 0xFF;
				netmask2 = netif->netmask.addr >> 8& 0xFF;
				netmask3 = netif->netmask.addr >> 16& 0xFF;
				netmask4 = netif->netmask.addr >> 24& 0xFF;

				gateway1 = netif->gw.addr  & 0xFF;
				gateway2 = netif->gw.addr >> 8& 0xFF;
				gateway3 = netif->gw.addr >> 16& 0xFF;
				gateway4 = netif->gw.addr >> 24& 0xFF;				
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
          IP_ADDR4      ( &ipaddr,   ipadd1,   ipadd2,   ipadd3,   ipadd4);
          IP_ADDR4      (&netmask, netmask1, netmask2, netmask3, netmask4);
          IP_ADDR4      (     &gw, gateway1, gateway2, gateway3, gateway4);
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
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
