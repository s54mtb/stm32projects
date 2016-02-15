/**
  ******************************************************************************
  * File Name          : audio.c
  * Description        : Audio functions
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "settings.h"
#include "audio.h"

extern settings_t settings;
extern TIM_HandleTypeDef htim3;

/* TIM3 setup */
void Audio_Init(settings_t settings)
{
	uint8_t run = settings.audio.run;
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;


	//HAL_TIM_Base_Stop(&htim3); 
	Audio_Run(0);
	
	if ((settings.audio.frequency < 100) | (settings.audio.frequency > 10000)) settings.audio.frequency = 1000;
	if ((settings.audio.volume < 1) | (settings.audio.volume > 100)) settings.audio.volume = 50;
	
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 48;                            // 1MHz timer clock3
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	if (settings.audio.frequency>0) 
    htim3.Init.Period = (uint16_t)(1000000U/settings.audio.frequency);         // Audio signal frequency is in Hz
	else
		htim3.Init.Period = 1000; 	// 1kHz
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim3);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
	if (settings.audio.volume > 0)
    sConfigOC.Pulse = htim3.Init.Period * settings.audio.volume / 200;   /// 0 to 50% duty cycle
	else
		sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
	
	//HAL_TIM_Base_Start(&htim3); 
	Audio_Run(run);


}


void Audio_Run(uint8_t on)	
{
	if (on>0) 
	{
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		settings.audio.run = 1;
	}
	else
	{
		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
		settings.audio.run = 0;
	}
}

