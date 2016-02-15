/**
  ******************************************************************************
  * File Name          : stm32f0xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  __SYSCFG_CLK_ENABLE();

  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  __HAL_REMAP_PIN_ENABLE(HAL_REMAP_PA11_PA12);

}



void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==TIM3)
  {
    /* Peripheral clock enable */
    __TIM3_CLK_ENABLE();
  
    /**TIM3 GPIO Configuration    
    PA6     ------> TIM3_CH1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  else if(htim_base->Instance==TIM14)
  {
     /* Peripheral clock enable */
    __TIM14_CLK_ENABLE();
  /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(TIM14_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM14_IRQn);
 }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM3)
  {
    /* Peripheral clock disable */
    __TIM3_CLK_DISABLE();
  
    /**TIM3 GPIO Configuration    
    PA6     ------> TIM3_CH1 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

  }
}



