# ifndef __APP_SYS_H
# define __APP_SYS_H

# include "bsp.h"

typedef enum
{
	USB_IDLE = 0,
	USB_CONNECTED,
	USB_DISCONNECT,
	USB_READY,
	USB_ACTIVE,
}USB_TypeDef;

typedef struct 
{
	uint8_t LinkStatus;
	uint8_t LwipInitStatus;
	uint8_t FTPServerStatus;
	uint8_t FlashDiskStatus;
	uint8_t USBDiskStatus;
	
	USB_TypeDef USBH_State;
	USB_TypeDef USBD_State;
}SystemInfo_Dev;


extern SystemInfo_Dev Sys_Info;
void app_sys_Config(void);

# endif

