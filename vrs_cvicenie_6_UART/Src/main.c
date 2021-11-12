#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"

#define BUFF_LEN 20

char buffer[BUFF_LEN];
char led_state='0';

void SystemClock_Config(void);

void process_serial_data(uint8_t ch);

int main(void) {
	memset(buffer, '\0', BUFF_LEN);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	SystemClock_Config();

	MX_GPIO_Init();
	MX_USART2_UART_Init();

	USART2_RegisterCallback(process_serial_data);


	while (1) {
		LL_USART_TransmitData8(USART2, led_state);
		LL_USART_TransmitData8(USART2, '\r');

		LL_mDelay(5000);
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

	if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0) {
		Error_Handler();
	}
	LL_RCC_HSI_Enable();

	/* Wait till HSI is ready */
	while (LL_RCC_HSI_IsReady() != 1) {

	}
	LL_RCC_HSI_SetCalibTrimming(16);
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {

	}
	LL_Init1msTick(8000000);
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	LL_SetSystemCoreClock(8000000);
}

void process_serial_data(uint8_t ch) {

	if (ch == 0x0D) {
		return;
	}

	//robime shift
	if (strlen(buffer) <= BUFF_LEN) {
		for (uint8_t i = 1; i < BUFF_LEN; i++) {
			buffer[i - 1] = buffer[i];
		}
		buffer[BUFF_LEN - 1] = '\0';
	}

	//concatenate (pripoj znak)
	char c = ch;
	strncat(buffer, &c, 1);

	if (strstr(buffer, "ledon") != NULL)
	{
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
		led_state = '1';
		memset(buffer, '\0', BUFF_LEN);
	}

	else if (strstr(buffer, "ledoff") != NULL)
	{
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
		led_state = '0';
		memset(buffer, '\0', BUFF_LEN);
	}

	return;
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
