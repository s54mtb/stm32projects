/**
  ******************************************************************************
  * @file    audio.h
  * @brief   Audio module header file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 S54MTB</center></h2>
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "stm32f0xx.h"                  // Device header
#include "settings.h"

#ifdef __cplusplus
 extern "C" {
#endif

void Audio_Init(settings_t settings);
void Audio_Run(uint8_t on);


    
#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H__ */

/************************ (C) COPYRIGHT S54MTB *****END OF FILE****/
