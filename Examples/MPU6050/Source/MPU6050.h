#ifndef _INC_MPU6050_H
#define _INC_MPU6050_H

#include "PJ_RPI.h"

#define MPU6050_ADDR 0x68	// 7 bit adres 
//#define MPU6050_ADDR 0x69     //One of these address should work. Depends on module HW address
#define CONFIG 0x1A
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B
#define PWR_MGMT_1 0x6B

void MPU6050_Read(short * accData, short * gyrData);
void MPU6050_Init(void);
void MPU6050_SetRegister(unsigned char regAddr, unsigned char regValue);

#endif
