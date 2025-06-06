//
// Created by Thijs Hulshof
//

#ifndef CONFIG_H
#define CONFIG_H

#define DISPLAY_UPSIDEDOWN // Uncomment to reverse display screen

#define DEBUGMODE   //Print info to UART0

//#define SIMPIN  "AT+CPIN=00000"

//Moving average sample moments, for smoothing out the power consumption value
#define samples 3

//Re-init the display every:
#define WAIT_INTERVAL_US (10LL * 60LL * 1000000LL)  // 10 minutes

#endif //CONFIG_H
