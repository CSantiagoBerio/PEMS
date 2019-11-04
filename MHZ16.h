/*
 * MHZ16.h
 *
 *  Created on: Nov 3, 2019
 *      Author: csant
 */

/* Driver configuration */
#include "ti_drivers_config.h"

#ifndef MHZ16_H_
#define MHZ16_H_

#define CHECKSUM_FAILED "Checksum doesn't match\n"

void UART_CO2_init();

uint8_t getChecksum(char packet[9]);

int getPPM(uint8_t packet[]);

void calibrateMHZ16();

int readMHZ16();


#endif /* MHZ16_H_ */
