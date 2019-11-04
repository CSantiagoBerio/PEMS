/*
 * pems.c
 *
 *  Created on: Nov 3, 2019
 *      Author: csant
 */

/* C includes */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Capture.h>
#include <ti/devices/cc32xx/driverlib/utils.h>
#include <ti/posix/ccs/time.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "MHZ16.h"
#include "DHT22.h"

UART_Handle uartCO2;
Capture_Handle captHandle;

void *mainThread(void *arg0){

    uint8_t dataStr[40];
    uint8_t str[29];
    double finalData[3]; // Variable that holds the final data, after translation
    int data = 0;

    GPIO_init();
    Capture_init();
    UART_init();

    DHT_begin(temp);

    UART_CO2_init(uartCO2);
    initCapture(captHandle);

    while(1){

//        data = readMHZ16();
//        printf("CO2 Concentration: %04d PPM\n", data);

        DHT_read(finalData); // Wakes up and reads the data sent from the DHT22
//        DHT_translateData(finalData); // Translates data from microseconds to actual floating numbers
        restartCapture(); // Restarts the Capture Instance

        printf("Humidity: %.1f %, Temp: %.1f C, %.1f F\n", finalData[0], finalData[1], finalData[2]);

        sleep(2);

    }
}
