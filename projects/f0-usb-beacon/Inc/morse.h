/**
  ******************************************************************************
  * @file    morse.h
  * @brief   Morse generator module header file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 S54MTB</center></h2>
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MORSE_H__
#define __MORSE_H__

#include "stm32f0xx.h"                  // Device header
#include "settings.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
#define MORSE_PIN			GPIO_PIN_0
#define MORSE_PORT		GPIOA

#define AUTORUN_SEL0PIN			GPIO_PIN_1
#define AUTORUN_SEL1PIN			GPIO_PIN_2
#define AUTORUN_SEL2PIN			GPIO_PIN_3
#define AUTORUN_SEL0PORT 		GPIOA
#define AUTORUN_SEL1PORT 		GPIOA
#define AUTORUN_SEL2PORT 		GPIOA

typedef struct 
{
	char 			ch;
	uint16_t		key;
} morse_t;

typedef enum  // Morse processor stages
{
	MPS_Stop,
	MPS_GetChar,
	MPS_SetOutput,
	MPS_NextKey,
	MPS_Space,
} MPS_t;

typedef struct 
{
	MPS_t MorsePorcessroStage;
	uint32_t timebase, duration;
  int MPS_repeat_counter;
  int MPS_message_idx;
  int MPS_key_idx;
  int MPS_pause;
  uint16_t MPS_CurrentKey;
} Morse_Status_t;

void Morse_Init(void);
void Morse_processor(void);
int Morse_Valid_Char(char ch);
uint16_t Morse_CharToKey(char ch);
void Morse_start(void);
void Morse_stop(void);
uint32_t Morse_Restart(void);

    
#ifdef __cplusplus
}
#endif

#endif /* __MORSE_H__ */

/************************ (C) COPYRIGHT S54MTB *****END OF FILE****/
