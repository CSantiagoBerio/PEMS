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

#define MCU_LOW (10e-3)
#define MCU_HIGH (40e-6)
#define DHT_WAKE_LOW (80e-6)
#define DHT_WAKE_HIGH (80e-6)
#define DHT_START_TRANSMIT_DATA (50e-6)
#define DATA_BIT_0 (30e-6)
#define DATA_BIT_1 (70e-6)

#define LOW (0)
#define HIGH (1)


UART_Handle uartHandle;
UART_Params uartParams;

Timer_Handle timerHandle;
Timer_Params timerParams;

Capture_Handle captureHandle;
Capture_Params captureParams;

uint_least16_t seconds = 0;
uint_least16_t minutes = 0;

uint32_t reading = 0;
uint32_t oldtime = 0;

uint32_t convert[32];
uint32_t data[41];
int countData = 0;

void print(UART_Handle handle, const char str[]){
    UART_write(handle, &str, sizeof(str));
}

double CtoF(double temp){
    return 1.8 * temp + 32;
}

void wakeUp(){
    /* Configure TEMP (Pin 59) to output mode, output high */
    GPIO_setConfig(TEMP, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH);

    GPIO_write(TEMP, LOW);

    sleep(MCU_LOW);

    GPIO_write(TEMP, HIGH);

    sleep(MCU_HIGH);


}

void waitForDHT(){

    GPIO_setConfig(TEMP, GPIO_CFG_INPUT | GPIO_CFG_IN_PD);
}

void readDHT(){

    printf("Starting data transmission...\n");

    sleep(DHT_START_TRANSMIT_DATA);

    printf("Reading...\n");
}

void sensorScan(Timer_Handle timer){
}

void capture(Capture_Handle handle, uint32_t interval){
    if(countData <=40){
        data[countData] = interval;
        countData++;
    }
}


void *mainThread(void *arg0){

    struct timespec ts = {0};
    struct timespec ts1;

    uint32_t data[41];

    clock_settime(CLOCK_REALTIME, &ts);

    const char str[] = "Progress Report#4: Temperature and Humidity Sensor Module\n";

    GPIO_init();
    UART_init();
    Timer_init();
    Capture_init();

    GPIO_setConfig(TEMP, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL | GPIO_CFG_IN_INT_BOTH_EDGES);

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
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

    Capture_Params_init(&captureParams);
    captureParams.periodUnit = Capture_PERIOD_US;
    captureParams.mode = Capture_ANY_EDGE;
    captureParams.callbackFxn = capture;

    captureHandle = Capture_open(TEMP_CAPTURE, &captureParams);
    if(captureHandle ==  NULL){
        const char mess[] = "Capture not opened";
        UART_write(uartHandle, &mess, sizeof(mess));
    }


    UART_write(uartHandle, &str, sizeof(str));


    GPIO_setConfig(TEMP, GPIO_CFG_INPUT | GPIO_CFG_IN_PD);
    usleep(1000);

    GPIO_setConfig(TEMP, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_write(TEMP, LOW);
    usleep(20000);

    GPIO_setConfig(TEMP, GPIO_CFG_INPUT | GPIO_CFG_IN_PU);
    Capture_start(captureHandle);
    usleep(50);

    while(1){
        if(GPIO_read(TEMP == 1)){
            uint32_t start = clock_settime(CLOCK_REALTIME, 0);
            if(GPIO_read(TEMP) == 0){
                uint32_t stop = clock_gettime(CLOCK_REALTIME, &ts1);
                ltoa(&ts1.tv_sec, convert);
               UART_write(uartHandle, &convert, sizeof(convert));

            }

        }

    }
    return (NULL);

}
