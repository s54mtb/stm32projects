/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc.h"
#include "CDC_receiver.h"
#include <string.h>
#include "command.h"
#include "settings.h"
#include "morse.h"
#include "ADF4351.h"

settings_t settings;

/* Private variables ---------------------------------------------------------*/
uint8_t    cdc_activity, refresh_lcd =0 ;
extern USBD_HandleTypeDef hUsbDeviceFS;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;
SPI_HandleTypeDef hspi1;
//extern HAL_StatusTypeDef adf4351_write(uint32_t data);

extern char *newline;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM14_Init(void);
static void MX_SPI1_Init(void);

volatile uint8_t cw_autorunning = 0;
volatile int ar_count = 0;

//HAL_StatusTypeDef SPI_SendWord(uint32_t data, SPI_HandleTypeDef* hspi)
//{
//	HAL_StatusTypeDef ret;
//	uint8_t buf[4];
//	buf[0] = (data >> 24) & 0xff;
//	buf[1] = (data >> 16) & 0xff;
//	buf[2] = (data >> 8) & 0xff;
//	buf[3] = data & 0xff;

//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
//	ret = HAL_SPI_Transmit(hspi, buf, 4, 10);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
//  
//	return ret;
//}


void abort_autorun(void)
{
	cw_autorunning = 0;
	ar_count = 0;
}

//void PLL_Sync(void)
//{
//		int i;

//	for (i=0; i<6; i++)
//	{
//    adf4351_write(ADF4351_GetRegisterBuf(5-i));	//	Write registers to PLL chip
//	}
//}

///***************************************************************************//**
// * @brief Writes 4 bytes of data to ADF4351.
// * @param data - Data value to write.
// * @retval SPI status
//*******************************************************************************/
//HAL_StatusTypeDef adf4351_write(uint32_t data)
//{
//  return SPI_SendWord(data, &hspi1);
//}


//void prepare_pll(void)
//{
//	
//	ADF4351_Init();
//	ADF4351_Reg1.b.PhaseAdjust = ADF4351_DISABLE;
//	ADF4351_Reg1.b.PhaseVal = 1;
//	ADF4351_Reg1.b.Prescaler = ADF4351_PRESCALER_4_5;
//	
//	ADF4351_Reg2.b.CounterReset = ADF4351_DISABLE;
//	ADF4351_Reg2.b.RDiv2 = ADF4351_REFDIV_2;
//	ADF4351_Reg2.b.RMul2 = ADF4351_REFMUL_1;
//	ADF4351_Reg2.b.CPCurrent = ADF4351_CPCURRENT_1_88;
//	ADF4351_Reg2.b.CPTristate = ADF4351_DISABLE;
//	ADF4351_Reg2.b.DoubleBuffer  = ADF4351_DISABLE;
//	ADF4351_Reg2.b.LockFunction = ADF4351_LDF_FRAC;
//	ADF4351_Reg2.b.LockPrecision = ADF4351_LDP_10NS;
//	ADF4351_Reg2.b.LowNoiseSpur = ADF4351_LOW_NOISE_MODE;
//	ADF4351_Reg2.b.MuxOut = ADF4351_MUX_THREESTATE;
//	ADF4351_Reg2.b.PhasePolarity = ADF4351_POLARITY_POSITIVE;
//	ADF4351_Reg2.b.PowerDown = ADF4351_DISABLE;
//	ADF4351_Reg2.b.RCountVal = 1;
//	
//	ADF4351_Reg3.b.AntibacklashW = ADF4351_ABP_6NS;
//	ADF4351_Reg3.b.BandSelMode = ADF4351_BANDCLOCK_LOW;
//	ADF4351_Reg3.b.ChargeCh = ADF4351_DISABLE;
//	ADF4351_Reg3.b.ClkDivMod = ADF4351_CLKDIVMODE_OFF;
//	ADF4351_Reg3.b.ClkDivVal = 150;
//	ADF4351_Reg3.b.CsrEn = ADF4351_DISABLE;
//	
//	ADF4351_Reg4.b.AuxEnable = ADF4351_ENABLE;
//	ADF4351_Reg4.b.Mtld = ADF4351_DISABLE;
//	ADF4351_Reg4.b.OutEnable = ADF4351_ENABLE;
//	ADF4351_Reg4.b.OutPower = ADF4351_POWER_PLUS5DB;
//	ADF4351_Reg4.b.VcoPowerDown =  ADF4351_DISABLE;
//	ADF4351_Reg4.b.Feedback = ADF4351_FEEDBACK_FUNDAMENTAL;

//	ADF4351_Reg5.b.LdPinMode = ADF4351_LD_PIN_DIGITAL_LOCK;

//}

int main(void)
{
	
	int autorun;
//		double calcfreq;
//	double CurrentFreq = 144e6;

	
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	MX_SPI1_Init();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

  MX_TIM3_Init();
	MX_TIM14_Init();
  MX_USB_DEVICE_Init();
	
	Morse_Init();
	
//	prepare_pll();
//  UpdateFrequencyRegisters(CurrentFreq, 50000000.0, 5000, 1, 1, &calcfreq); 	
//	PLL_Sync();
	
	autorun = cmd_isautorun() ;

  /* Infinite loop */
  while (1)
  {
	  if (is_line_received())
		{
//     Process received line

			cmd_proc(get_line_buffer());
		}
		HAL_Delay(10); ar_count++;
		if ((ar_count > 1000) & (autorun>0))
		{
			// Start Autorun
			cmd_autorun(autorun);
			cw_autorunning = 1;
			while(cw_autorunning) HAL_Delay(10);
		}
  }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
  /* RCC Configuration----------------------------------------------------------*/
	
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLCLK;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}



/* TIM3 init function */
void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 48;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim3);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 250;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

}


/* TIM14 init function */
void MX_TIM14_Init(void)
{

  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 48;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 1000;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim14);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance==TIM14) //check if the interrupt comes from TIM3
		{
			Morse_processor();
		}
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOF_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
	
  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = MORSE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(MORSE_PORT, &GPIO_InitStruct);
	
  /*Configure GPIO pins : SEL0, SEL1, SEL2 --- autorun select pins */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Pin = AUTORUN_SEL0PIN;
  HAL_GPIO_Init(AUTORUN_SEL0PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = AUTORUN_SEL1PIN;
  HAL_GPIO_Init(AUTORUN_SEL1PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = AUTORUN_SEL2PIN;
  HAL_GPIO_Init(AUTORUN_SEL2PORT, &GPIO_InitStruct);
	
	  /*Configure GPIO pin : PB1 --- Slave Select for SPI */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


}



/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLED;
  HAL_SPI_Init(&hspi1);
	
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif



