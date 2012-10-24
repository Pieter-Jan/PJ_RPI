#include "PJ_RPI_GPIO.h"
#include <stdio.h>

int main()
{
	if(map_peripheral(&gpio) == -1) 
	{
       	 	printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
        	return -1;
    	}

	printf("WtF!\n");

	// Define pin 7 as output
	INP_GPIO(4);
	OUT_GPIO(4);

	while(1)
	{
		// Toggle pin 7 (blink a led!)
		GPIO_SET = 1 << 4;
		sleep(1);

		GPIO_CLR = 1 << 4;
		sleep(1);
	}

	return 0;	

}
