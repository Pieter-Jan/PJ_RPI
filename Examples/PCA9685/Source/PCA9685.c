#include "PCA9685.h"

void PCA9685_Init(void)
{
	PCA9685_SetRegister(MODE1, 0x80);
	sleep(1);
	PCA9685_SetRegister(MODE1, 0x00);
	PCA9685_SetRegister(MODE2, 0x04);
}

void PCA9685_SetRegister(unsigned char regAddr, unsigned char regValue)
{
	BSC0_A = PCA9685_ADDR;

	BSC0_DLEN = 2;
	BSC0_FIFO = (unsigned char)regAddr;
	BSC0_FIFO = (unsigned char)regValue;

	BSC0_S = CLEAR_STATUS;
	BSC0_C = START_WRITE;

	wait_i2c_done();
}

void PCA9685_SetFrequency(unsigned short frequency)
{
	// The PRE_SCALE register can only be set when the SLEEP bit of MODE1 register is set to logic 1.
	PCA9685_SetRegister(MODE1, 0x10);
	PCA9685_SetRegister(PRE_SCALE, clock_frequency/(frequency*4096) - 1);
	PCA9685_SetRegister(MODE1, 0x80);
}

void PCA9685_SetDutyCycle(unsigned char outputNumber, double dutyCycle_percent)
{
	unsigned char toAdd = outputNumber*4;

	// PCA9685 allows us to change the delay too with the LEDn_ON register
	// we don't need this so we just set this to zero
	PCA9685_SetRegister((LED0_ON_L + toAdd), 0x00);
	PCA9685_SetRegister((LED0_ON_H + toAdd), 0x00);

	// Convert duty cycle to 12 bit value
	unsigned short dutyCycle;
	dutyCycle = (dutyCycle_percent/100.0) * 4096;

	PCA9685_SetRegister((LED0_OFF_L + toAdd), dutyCycle);
	PCA9685_SetRegister((LED0_OFF_H + toAdd), (dutyCycle >> 8) & 0x0F);
}
