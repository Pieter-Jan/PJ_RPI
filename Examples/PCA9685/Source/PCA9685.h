#ifndef _INC_PCA9685_H
#define _INC_PCA9685_H

#include "PJ_RPI.h"
#include <stdio.h>

#define PCA9685_ADDR 0b01000000 	// 7 bit adress
#define clock_frequency 25000000.0	// 25 MHz internal clock

// Registers
#define PRE_SCALE 0xFE
#define MODE1 0x00
#define MODE2 0x01

#define LED0_ON_L 0x06				// All other led registers will be defined relative to this one
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09

void PCA9685_Init(void);
void PCA9685_SetRegister(unsigned char regAddr, unsigned char regValue);

void PCA9685_SetFrequency(unsigned short frequency);
void PCA9685_SetDutyCycle(unsigned char outputNumber, double dutyCycle_percent);

#endif 