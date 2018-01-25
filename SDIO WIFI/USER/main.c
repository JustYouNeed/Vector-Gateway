# include "bsp.h"
# include "bsp_wlan.h"

int main(void)
{
	bsp_Config();
	bsp_wlan_Config();
//	bsp_sd_Config();
	while(1)
	{
		bsp_led_Toggle(2);
		bsp_tim_DelayMs(100);
	//	printf("nihao\r\n");
	}
}
