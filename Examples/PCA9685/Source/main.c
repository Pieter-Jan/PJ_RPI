#include "PJ_RPI.h"
#include "PCA9685.h"
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>

struct timespec tp;
long startTime, procesTime;

int main()
{
	// Gain access to peripheral memory structures
    if(map_peripheral(&gpio) == -1) {
        printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
        return -1;
    }
    if(map_peripheral(&bsc0) == -1) {
        printf("Failed to map the physical BSC0 (I2C) registers into the virtual memory space.\n");
        return -1;
    }
 
    // I2C initialization
    i2c_init();

	INP_GPIO(17);
	OUT_GPIO(17);
	GPIO_CLR = 1 << 17;

	// Initialize Hardware
	PCA9685_Init();

	// Set PWM Frequency to 100Hz
	PCA9685_SetFrequency(100);

	while(1)
	{
		// Get process start time
		clock_gettime(CLOCK_REALTIME, &tp);
		startTime = tp.tv_sec*1000000000 + tp.tv_nsec;

		// Update PWM ; Convert percentage from O -> 100 to 10 -> 20
		PCA9685_SetDutyCycle(0, 20);
		PCA9685_SetDutyCycle(1, 10);
		PCA9685_SetDutyCycle(2, 50);
		PCA9685_SetDutyCycle(3, 50);
		
		GPIO_SET = 1 << 17;

		// Get process end time
		clock_gettime(CLOCK_REALTIME, &tp);
		procesTime = (tp.tv_sec*1000000000 + tp.tv_nsec) - startTime;
		// Wait until process has lasted 10 ms
		while(procesTime < 10000000)
		{
			clock_gettime(CLOCK_REALTIME, &tp);
			procesTime = (tp.tv_sec*1000000000 + tp.tv_nsec) - startTime;
		}
	}
}  
