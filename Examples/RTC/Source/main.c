#include "PJ_RPI.h"
#include <stdio.h>
#include <time.h>
#include <fcntl.h>

int systohc = 0;
int hctosys = 0;
struct tm t;
time_t now;

// BCD helper functions only apply to BCD 0-99 (one byte) values
unsigned int bcdtod(unsigned int bcd) {
    return ((bcd & 0xf0) >> 4) * 10 + (bcd & 0x0f);
}
unsigned int dtobcd(unsigned int d) {
    return ((d / 10) << 4) + (d % 10);
}

int main(int argc, char *argv[]) {

    if(argc == 2) {
        if(!strcmp(argv[1], "-w"))
            systohc = 1;
        if(!strcmp(argv[1], "-s"))
            hctosys = 1;
    }

    if(map_peripheral(&gpio) == -1) {
        printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
        return -1;
    }
    if(map_peripheral(&bsc0) == -1) {
        printf("Failed to map the physical BSC0 (I2C) registers into the virtual memory space.\n");
        return -1;
    }

    i2c_init();
		
    // I2C Device Address 0x51 (hardwired into the RTC chip)
    BSC0_A = 0x51;

    if(systohc) {

        printf("Setting RTC from system clock\n");

        now = time(NULL);
        gmtime_r(&now, &t);    // explode time_t (now) into an struct tm

        //////
        // Write Operation to set the time (writing 15 of the 16 RTC registers to
        // also reset status, alarm, and timer settings).
        //////
        BSC0_DLEN = 16;
        BSC0_FIFO = 0;        			// Addr 0
        BSC0_FIFO = 0;        			// control1
        BSC0_FIFO = 0;        			// control2
        BSC0_FIFO = dtobcd(t.tm_sec);		// seconds
        BSC0_FIFO = dtobcd(t.tm_min);   	// mins
        BSC0_FIFO = dtobcd(t.tm_hour);  	// hours
        BSC0_FIFO = dtobcd(t.tm_mday);  	// days
        BSC0_FIFO = dtobcd(t.tm_wday);  	// weekdays (sun 0)
        BSC0_FIFO = dtobcd(t.tm_mon + 1);    	// months 0-11 --> 1-12
        BSC0_FIFO = dtobcd(t.tm_year - 100);    // years
        BSC0_FIFO = 0x0;    			// alarm min
        BSC0_FIFO = 0x0;    			// alarm hour
        BSC0_FIFO = 0x0;    			// alarm day
        BSC0_FIFO = 0x0;    			// alarm weekday
        BSC0_FIFO = 0x0;    			// CLKOUT control
        BSC0_FIFO = 0x0;    			// timer control

        BSC0_S = CLEAR_STATUS; 			// Reset status bits (see #define)
        BSC0_C = START_WRITE;    		// Start Write (see #define)

        wait_i2c_done();
    }

    /* Write operation to restart the PCF8563 register at index 2 ('secs' field) */
    BSC0_DLEN = 1;    // one byte
    BSC0_FIFO = 2;    // value 2
    BSC0_S = CLEAR_STATUS; // Reset status bits (see #define)
    BSC0_C = START_WRITE;    // Start Write (see #define)

    wait_i2c_done();

    /* Start Read of RTC chip's time */
    BSC0_DLEN = 7;
    BSC0_S = CLEAR_STATUS; // Reset status bits (see #define)
    BSC0_C = START_READ;    // Start Read after clearing FIFO (see #define)

    wait_i2c_done();

    // Store the values read in the tm structure, after masking unimplemented bits.
    t.tm_sec = bcdtod(BSC0_FIFO & 0x7f);
    t.tm_min = bcdtod(BSC0_FIFO & 0x7f);
    t.tm_hour = bcdtod(BSC0_FIFO & 0x3f);
    t.tm_mday = bcdtod(BSC0_FIFO & 0x3f);
    t.tm_wday = bcdtod(BSC0_FIFO & 0x07);
    t.tm_mon = bcdtod(BSC0_FIFO & 0x1f) - 1; // 1-12 --> 0-11
    t.tm_year = bcdtod(BSC0_FIFO) + 100;

    printf("%02d:%02d:%02d %02d/%02d/%02d (UTC on PCF8563)\n",
        t.tm_hour,t.tm_min,t.tm_sec,
        t.tm_mday,t.tm_mon + 1,t.tm_year - 100);

    if(hctosys) {
        printf("Setting system clock from RTC\n");
        now = timegm(&t);
        stime(&now);
    }

    unmap_peripheral(&gpio);
    unmap_peripheral(&bsc0);

    // Done!
}

