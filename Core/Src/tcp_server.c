#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "tcp_server.h"
#include "uart.h"
#include "modbus_rtu.h"

#define TCP_SERVER_PORT    502   /* define the UDP local connection port */

uint8_t buffer_len    = 0;
uint8_t buffer[128]   = {0};
uint8_t buffer_d[128] = {0};

#if LWIP_TCP

static struct tcp_pcb *tcp_server_pcb;

/* ECHO protocol states */
enum tcp_server_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

struct tcp_server_struct
{
  u8_t state;             /* current connection state */
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};


static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void  tcp_server_error(void *arg, err_t err);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void  tcp_server_send(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
static void  tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
static uint16_t tcp_server_process_message(uint8_t * src, uint8_t * des);

/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
void tcp_server_init(void)
{
  /* create new tcp pcb */
  tcp_server_pcb = tcp_new();

  if (tcp_server_pcb != NULL)
  {
    err_t err;
    
    err = tcp_bind(tcp_server_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);
    
    if (err == ERR_OK)
    {
      /* start tcp listening for echo_pcb */
      tcp_server_pcb = tcp_listen(tcp_server_pcb);
      
      /* initialize LwIP tcp_accept callback function */
      tcp_accept(tcp_server_pcb, tcp_server_accept);
    }
    else 
    {
      /* deallocate the pcb */
      memp_free(MEMP_TCP_PCB, tcp_server_pcb);
    }
  }
}

/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used 
  * @retval err_t: error status
  */
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_server_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* allocate structure es to maintain tcp connection informations */
  es = (struct tcp_server_struct *)mem_malloc(sizeof(struct tcp_server_struct));
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->p = NULL;
    
    /* pass newly allocated es structure as argument to newpcb */
    tcp_arg(newpcb, es);
    
    /* initialize lwip tcp_recv callback function for newpcb  */ 
    tcp_recv(newpcb, tcp_server_recv);
    
    /* initialize lwip tcp_err callback function for newpcb  */
    tcp_err(newpcb, tcp_server_error);
    
    /* initialize lwip tcp_poll callback function for newpcb */
    tcp_poll(newpcb, tcp_server_poll, 1);
    
    ret_err = ERR_OK;
  }
  else
  {
    /*  close tcp connection */
    tcp_server_connection_close(newpcb, es);
    /* return memory error */
    ret_err = ERR_MEM;
  }
  return ret_err;  
}


/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct tcp_server_struct *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct tcp_server_struct *)arg;
  
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
       /* we're done sending, close connection */
       tcp_server_connection_close(tpcb, es);
    }
    else
    {
      /* we're not done yet */
      /* acknowledge received packet */
      tcp_sent(tpcb, tcp_server_sent);
      
      /* send remaining data*/
      tcp_server_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      es->p = NULL;
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)
  {
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    
    /* store reference to incoming pbuf (chain) */
    es->p = p;
		
 		buffer_len = 	pbuf_copy_partial(p, buffer, p->tot_len, 0);
		
		if(buffer_len > 0)
		{
			//------------------------------------------------------
			buffer_len = tcp_server_process_message(buffer, buffer_d);
			if(buffer_len > 0)
			{
				es->p->payload = buffer;
				es->p->len = buffer_len;
				tcp_sent(tpcb, tcp_server_sent);
			}
		}   
		
    /* initialize LwIP tcp_sent callback function */
    //tcp_sent(tpcb, tcp_server_sent);

    /* send back the received data (echo) */
    //tcp_server_send(tpcb, es);
		
    ret_err = ERR_OK;
  }
  else if (es->state == ES_RECEIVED)
  {

    /* more data received from client and previous data has been already sent*/
    if(es->p == NULL)
    {
      es->p = p;
			//tcp_server_send(tpcb, es);	
			buffer_len = 	pbuf_copy_partial(p, buffer, p->tot_len, 0);

			if(buffer_len > 0)
			{
				//------------------------------------------------------	
				buffer_len = tcp_server_process_message(buffer, buffer_d);
				if(buffer_len > 0)
				{
					es->p->payload = buffer;
					es->p->len = buffer_len;			
					//tcp_output(tpcb);
					tcp_server_send(tpcb, es);							
				}
			}
      /* send back received data */
    }
    else
    {
      struct pbuf *ptr;

      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  }  
  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing */
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs. 
  * @param  arg: pointer on argument parameter 
  * @param  err: not used
  * @retval None
  */
static void tcp_server_error(void *arg, err_t err)
{
  struct tcp_server_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_server_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_server_struct *es;

  es = (struct tcp_server_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_server_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        tcp_server_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  None
  * @retval None
  */
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_server_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_server_struct *)arg;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send */
    tcp_server_send(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection*/
    if(es->state == ES_CLOSING)
      tcp_server_connection_close(tpcb, es);
  }
  return ERR_OK;
}


/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_server_send(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p;
		
		
    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
		tcp_output(tpcb);	
    if (wr_err == ERR_OK)
    {	
      u16_t plen;

      plen = ptr->len;
     
      /* continue with next pbuf in chain (if any) */
      es->p = ptr->next;
      
      if(es->p != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p);
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);

      /* Update tcp window size to be advertized : should be called when received
      data (with the amount plen) has been processed by the application layer */
      tcp_recved(tpcb, plen);				
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later / harder, defer to poll */
     es->p = ptr;
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (es != NULL)
  {
    mem_free(es);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);
}
	uint8_t len_hi, len_lo = 0;
static uint16_t tcp_server_process_message(uint8_t * src, uint8_t * des)
{
	uint16_t _len = 0;
	uint8_t t_id_hi, t_id_lo = 0;

	
	t_id_hi = src[0];
	t_id_lo = src[1];
	
	len_hi  = src[4];
	len_lo  = src[5];
	
	if(src[6] == slave_id)
	{
		memcpy(src, src + 6, ((len_hi << 8)|len_lo));
		if(modbus_rtu_process(src, des, ((len_hi << 8)|len_lo)))
		{
			src[0] = t_id_hi;
			src[1] = t_id_lo;
			src[2] = 0;
			src[3] = 0;
			
			src[4] = ((des[2] + 3) >> 8);
			src[5] = (des[2] + 3) & 0xFF;
			
			switch(des[1])
			{
				case 3:
					memcpy(src + 6 , des, des[2] + 3);	
					_len = des[2] + 3 + 6;					
					break;
				
				case 6:
					memcpy(src + 6 , des, 6);
					_len = 12;	
					break;
				
				case 16:	
					src[4] = (((des[4] >> 8) | des[5]) * 2) >> 8;
					src[5] = (((des[4] >> 8) | des[5]) * 2) & 0xFF;				
					memcpy(src + 6 , des, 6);	
					_len = 12;	
					break;				
			}	
		}
	}		
	return _len;
}

#endif /* LWIP_TCP */
