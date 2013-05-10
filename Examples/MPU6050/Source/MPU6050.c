#include "MPU6050.h"

void MPU6050_Init(void)
{
//    MPU6050_SetRegister(PWR_MGMT_1, 0x80);	// Device Reset
    MPU6050_SetRegister(PWR_MGMT_1, 0x00); 	// Clear sleep bit
    MPU6050_SetRegister(CONFIG, 0x00); 	
    MPU6050_SetRegister(GYRO_CONFIG, 0x08);
    MPU6050_SetRegister(ACCEL_CONFIG, 0x08);
}

void MPU6050_SetRegister(unsigned char regAddr, unsigned char regValue)
{
    // See datasheet (PS) page 36: Single Byte Write Sequence

    // Master:   S  AD+W       RA       DATA       P
    // Slave :            ACK      ACK        ACK
	
    BSC0_A = MPU6050_ADDR;

    BSC0_DLEN = 2;
    BSC0_FIFO = (unsigned char)regAddr;
    BSC0_FIFO = (unsigned char)regValue;
	
    BSC0_S = CLEAR_STATUS; 	// Reset status bits (see #define)
    BSC0_C = START_WRITE;    	// Start Write (see #define)

    wait_i2c_done();
}

void MPU6050_Read(short * accData, short * gyrData)
{
    // See datasheet (PS) page 37: Burst Byte Read Sequence

    // Master:   S  AD+W       RA       S  AD+R           ACK        NACK  P
    // Slave :            ACK      ACK          ACK DATA       DATA

    BSC0_DLEN = 1;    		// one byte
    BSC0_FIFO = 0x3B;    	// value of first register
    BSC0_S = CLEAR_STATUS; 	// Reset status bits (see #define)
    BSC0_C = START_WRITE;    	// Start Write (see #define)

    wait_i2c_done();

    BSC0_DLEN = 14;

    BSC0_S = CLEAR_STATUS;	// Reset status bits (see #define)
    BSC0_C = START_READ;    	// Start Read after clearing FIFO (see #define)

    wait_i2c_done();

    short tmp;

    int i = 0;	
    for(i; i < 3; i++) 		// Accelerometer
    {
	tmp = BSC0_FIFO << 8;	
	tmp += BSC0_FIFO;
	accData[i] = tmp; 
    }
    
    tmp = BSC0_FIFO << 8; 	// Temperature
    tmp += BSC0_FIFO;	

    i = 0;	
    for(i; i < 3; i++)		// Gyroscope
    {
	tmp = BSC0_FIFO << 8;
	tmp += BSC0_FIFO;
	gyrData[i] = tmp; 
    }
}
