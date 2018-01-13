# include "usbd_user.h"
# include "usbd_cdc_if.h"
#include "string.h"	
#include "stdarg.h"		 
#include "stdio.h"	


uint8_t  USART_PRINTF_Buffer[256];


void usb_printf(char *fmt, ...)
{
	uint16_t len;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)USART_PRINTF_Buffer, fmt, ap);
	va_end(ap);
	
	len = strlen((const char*)USART_PRINTF_Buffer);
	
	CDC_Transmit_FS(USART_PRINTF_Buffer, len);
}


