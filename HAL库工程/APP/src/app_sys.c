# include "app_sys.h"

SystemInfo_Dev Sys_Info;

/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void app_sys_Config(void)
{
	Sys_Info.LinkStatus = 0;
	Sys_Info.LwipInitStatus = 0;
	Sys_Info.FlashDiskStatus = 0;
	Sys_Info.FTPServerStatus = 0;	
	Sys_Info.USBD_State = USB_DISCONNECT;
	Sys_Info.USBH_State = USB_DISCONNECT;
}
