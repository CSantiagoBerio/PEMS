/*
 * CO2.c
 *
 *  Created on: Oct 18, 2019
 *      Author: Christian Santiago Berio
 */

/* C includes */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Capture.h>
#include <ti/devices/cc32xx/driverlib/utils.h>
#include <ti/posix/ccs/time.h>


/* Driver configuration */
#include "ti_drivers_config.h"

UART_Handle uartCO2;
UART_Params paramsCO2;

UART_Handle uartLocal;
UART_Params paramsLocal;

struct timespec ts = {0};
uint8_t cal[9] = {0xff, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // Calibration Command
uint8_t readCO2[9] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // Read CO2 Command
uint8_t disableAutoCal[9] = {};
char packet[9]; // Resulting packet from reading UART


void UART_CO2_init(){
    /* Create a UART for CO2 use */
    UART_Params_init(&paramsCO2);

    /* Reading Specs */
    paramsCO2.readMode = UART_MODE_BLOCKING;
    paramsCO2.readDataMode = UART_DATA_BINARY;
    paramsCO2.readReturnMode = UART_RETURN_FULL;
    paramsCO2.readTimeout = UART_WAIT_FOREVER;

    /* Writing Specs */
    paramsCO2.writeMode = UART_MODE_BLOCKING;
    paramsCO2.writeDataMode = UART_DATA_BINARY;

    /* Data and Other Specs */
    paramsCO2.baudRate = 9600;
    paramsCO2.dataLength = UART_LEN_8;
    paramsCO2.stopBits = UART_STOP_ONE;



    /* Open UART */
    uartCO2 = UART_open(UART_CO2, &paramsCO2);

    if (uartCO2 == NULL) {
        /* UART_open() failed */
        while (1);
    }
    else{
        const char mess[] = "UART_CO2 connection established\n";
        UART_write(uartLocal, &mess, sizeof(mess));
    }
}

void UART_LOCAL_init(){
    /* Create a UART for Local use */
    UART_Params_init(&paramsLocal);
    paramsLocal.writeDataMode = UART_DATA_TEXT;
    paramsLocal.baudRate = 115200;

    /* Open UART */
    uartLocal = UART_open(UART_LOCAL, &paramsLocal);

    if (uartLocal == NULL) {
        /* UART_open() failed */
        while (1);
    }
    else{
        const char mess[] = "UART_LOCAL connection established\n";
        UART_write(uartLocal, &mess, sizeof(mess));
    }
}
/*
 * Calculates the checksum of the packet sent by the Sensor
 *
 * Sum all 9 bytes of the packet,
 * then invert by subtracting 0xFF from the result of the sum,
 * then add one to the result of the inversion.
 *
 * @param: packet
 *
 * @ret:   checksum value
 *
 *
 */
uint8_t getChecksum (char packet[]){

    uint8_t i, checksum;
    for(i = 1; i<8; i++){
        checksum += packet[i];
    }

    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
}

int getPPM(uint8_t packet[]){

    return ((int)packet[2]*256 + (int)packet[3]);
}

void calibrateMHZ16(){

    UART_write(uartCO2, &cal, sizeof(cal));
    usleep(10);
}


void *mainThread(void *arg0){

    clock_settime(CLOCK_REALTIME, &ts);

    GPIO_init();
    UART_init();

    UART_LOCAL_init();
    UART_CO2_init();

    size_t buffSize = 9;
    uint8_t checksum;
    int i;
    bool *isAvailable;

    // Calibrate Sensor
    calibrateMHZ16();
    // Read Co2 Concentration
    UART_write(uartCO2, &readCO2, buffSize);
    usleep(10);

    i=0;
    // Make sure there is data in the buffer to read
    UART_control(uartCO2, UART_CMD_ISAVAILABLE, &isAvailable);
    if(isAvailable){
        UART_read(uartCO2, &packet, 9);

    }
    else {
        UART_write(uartLocal, "No bytes available...yet", sizeof("No bytes available...yet"));
    }

    checksum = getChecksum(&packet);
    printf("Checksum: %d\n", checksum);

    int ppm = getPPM(&packet);
    printf("CO2 concentration: %d PPM\n", ppm);




    return (NULL);}

