/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

TaskHandle_t handle_blink_led; //Reference de la tache pour pouvoir interragir avec
TaskHandle_t handle_give, handle_take;
SemaphoreHandle_t sem1;

QueueHandle_t queue1;

int i=1;
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

void task_blink_led (void * unused)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		printf("Je clignote\r\n");
		vTaskDelay(100);
	}
}

//Exemple : On a une tache qui attend une donnée, la donnée arrive dans une interruption (de timer par exemple)
// Dans cette interruption, on envoit le semaphore à la tache qui attend pour la reveiller et qu'elle s'execute
void task_Give (void * unused)
{

	for(;;){
		printf("Je rentre en interruption\r\n");
		//xSemaphoreGive(sem1);
		//xTaskNotifyGive(handle_take);

		xQueueSend(queue1, &i, portMAX_DELAY);  // bloque si pleine

		printf("J'ai envoyé le message\r\n");
		//vTaskDelay(100);

		vTaskDelay(100+i);
		i=i+100;
	}
}

//Reveiller par le semaphore envoyé par la tache give (qui simule une interruption)
//Pas besoin de delai, elle se reveille quand give est executé et il y a deja un delai dedans
//La tache execute tout ce qu'il y a avant le semaphore take, et elle se bloque à cette ligne jusqu'a qu'il arrive
void task_Take (void * unused)
{
	int data;

	for(;;){
		printf("Je suis arrivé au semaphore, je me bloque\r\n");
		if(xQueueReceive(queue1, &data, 1000) == pdTRUE)
				{
					printf("J'ai recu le message : %d\r\n", data);
				}
				else
				{
					printf("Erreur de reception du message\r\n");
					Error_Handler();
					//NVIC_SystemReset(); reset du systeme
				}



		/*if(ulTaskNotifyTake(pdTRUE, 1000) == pdTRUE)
		{
			printf("J'ai recu le semaphore\r\n");
		}
		else
		{
			printf("Erreur de reception du semaphore\r\n");
			Error_Handler();
			//NVIC_SystemReset(); reset du systeme
		}
		/*if(xSemaphoreTake(sem1, 1000) == pdTRUE)
		{
			printf("J'ai recu le semaphore\r\n");
		}
		else
		{
			printf("Erreur de reception du semaphore\r\n");
			Error_Handler();
			//NVIC_SystemReset(); reset du systeme
		}*/
	}
}



// ET ICI
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
	/* USER CODE BEGIN 2 */
	printf("\r\n======================================================================\r\n");

	sem1 = xSemaphoreCreateBinary(); //Allocation de la memoire pour créer le semaphore (Obligatoire dés qu'on a un handle)


	queue1 = xQueueCreate (3, sizeof(int));

	BaseType_t ret;

	xTaskCreate(task_blink_led,
			"BLINK_LED",
			256, //Taille de la pile
			NULL, //parameter
			3, //Priority
			&handle_blink_led //Allocation de la mémoire pour créer l'objet
	);

	xTaskCreate(task_Give,
			"GIVE",
			256,
			NULL,
			2,
			&handle_give
	);

	ret = xTaskCreate(task_Take,
			"Take",
			256,
			NULL,
			1,
			&handle_take
	);

	if (ret != pdPASS)
	{
		printf("Erreur dans l'acquisition du semaphore");
		Error_Handler();
	}


	vTaskStartScheduler();		// Ne rien coder apres cette ligne!
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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 216;
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
