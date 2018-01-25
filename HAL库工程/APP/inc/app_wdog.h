# ifndef __APP_WDOG_H
# define __APP_WDOG_H

# include "stm32f4xx.h"

void app_wdog_Config(uint16_t feedTime);

void app_wdog_Feed(void);

# endif



