/*
 * dht22.h
 *
 *  Created on: Nov 2, 2019
 *      Author: Christian Santiago
 */

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Capture.h>
#include <stdint.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#ifndef DHT22_H_
#define DHT22_H_

#define READ_SUCCESS        "DHT22 reading successful\n"
#define READ_ERROR          "DHT22 reading failed\n"
#define READ_START          "DHT22 starting read\n"
#define READ_END            "DHT22 read ended\n"
#define BAD_CHECKSUM        "Checksum doesn't match\n"
#define GOOD_CHECKSUM       "Checksum matches\n"

#define LOW (0)
#define HIGH (1)

#define INPUT_PU            (GPIO_CFG_INPUT | GPIO_CFG_IN_PU)
#define OUTPUT              (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW)


/* Converts the temperature from Celsius to Fahrenheit*/
double CtoF(double t);

/*Capture Compare ISR subroutine
 * Captures and stores the times between edges, high and lows*/
void capture(Capture_Handle handle, uint32_t interval);

/* Initializes a Capture Instance*/
void initCapture(Capture_Handle handle);

/* Restarts the Capture Instance, preparation for next reading*/
void restartCapture(void);

/*MCU sends the wakeup signal to DHT22, and DHT22 responds with the data*/
void DHT_read(double *readings);

/* Translates the 40-bit signal into the separates parts, Relative Humidity, Temperature and Checksum*/
void DHT_translateData(double *readings);

/*Place inside a loop to continuously get DHT22 readings*/
void DHT_main();

void DHT_begin(uint_least8_t index);

#endif /* DHT22_H_ */
