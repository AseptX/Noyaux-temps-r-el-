/*
 * adxl.h
 *
 *  Created on: Apr 23, 2024
 *      Author: Antoine
 */

#ifndef INC_ADXL_H_
#define INC_ADXL_H_

#include "main.h"

void receive(uint8_t address, uint8_t *data, uint16_t size);
void transmit (uint8_t address, uint8_t *data, uint16_t size);
void receive_multiple_data(uint8_t address, uint8_t *data, uint16_t size);

#endif /* INC_ADXL_H_ */
