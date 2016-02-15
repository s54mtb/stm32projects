/**
  ******************************************************************************
  * @file    settings.h
  * @brief   settings header file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 S54MTB</center></h2>
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "stm32f0xx.h"                  // Device header

#ifdef __cplusplus
 extern "C" {
#endif
	 
#define CW_MAX_MSG_LENGTH			64
#define SETTINGS_CATLEN				8
#define FLASH_STORE_ADR				0x8007c00
#define FLASH_BACKUP_ADR      0x8007800

typedef struct 
{
	uint8_t 			volume;					/// 1...100% --> 0,5 to 50% PWM
	uint16_t 			frequency;      /// in Hz ... 100Hz to 10kHz
  uint8_t				run;						/// 1 -- audio active, 0 -- audio silent
  uint8_t				cw;	            /// 1 -- audio output active with CW generator
} audio_t;

typedef struct 
{
	char		 			message[CW_MAX_MSG_LENGTH];		/// Message buffer
	uint32_t 			dottime;        /// in ms ... dot time in ms 
	uint32_t			msgperiod;			/// Message repetition period in seconds
  uint16_t			repeat;					/// Number of repetitions (0 for permanent)
} cw_msg_t;

typedef struct
{
	uint8_t				polarity;				/// Output polarity
	uint8_t				cw;							/// 1 -- output active with CW generator
} out_t;
	 
typedef struct 
{
	char				Ident[10];				/// Identification
	audio_t			audio;            /// Audio settings
	cw_msg_t		cw_message;				/// Message settings
	out_t				out;							/// Output settings
	uint8_t 		autorun;
	uint32_t		__padding;				/// Gap for flash programming
} settings_t;
    
#ifdef __cplusplus
}
#endif

#endif /* __SETTINGS_H__ */

/************************ (C) COPYRIGHT S54MTB *****END OF FILE****/
