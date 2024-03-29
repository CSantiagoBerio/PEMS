/*
 * temp.c
 *
 *  Created on: Oct 14, 2019
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
 *       characteristics of a room. Afterwards it will d
 *       isplay them via UART(testing purposes)
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

/*
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
 */

#define READ_SUCCESS        "DHT22 reading successful\n"
#define READ_ERROR          "DHT22 reading failed\n"
#define READ_START          "DHT22 starting read\n"
#define READ_END            "DHT22 read ended\n"
#define BAD_CHECKSUM        "Checksum doesn't match\n"
#define GOOD_CHECKSUM       "Checksum matches\n"

#define LOW (0)
#define HIGH (1)

#define INPUT_PU            GPIO_CFG_INPUT | GPIO_CFG_IN_PU
#define OUTPUT              GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW

UART_Handle uartHandle;
UART_Params uartParams;

Capture_Handle captHandle;
Capture_Params captParams;

int32_t data[81] = {0};
int16_t DHT_Data[5];

int countData = 0;
int allDataCaptured = 0;


double CtoF(double temp){
    return (1.8 * temp + 32);
}

void initUART(){

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.baudRate = 115200;

    /* Open UART */
    uartHandle = UART_open(UART_LOCAL, &uartParams);

    if (uartHandle == NULL) {
        /* UART_open() failed */
        while (1);
    }
    else{
        const char mess[] = "UART connection established\n";
        UART_write(uartHandle, &mess, sizeof(mess));
    }
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

void initCapture()
{
    Capture_Params_init(&captParams);
    captParams.mode = Capture_ANY_EDGE;
    captParams.periodUnit = Capture_PERIOD_US;
    captParams.callbackFxn = capture;

    captHandle = Capture_open(TEMP_CAPTURE, &captParams);

    if(captHandle == NULL){
        const char mess[32] = "Capture Init Unsuccessful\n";
        UART_write(uartHandle, &mess, sizeof(mess));
    }else{
        const char mess[32] = "Capture Init Successful\n";
        UART_write(uartHandle, &mess, sizeof(mess));
    }

    //    status = Capture_start(captHandle);
    //    if(status == Capture_STATUS_ERROR){
    //        while(1);
    //    }
}

void restartCapture(void){
    Capture_stop(captHandle);
    Capture_close(captHandle);
    initCapture();
}

void DHT_read(void)
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

    /* Si se a�aden interrupts aqui se deben disable
     * ya que la lectura de la temp/hum es time-critical
     */

    //Start capturing data
    status = Capture_start(captHandle);
    if(status == Capture_STATUS_ERROR){
        while(1);
    }

    sleep(2);
}

void DHT_translateData(double *readings){
    int checksum; // Size of bits for checksum
    int i, j;
    int32_t rawData[40] = {0}; // Array to hold our usable bits
    int8_t dataStr[38];

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

    checksum = DHT_Data[0] + DHT_Data[1] + DHT_Data[2] + DHT_Data[3];
    if(checksum != DHT_Data[4]){
        UART_write(uartHandle, BAD_CHECKSUM, sizeof(BAD_CHECKSUM));
    }else{
        UART_write(uartHandle, GOOD_CHECKSUM, sizeof(GOOD_CHECKSUM));
    }
    readings[0] = (DHT_Data[0] << 8) | DHT_Data[1];


    readings[1] = ((DHT_Data[2] & 0x7F) << 8) | DHT_Data[3];

    if(DHT_Data[2] & 0x80){
        readings[1] *= -1;
    }

    readings[0] /= 10.0;
    readings[1] /= 10.0;
    readings[2] = CtoF(readings[1]);

    sprintf(dataStr,"Humidity: %.1f %, Temp: %.1f C, %.1f F\n", readings[0], readings[1], readings[2]);

    UART_write(uartHandle, &dataStr, sizeof(dataStr));
}

void DHT_main(void){
    double finalData[3]; // Variable that holds the final data, after translation
    const char dataStr[38]; // Variable that holds the formatted string, for debug purposes
    DHT_read(); // Wakes up and reads the data sent from the DHT22
    countData = 0; // Resets data count after read task finishes
    DHT_translateData(finalData); // Translates data from microseconds to actual floating numbers
    restartCapture(); // Restarts the Capture Instance
}


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
