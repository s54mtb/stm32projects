/**
 * Defines for your entire project at one place
 * 
 * @author     Tilen Majerle
 * @email      tilen@majerle.eu
 * @website    http://stm32f4-discovery.com
 * @version    v1.0
 * @ide        Keil uVision 5
 * @license    GNU GPL v3
 *	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H

/* Put your global defines for all libraries here used in your project */

/* Defines for RCC settings for system */
/* I've added these defines in options for target in Keil uVision for each target different settings */
//#define RCC_OSCILLATORTYPE    RCC_OSCILLATORTYPE_HSE /*!< Used to select system oscillator type */
//#define RCC_PLLM              8                      /*!< Used for PLL M parameter */
//#define RCC_PLLN              360                    /*!< Used for PLL N parameter */
//#define RCC_PLLP              2                      /*!< Used for PLL P parameter */
//#define RCC_PLLQ              7                      /*!< Used for PLL Q parameter */
//#define RCC_PLLR              10                     /*!< Used for PLL R parameter, available on STM32F446xx */


/* Control pins */
/* RS - Register select pin */
#define HD44780_RS_PORT            GPIOA
#define HD44780_RS_PIN            GPIO_Pin_6
/* E - Enable pin */
#define HD44780_E_PORT            GPIOA
#define HD44780_E_PIN                GPIO_Pin_4
 
/* D4 - Data 4 pin */
#define HD44780_D4_PORT            GPIOA
#define HD44780_D4_PIN            GPIO_Pin_3
/* D5 - Data 5 pin */
#define HD44780_D5_PORT            GPIOA
#define HD44780_D5_PIN            GPIO_Pin_2
/* D6 - Data 6 pin */
#define HD44780_D6_PORT            GPIOA
#define HD44780_D6_PIN            GPIO_Pin_1
/* D7 - Data 7 pin */
#define HD44780_D7_PORT            GPIOA
#define HD44780_D7_PIN            GPIO_Pin_0

#define CDC_TOGGLE_RX_LED				1
#define CDC_TOGGLE_RX_LED_CLKENABLE    __HAL_RCC_GPIOA_CLK_ENABLE()
#define CDC_TOGGLE_RX_LED_PORT            GPIOA
#define CDC_TOGGLE_RX_LED_PIN            GPIO_Pin_7

#endif
