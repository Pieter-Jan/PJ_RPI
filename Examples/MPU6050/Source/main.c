#include "PJ_RPI.h"

#include "MPU6050.h"	// Library with all MPU6050 macro's and functions

#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>

#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536

#define filterConstant 0.10             // Complementary filter

void ComplementaryFilter(short accData[3], short gyrData[3], float *pitch, float *roll); 	

int main(int argc, char *argv[])
{
    // Map memory areas			
    if(map_peripheral(&gpio) == -1) {
        printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
        return -1;
    }
    if(map_peripheral(&bsc0) == -1) {
        printf("Failed to map the physical BSC0 (I2C) registers into the virtual memory space.\n");
        return -1;
    }
	
    /* BSC0 is on GPIO 0 & 1 */
    i2c_init();
		
    MPU6050_Init();
    printf("MPU6050 initialized.\n");
 
    // Global variables
    short accData[3], gyrData[3];      	// Short is a 16 bit int!
    short tmp;
    float pitch = 0, roll = 0;
    float pitchOnlyAcc = 0, pitchOnlyGyr = 0;
    bool isPlot = false;
    float time = 0;
    FILE *fileHandle;
    struct timespec tp;
    long startTime, procesTime;

    if(argc > 1) 			// An argument was specified (plot file)
    {
    	// Create file to put the plot data in
    	fileHandle = fopen(argv[1], "w+");
    	printf("File %s was created.\n", argv[1]);
	isPlot = true;
    }

    initscr();				// Initialise ncurses window

    while(1)
    {
	// Get current time
	clock_gettime(CLOCK_REALTIME, &tp);
	startTime = tp.tv_sec*1000000000 + tp.tv_nsec;

	// Read MPU6050 sensor
        MPU6050_Read(&accData[0], &gyrData[0]);
	
	// Switch Axes
	tmp = accData[2];
	accData[2] = -tmp;
	tmp = accData[0];
	accData[0] = -tmp;
 
	// Calculate the pitch and roll with the complementary filter
	ComplementaryFilter(&accData[0], &gyrData[0], &pitch, &roll); 	

	// Calculate pitch and roll with only accelerometer and gyro for plotting reasons
	pitchOnlyGyr += (float)gyrData[0] / 6553.6; 	// Angle around the X-axis
        pitchOnlyAcc = atan2f((float)accData[1], (float)accData[2]) * 180 / M_PI;

	if(isPlot)
	{
		// Write data to plot file
		fprintf(fileHandle, "%f \t %f \t %f \t %f\n", time, pitch, pitchOnlyGyr, pitchOnlyAcc);		
	}

	clear();

        attron(A_STANDOUT);
        mvprintw(1, 2, "MPU6050 Data");		
        attroff(A_STANDOUT);

 	mvprintw(3, 5, "Accelerometer X: %f", (float)accData[0]/ACCELEROMETER_SENSITIVITY);
	mvprintw(4, 5, "              Y: %f", (float)accData[1]/ACCELEROMETER_SENSITIVITY);
	mvprintw(5, 5, "              Z: %f", (float)accData[2]/ACCELEROMETER_SENSITIVITY);
	
	mvprintw(3, 40, "Gyroscope RX: %f", (float)gyrData[0]/GYROSCOPE_SENSITIVITY);
	mvprintw(4, 40, "          RY: %f", (float)gyrData[1]/GYROSCOPE_SENSITIVITY);
	mvprintw(5, 40, "          RZ: %f", (float)gyrData[2]/GYROSCOPE_SENSITIVITY);

        attron(A_STANDOUT);
        mvprintw(7, 2, "Complementary Filter");		
        attroff(A_STANDOUT);

	mvprintw(9, 5, "Pitch: %f", pitch);
	mvprintw(10, 5, "Roll:  %f", roll);
 
	refresh();
	
	clock_gettime(CLOCK_REALTIME, &tp);
	procesTime = (tp.tv_sec*1000000000 + tp.tv_nsec) - startTime;

	time += 0.01;

	while(procesTime < 10000000)	// Wait for 10 ms
	{
	    clock_gettime(CLOCK_REALTIME, &tp);
	    procesTime = (tp.tv_sec*1000000000 + tp.tv_nsec) - startTime;
	}
    }
    endwin();
}

void ComplementaryFilter(short accData[3], short gyrData[3], float *pitch, float *roll)
{
    float pitchAcc, rollAcc;               

    // Integrate the gyroscope data -> int(angularSpeed) = angle
    *pitch += (float)gyrData[0] / 6553.6; 	// Angle around the X-axis
    *roll -= (float)gyrData[1] / 6553.6;	// Angle around the Y-axis

    // Compensate for drift with accelerometer data if !bullshit
    // Sensitivity = -2 to 2 G at 16Bit -> 2G = 32768 && 0.5G = 8192
    int forceMagnitudeApprox = abs(accData[0]) + abs(accData[1]) + abs(accData[2]);
    if (forceMagnitudeApprox > 8192 && forceMagnitudeApprox < 32768)
    {
	// Turning around the X axis results in a vector on the Y-axis
        pitchAcc = atan2f((float)accData[1], (float)accData[2]) * 180 / M_PI;
        *pitch = *pitch * (1 - filterConstant) + pitchAcc * filterConstant;

	// Turning around the Y axis results in a vector on the X-axis
        rollAcc = atan2f((float)accData[0], (float)accData[2]) * 180 / M_PI;
        *roll = *roll * (1 - filterConstant) + rollAcc * filterConstant;
    }
} 	
