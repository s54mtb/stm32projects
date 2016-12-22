/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "command.h"

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_FS;
extern TIM_HandleTypeDef htim14;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{

  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USB global Interrupt / USB wake-up interrupt through EXTI line 18.
*/
void USB_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

static GPIO_PinState testpin = GPIO_PIN_RESET;

/**
* @brief This function handles TIM14 global interrupt.
*/
void TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM14_IRQn 0 */

	if (GetFtestState() == 1)
	{
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, testpin);
	  if (testpin == GPIO_PIN_RESET) testpin = GPIO_PIN_SET; else testpin = GPIO_PIN_RESET;
	}
  /* USER CODE END TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM14_IRQn 1 */

  /* USER CODE END TIM14_IRQn 1 */
}

