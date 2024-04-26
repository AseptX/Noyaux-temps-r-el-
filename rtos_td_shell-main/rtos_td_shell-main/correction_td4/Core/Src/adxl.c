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
	address |= 0x80; // mise à 1 du bit read pour autoriser la lecture
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

void receive_multiple_data(uint8_t address, uint8_t *data, uint16_t size)
{
	address = address | 0xC0; //Mise à 1 du bit MB => permet de transmettre les data en continue tant qu'on arrete par l'horloge
	// le nombre de coup d'horloge est defini par le size du receive
	// Mise à 1 du bit read pour autoriser la lecture
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, &address, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2, data, size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);
}
