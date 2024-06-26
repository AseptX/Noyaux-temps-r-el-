/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "adxl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TASK_SHELL_STACK_DEPTH 512
#define TASK_SHELL_PRIORITY 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
TaskHandle_t h_task_shell = NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

SemaphoreHandle_t sem1;
TaskHandle_t handle_blink_led, handle_spam, tache1, tache2, tache3 , tache4, tache5, handle_stats;
int delay_led, delay_spam;
char * message;


int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

void ftache1 (void * unused)
{
	for(;;)
	{
		//		printf("Tache 1 \r\n");
		vTaskDelay(1);
	}
}

void task_spam (void * unused)
{
	vTaskSuspend(0); //Suspend la tache en cours (0 veux dire ca)

	for(;;)
	{
		printf("%s\r\n", message);
		vTaskDelay(delay_spam);
	}
}

void task_blink_led (void * unused)
{
	vTaskSuspend(0); //Suspend la tache en cours (0 veux dire ca)

	for(;;)
	{
		//		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		//		printf("Je clignote\r\n");
		vTaskDelay(delay_led);
	}
}

void task_stats (void * unused)
{
	vTaskSuspend(0); //Suspend la tache en cours (0 veux dire ca)

	for(;;)
	{
		//		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		//		printf("Je clignote\r\n");
		vTaskDelay(1);
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		shell_uart_receive_irq_cb();	// C'est la fonction qui donne le sémaphore!
	}
}

int fonction(int argc, char ** argv)
{
	printf("Je suis une fonction bidon\r\n");

	printf("argc = %d\r\n", argc);

	for (int i = 0 ; i < argc ; i++)
	{
		printf("arg %d = %s\r\n", i, argv[i]);
	}

	return 0;
}

int addition(int argc, char ** argv)
{
	if (argc == 3)
	{
		int a, b;
		a = atoi(argv[1]);
		b = atoi(argv[2]);
		printf("%d + %d = %d\r\n", a, b, a+b);

		return 0;
	}
	else
	{
		printf("Erreur, pas le bon nombre d'arguments\r\n");
		return -1;
	}
}

int led(int argc, char ** argv)
{
	int delay = atoi(argv[1]); // Conversion chaine de caractère (ASCII) vers Integer
	if (delay == 0)
	{
		//		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin, 0);
		printf("Je m'éteins\r\n");
		vTaskSuspend(handle_blink_led); //Suspend la tache
	}
	else
	{
		delay_led = delay;
		vTaskResume(handle_blink_led); //Reprend la tache la ou elle en etait

	}
}

int spam(int argc, char ** argv){

	message = argv[1];

	if (strcmp(message, "stop")==0)
	{
		vTaskSuspend(handle_spam);
	}
	else
	{
		if (argc < 3)
		{
			printf("error args\r\n");
			return -1;
		}
		delay_spam = atoi(argv[2]);
		vTaskResume(handle_spam);
	}

	return 0;
}

//Création d'un buffer de 400 octets pour écrire dedans
char text_buffer[400];

int stats(int argc, char ** argv){
	//écrit les données dans le buffer
	vTaskGetRunTimeStats(text_buffer);
	printf("%s\r\n", text_buffer);

	vTaskList(text_buffer);
	printf("%s\r\n", text_buffer);

	return 0;
}

HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// TODO
}

int SPI(int argc, char ** argv){
	uint8_t address_DEVID = 0x00, address_INT_enable = 0x2E, address_PWR_CTL = 0x2D;
	uint8_t data = 0, data_INT_enable = 0, data_PWR_CTL = 0;
	uint16_t size = 1;

	uint8_t data6[6];

	receive(address_DEVID, &data, size);

	printf("DEVID = 0x%X\r\n", data);

	uint8_t test = 0;

	transmit (address_INT_enable, &data_INT_enable, size);
	transmit (address_PWR_CTL, &data_PWR_CTL, size);

	receive(address_INT_enable, &test, 1);
	printf("INT_Enable = %x\r\n", test);
	receive(address_PWR_CTL, &test, 1);
	printf("PWR_CTL = %x\r\n", test);

	receive(address_INT_enable, &data_INT_enable, size);
	data_INT_enable = data_INT_enable | 0x80;

	receive(address_PWR_CTL, &data_PWR_CTL, size);
	data_PWR_CTL = data_PWR_CTL | 0x08;

	transmit (address_INT_enable, &data_INT_enable, size);
	transmit (address_PWR_CTL, &data_PWR_CTL, size);

	receive(address_INT_enable, &test, 1);
	printf("INT_Enable = %x\r\n", test);
	receive(address_PWR_CTL, &test, 1);
	printf("PWR_CTL = %x\r\n", test);

	//	for (int i = 0; i < 4; i++){

	while (HAL_GPIO_ReadPin(INT_GPIO_Port, INT_Pin) == 0);	// polling sur INT1

	printf("INT1 detected\r\n");



	int16_t acc_x = (int16_t)(data6[0]) | (int16_t)(data6[1])<<8;
	int16_t acc_y = (int16_t)(data6[2]) | (int16_t)(data6[3])<<8;
	int16_t acc_z = (int16_t)(data6[4]) | (int16_t)(data6[5])<<8;

	printf("Transmission de data : %d %d %d\r\n", acc_x, acc_y, acc_z);



	//	}

	return 0;
}


void task_shell(void * unused)
{
	shell_init();
	shell_add('f', fonction, "Une fonction inutile");
	shell_add('a', addition, "Effectue une somme");
	shell_add('l', led, "Faire clignoter une LED");
	shell_add('s', spam, "Envoyer du text dans la liaison serie");
	shell_add('t', stats, "Temps passé dans chaque tache");
	shell_add('d', SPI, "test la liaison SPI");
	shell_run();	// boucle infinie
}

//Tache appelé automatiquement si le systeme detecte un overflow
void vApplicationStackOverflowHook( TaskHandle_t xTask,
		signed char *pcTaskName )
{
	// allumer led
	//	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin, 1);
	Error_Handler();
}

//Active le timer 2
void configureTimerForRunTimeStats(void)
{
	HAL_TIM_Base_Start(&htim2);
}

//Renvois la valeur du timer
unsigned long getRunTimeCounterValue(void)
{
	return __HAL_TIM_GET_COUNTER(&htim2);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_TIM2_Init();
	MX_SPI2_Init();
	/* USER CODE BEGIN 2 */

	sem1 = xSemaphoreCreateBinary();

	//Creation de tache pour depassement du tas
	/*while(1)
	{
	 int i;
	if( xTaskCreate(ftache1,	"BLINK_LED", 256, NULL,	3, &tache1) != pdPASS)
		{
			printf("Error creating task %d \r\n", i);
			Error_Handler();
		}
	i++;
	}*/

	//Creation de tache pour depassement de la pile
	xTaskCreate(ftache1,
			"tache1",
			50, //Taille de la pile
			NULL, //parameter
			0, //Priority
			&tache1 //Allocation de la mémoire pour créer l'objet
	);

	xTaskCreate(task_stats,
			"stats",
			50, //Taille de la pile
			NULL, //parameter
			0, //Priority
			&tache1 //Allocation de la mémoire pour créer l'objet
	);

	/*xTaskCreate(task_blink_led,
				"BLINK_LED",
				256, //Taille de la pile
				NULL, //parameter
				3, //Priority
				&handle_blink_led //Allocation de la mémoire pour créer l'objet
		);*/

	xTaskCreate(task_spam,
			"SPAM",
			256, //Taille de la pile
			NULL, //parameter
			2, //Priority
			&handle_spam //Allocation de la mémoire pour créer l'objet
	);

	if (xTaskCreate(task_shell, "Shell", TASK_SHELL_STACK_DEPTH, NULL, TASK_SHELL_PRIORITY, &h_task_shell) != pdPASS)
	{
		printf("Error creating task shell\r\n");
		Error_Handler();
	}

	vTaskStartScheduler();
	/* USER CODE END 2 */

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
