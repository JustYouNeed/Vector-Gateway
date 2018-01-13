# ifndef __APP_SYS_H
# define __APP_SYS_H

# include "bsp.h"

typedef struct 
{
	uint8_t LinkStatus;
	uint8_t LwipInitStatus;
}SystemInfo_Dev;


extern SystemInfo_Dev Sys_Info;

# endif

