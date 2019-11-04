/*
 * dht22.c
 *
 * *  Created on: Oct 14, 2019
 *      Author: Christian Santiago Berio
 *
 *  About:
 *      Library developed for the implementation of the
 *      DHT22 Temperature and Humidity sensor integration with the
 *      P.E.M.S project.
 *
 *          P: Plant
 *          E: Environment
 *          M: Monitoring
 *          S: System
 *
 *       The sensor will read the temperature and humidity
 *       characteristics of a room. Afterwards it will
 *       display them via UART(testing purposes)
 *
 *
 *
 * DHT22 sensor starts in low-power mode, in order for the sensor to start transmitting data
 * the MCU must send a wake up signal. This signal must be composed of a low-signal,MCU_LOW, lasting,
 * at least, 10 milliseconds and a high-signal, MCU_HIGH, lasting, at least, 70 microseconds
 *
 * Once the DHT22 has woken up, the DHT22 will send a response signal, consisting of a low-signal, DHT_WAKE_LOW,
 * lasting, at least, 80 microseconds and a high-signal, DHT_WAKE_HIGH, lasting, at least, 80 microseconds
 *
 * Before transmitting the data, the DHT22 will send a signal lasting, at least, 50 microseconds.
 *
 * If the signal transmitted stays high for 30 microseconds(DATA_BIT_0) or less, the bit is a 0.
 * If the signal transmitted stays high for 70 microseconds(DATA_BIT_1), the bit is a 1
 *
 *
 *
 */


/* C includes */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* DHT22 Library */
#include "dht22.h"

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Capture.h>

/* Driver configuration */
#include "ti_drivers_config.h"

uint_least8_t TEMP;

Capture_Handle captHandle;
Capture_Params captParams;

int32_t data[81];
int16_t DHT_Data[5];

int countData = 0;
int allDataCaptured = 0;

double CtoF(double t){
    return 1.8 * t + 32;
}

void capture(Capture_Handle handle, uint32_t interval){

    if(countData == 81){
        countData = 0;
        allDataCaptured = 1;
    }

    if(interval < 100){
        data[countData] = interval;
        countData++;
    }
}

void initCapture(Capture_Handle handle)
{
    captHandle = handle;

    Capture_Params_init(&captParams);
    captParams.mode = Capture_ANY_EDGE;
    captParams.periodUnit = Capture_PERIOD_US;
    captParams.callbackFxn = capture;

    captHandle = Capture_open(TEMP_CAPTURE, &captParams);

    if(captHandle == NULL)
        while(1);
}

void restartCapture(void){
    Capture_stop(captHandle);
    Capture_close(captHandle);
    initCapture(captHandle);
}

void DHT_read(double *readings)
{
    int status;
    int low = 4000;
    int high = 4000;


    //Starts data line as Pull Up
    GPIO_setConfig(TEMP, INPUT_PU);
    usleep(10000);

    GPIO_setConfig(TEMP, INPUT_PU);
    usleep(1000); // Sleep/Pause program for at least 1 millisecond = 1000 microseconds

    GPIO_setConfig(TEMP, OUTPUT);
    usleep(40); // Sleep/Pause program for at least 20 millisecond = 20000 microseconds

    GPIO_setConfig(TEMP, INPUT_PU);

    /*This will wait for the DHT22 to pull down the data line as a response*/
    do{
        if(low == 0){
            return;
        }
        low --;
    }while(GPIO_read(TEMP) != 0);

    /*This will wait for the DHT22 to pull up the data line as a response*/
    do{
        if(high == 0){
            return;
        }
        high --;
    }while(GPIO_read(TEMP) != 1);

    /* Si se añaden interrupts aqui se deben disable
     * ya que la lectura de la temp/hum es time-critical
     */

    //Start capturing data
    status = Capture_start(captHandle);
    if(status == Capture_STATUS_ERROR){
        while(1);
    }

    sleep(2);

    DHT_translateData(readings);
}

void DHT_translateData(double *readings){
//    int checksum; // Size of bits for checksum
    int i, j;
    int32_t rawData[40] = {0}; // Array to hold our usable bits
//    int8_t dataStr[38];

    for(i=1, j=0;i<40, j<40;i+=2, j++){
        if(data[i] < 28){
            rawData[j] = 0;
        }else if(data[i] > 70){
            rawData[j] = 1;
        }
    }

    //Reset Data holding variable
    DHT_Data[0] = DHT_Data[1] = DHT_Data[2] = DHT_Data[3] = DHT_Data[4] = 0;

    for(i=0; i<8; i++){
        DHT_Data[0] |= (rawData[i]      << (7-i));
        DHT_Data[1] |= (rawData[i+8]    << (7-i));
        DHT_Data[2] |= (rawData[i+16]   << (7-i));
        DHT_Data[3] |= (rawData[i+24]   << (7-i));
        DHT_Data[4] |= (rawData[i+32]   << (7-i));
    }

//    checksum = DHT_Data[0] + DHT_Data[1] + DHT_Data[2] + DHT_Data[3];
//    if(checksum != DHT_Data[4]){
//        prtinf(uartHandle, BAD_CHECKSUM, sizeof(BAD_CHECKSUM));
//    }else{
//        printf(uartHandle, GOOD_CHECKSUM, sizeof(GOOD_CHECKSUM));
//    }
    readings[0] = (DHT_Data[0] << 8) | DHT_Data[1];


    readings[1] = ((DHT_Data[2] & 0x7F) << 8) | DHT_Data[3];

    if(DHT_Data[2] & 0x80){
        readings[1] *= -1;
    }

    readings[0] /= 10.0;
    readings[1] /= 10.0;
    readings[2] = CtoF(readings[1]);

    countData = 0; // Resets data count after read task finishes
}

void DHT_begin(uint_least8_t index){
    TEMP = index;

    GPIO_setConfig(TEMP, INPUT_PU);
    usleep(10000);
}

//void DHT_main(void){
//    double finalData[3]; // Variable that holds the final data, after translation
//    DHT_read(); // Wakes up and reads the data sent from the DHT22
//    DHT_translateData(finalData); // Translates data from microseconds to actual floating numbers
//    restartCapture(); // Restarts the Capture Instance
//}



/********************  Main method for DHT22
void *tempThread(void *arg0){

    //    const char formatData[50] = "Temp"


    GPIO_init();
    UART_init();
    Timer_init();
    Capture_init();

    //    GPIO_setConfig(TEMP, INPUT_PU);

    initUART(); // Initializes and Opens an UART Connection to print, since printf only works in Debug Mode
    initCapture(); // Initializes and Opens a Capture instance for later use

    while(1){
        DHT_main();
    }
}

 ********************/

