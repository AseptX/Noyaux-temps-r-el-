/*
 * adxl.c
 *
 *  Created on: Apr 23, 2024
 *      Author: Antoine
 */

#include "adxl.h"

#include "spi.h"
#include "gpio.h"

void receive(uint8_t address, uint8_t *data, uint16_t size)
{
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET); //PB4 à 0 (PB4 = GPIO output)
	HAL_SPI_Transmit(&hspi2, &address, 1, HAL_MAX_DELAY);
	//C'est à nous de definir le size du message du retour
	HAL_SPI_Receive(&hspi2, data, size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);
}

void transmit (uint8_t address, uint8_t *data, uint16_t size) {
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET); //PB4 à 0 (PB4 = GPIO output)
	HAL_SPI_Transmit(&hspi2, &address, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);
}
