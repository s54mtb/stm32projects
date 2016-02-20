/**
  ******************************************************************************
  * @file    morse.c
  * @brief   Morse generator module 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 S54MTB</center></h2>
  *
  ******************************************************************************
  */ 
#include "morse.h"
#include "command.h"
#include <stdio.h>
#include <string.h>  // strcmp
#include <ctype.h>   // toupper
#include <stdlib.h>
#include "usbd_cdc_if.h"
#include "settings.h"
#include "audio.h"

extern TIM_HandleTypeDef htim14;
extern settings_t settings;

//static char CW_message_text[CW_MAX_MSG_LENGTH]; 
Morse_Status_t MS;


/**
   * @brief CW Table 
	 * 				Sequence start is marked with zero bit
   */

#define CW_special		0x8000 	/// Inidcates special sequence or command in CW generator
#define CH_special		0x80  	/// Inidcates special sequence or command in CW generator
enum
{
	CW_SPC_AA,
	CW_SPC_AR,
	CW_SPC_AS,
	CW_SPC_BK,
	CW_SPC_BT,
	CW_SPC_CL,
	CW_SPC_CT,
	CW_SPC_DO,
	CW_SPC_KN,
	CW_SPC_SK,
	CW_SPC_SN,
	CW_SPC_SOS,
	CW_SPC_BRK
};

const morse_t morse_keys[] =
{
		{'A', 0xfff9},	/// 	11111111 11111001
		{'B', 0xffe8},	/// 	11111111 11101000
		{'C', 0xffea},	/// 	11111111 11101010
		{'D', 0xfff4},	/// 	11111111 11110100
		{'E', 0xfffc},	/// 	11111111 11111100
		{'F', 0xffe2},	/// 	11111111 11100010
		{'G', 0xfff6},	/// 	11111111 11110110
		{'H', 0xffe0},	/// 	11111111 11100000
		{'I', 0xfff8},	/// 	11111111 11111000
		{'J', 0xffe7},	/// 	11111111 11100111
		{'K', 0xfff5},	/// 	11111111 11110101
		{'L', 0xffe4},	/// 	11111111 11100100
		{'M', 0xfffb},	/// 	11111111 11111011
		{'N', 0xfffa},	/// 	11111111 11111010
		{'O', 0xfff7},	/// 	11111111 11110111
		{'P', 0xffe6},	/// 	11111111 11100110
		{'Q', 0xffed},	/// 	11111111 11101101
		{'R', 0xfff2},	/// 	11111111 11110010
		{'S', 0xfff0},	/// 	11111111 11110000
		{'T', 0xfffd},	/// 	11111111 11111101
		{'U', 0xfff1},	/// 	11111111 11110001
		{'V', 0xffe1},	/// 	11111111 11100001
		{'W', 0xfff3},	/// 	11111111 11110011
		{'X', 0xffe9},	/// 	11111111 11101001
		{'Y', 0xffeb},	/// 	11111111 11101011
		{'Z', 0xffec},	/// 	11111111 11101100
		{' ', 0xffef},	/// 	11111111 00000000
		{'0', 0xffdf},	/// 	11111111 11011111
		{'1', 0xffcf},	/// 	11111111 11001111
		{'2', 0xffc7},	/// 	11111111 11000111
		{'3', 0xffc3},	/// 	11111111 11000011
		{'4', 0xffc1},	/// 	11111111 11000001
		{'5', 0xffc0},	/// 	11111111 11000000
		{'6', 0xffd0},	/// 	11111111 11010000
		{'7', 0xffd8},	/// 	11111111 11011000
		{'8', 0xffdc},	/// 	11111111 11011100
		{'9', 0xffde},	/// 	11111111 11011110
		{'.', 0xff95},  ///   11111111 10010101
		{',', 0xffb3},  ///   11111111 10110011
		{':', 0xffb8},  ///   11111111 10111000
		{'?', 0xff8c},  ///   11111111 10001100
		{39,  0xff9e},  ///   11111111 10011110  --- Apostrophe
		{'-', 0xffa1},  ///   11111111 10100001  --- Minus=Hyphen
		{'/', 0xffd2},  ///   11111111 11010010
		{'(', 0xffad},  ///   11111111 10101101
		{')', 0xffad},  ///   11111111 10101101
		{'{', 0xffad},  ///   11111111 10101101
		{'}', 0xffad},  ///   11111111 10101101
		{'[', 0xffad},  ///   11111111 10101101 
		{']', 0xffad},  ///   11111111 10101101 
		{34,  0xff92},  ///   11111111 10010010 --- Quotation "
		{'@', 0xff9a},  ///   11111111 10011010 
		{'=', 0xffd1},  ///   11111111 11010001
		{	CH_special | CW_SPC_AA	, CW_special |	CW_SPC_AA	},  ///      --->   AA, New line	.-.-
		{	CH_special | CW_SPC_AR	, CW_special |	CW_SPC_AR	},  ///      --->   AR, End of message	.-.-.
		{	CH_special | CW_SPC_AS	, CW_special |	CW_SPC_AS	},  ///      --->   AS, Wait	.-...
		{	CH_special | CW_SPC_BK	, CW_special |	CW_SPC_BK	},  ///      --->   BK, Break	-...-.-
		{	CH_special | CW_SPC_BT	, CW_special |	CW_SPC_BT	},  ///      --->   BT, New paragraph	-...-
		{	CH_special | CW_SPC_CL	, CW_special |	CW_SPC_CL	},  ///      --->   CL, Going off the air ("clear")	-.-..-..
		{	CH_special | CW_SPC_CT	, CW_special |	CW_SPC_CT	},  ///      --->   CT, Start copying	-.-.-
		{	CH_special | CW_SPC_DO	, CW_special |	CW_SPC_DO	},  ///      --->   DO, Change to wabun code	-..---
		{	CH_special | CW_SPC_KN	, CW_special |	CW_SPC_KN	},  ///      --->   KN, Invite a specific station to transmit	-.--.
		{	CH_special | CW_SPC_SK	, CW_special |	CW_SPC_SK	},  ///      --->   SK, End of transmission (also VA)	...-.-
		{	CH_special | CW_SPC_SN	, CW_special |	CW_SPC_SN	},  ///      --->   SN, Understood (also VE)	...-.
		{	CH_special | CW_SPC_SOS	, CW_special |	CW_SPC_SOS},  ///      --->   SOS, Distress message	...---...
		{	CH_special | CW_SPC_BRK	, CW_special |	CW_SPC_BRK},  ///      --->   SOS, Distress message	...---...
};

#define CW_NOKEYFOUND		0xffff


/**
   * @brief Init for Morse engine
   * @param None
   * @param None
   * @retval None
   */
void Morse_Init(void)
{
	//settings.cw_message.message = CW_message_text;
	HAL_TIM_Base_Start_IT(&htim14);
}


/**
   * @brief This function initializes and start the morse processor 
   * @param None
   * @param None
   * @retval None
   */
void Morse_start(void)
{
	MS.MPS_message_idx = 0;				// Counter for characters in message
	MS.MPS_pause = 0;							// No pause for start!
	MS.MPS_repeat_counter = 0;	
	MS.timebase = 0;
	MS.duration = 0;
	MS.MorsePorcessroStage = MPS_GetChar;
}


/**
   * @brief Stop morse generator 
   * @param None
   * @param None
   * @retval None
   */
void Morse_stop(void)
{
  Audio_Run(0);	
	HAL_GPIO_WritePin(MORSE_PORT, MORSE_PIN, (settings.out.polarity == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	MS.MorsePorcessroStage = MPS_Stop;
}

/**
   * @brief Restart the morse processor -> fresh start if previously running 
   * @param None
   * @param None
   * @retval None
   */
uint32_t Morse_Restart(void)
{
	uint32_t running = (MS.MorsePorcessroStage != MPS_Stop);
	if (running)
	{
		Morse_stop();
		Morse_start();
	}
	return running;
}


/**
   * @brief Set outputs to "DAH" 
   * @param None
   * @param None
   * @retval None
   */
void Morse_SetDah(void)
{
	if (settings.out.cw) HAL_GPIO_WritePin(MORSE_PORT, MORSE_PIN, (settings.out.polarity == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET); 
	if (settings.audio.cw) Audio_Run(1);
}


/**
   * @brief Set outputs to "DIT" 
   * @param None
   * @param None
   * @retval None
   */
void Morse_SetDit(void)
{
	if (settings.out.cw) HAL_GPIO_WritePin(MORSE_PORT, MORSE_PIN, (settings.out.polarity == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET); 
	if (settings.audio.cw) Audio_Run(1);
}

/**
   * @brief Reset all outputs 
   * @param None
   * @param None
   * @retval None
   */
void Morse_SetNil(void)
{
	HAL_GPIO_WritePin(MORSE_PORT, MORSE_PIN, (settings.out.polarity == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	Audio_Run(0);
}


/**
   * @brief This function is periodically called from Timer 14 interrupt 
   * @param None
   * @param None
   * @retval None
   */
void Morse_processor(void)
{
	// 1kHz here!
  if (MS.MorsePorcessroStage != MPS_Stop)
	{
    // 
		MS.timebase++;
		if (MS.timebase > MS.MPS_pause)
		{
		  HAL_TIM_Base_Stop_IT(&htim14);
			MS.timebase = 0;
			MS.duration += MS.MPS_pause;  // message duration
			switch (MS.MorsePorcessroStage)
			{
				case MPS_GetChar :  // get next char from message and convert to key sequence
					Morse_SetNil();
				  if (settings.cw_message.message[MS.MPS_message_idx] == 0x20) // space
					{
						MS.MorsePorcessroStage = MPS_Space;
					}
					else
					{
						MS.MPS_CurrentKey =   // Get character from message and convert to key
									Morse_CharToKey(settings.cw_message.message[MS.MPS_message_idx]); 
						MS.MPS_key_idx = 0x8000;
						while (MS.MPS_CurrentKey & MS.MPS_key_idx) 
							MS.MPS_key_idx>>=1; // find zero bit in key sequence					
						MS.MPS_key_idx>>=1; // shift right one to initial dit/dah bit
						MS.MPS_pause = settings.cw_message.dottime * 3;  // wait 3 dits at the beginning of the char
						MS.MorsePorcessroStage = MPS_SetOutput;
					}
				break;
					
				case MPS_SetOutput :
					if (MS.MPS_CurrentKey & MS.MPS_key_idx) 
					{
						MS.MPS_pause = settings.cw_message.dottime * 3;
						Morse_SetDah();
					}
					else
					{
						MS.MPS_pause = settings.cw_message.dottime;
						Morse_SetDit();
					}
				  MS.MorsePorcessroStage = MPS_NextKey;				
				break;
					
				case MPS_NextKey:
					Morse_SetNil();
				  MS.MPS_pause = settings.cw_message.dottime;
				  MS.MPS_key_idx>>=1;
				  if (MS.MPS_key_idx == 0)
					{  // finished through key sequence 
						MS.MPS_message_idx++;
						if (settings.cw_message.message[MS.MPS_message_idx] > 0)
						{ // next char in message
						  MS.MorsePorcessroStage = MPS_GetChar;
						}
						else
						{ // message finished, check for repetition
							MS.MPS_repeat_counter++;
							if ((settings.cw_message.repeat>0) &   // repeat finished
								      (MS.MPS_repeat_counter>=settings.cw_message.repeat))
							{
								MS.MorsePorcessroStage = MPS_Stop;
							}
							else
							{
								// Check if message until now did not exceed duration
								if (1000*settings.cw_message.msgperiod > MS.duration) 
								  MS.MPS_pause = 1000*settings.cw_message.msgperiod - MS.duration;
								else // set word space (7 dots time)
								{
									MS.MPS_message_idx = 0;	// Restart the message
									MS.duration = 0;        // Reset duration counter
									MS.MorsePorcessroStage = MPS_GetChar;
									MS.MPS_pause = 7*settings.cw_message.dottime; // wait one space
								}
							}
						}
					}
					else 
					{
						
						MS.MorsePorcessroStage = MPS_SetOutput;
					}
				break;
				
				case MPS_Space:  // space ---> 7x dit time
					Morse_SetNil();
				  MS.MPS_pause = settings.cw_message.dottime*7;
				  MS.MorsePorcessroStage = MPS_NextKey;	
				break;
				
				default: MS.MorsePorcessroStage = MPS_Stop;
					
			}  /* switch (MS.MorsePorcessroStage) */
			
		}  /* if (MS.timebase > MS.MPS_pause)  */
					
  } /*  if (MS.MorsePorcessroStage != MPS_Stop)  */
		HAL_TIM_Base_Start_IT(&htim14);
}


/**
   * @brief This function checks if character ch is valid CW char (has CW key in morse_keys[] table)
   * @param ch - character
   * @retval 0 - no valid char found, 1 - valid char in table
   */
int Morse_Valid_Char(char ch)
{
	int i;
	int valid = 0;
	
	for (i = 0; i<sizeof(morse_keys)/sizeof(morse_t); i++)
	{
		if (ch == morse_keys[i].ch) valid = 1;
	}
	return valid;	
}




/**
   * @brief This function returns CW key from morse_keys[] table
   * @param ch - character
   * @retval key from CW table or 
   */
uint16_t Morse_CharToKey(char ch)
{
	int i;
	
	for (i = 0; i<sizeof(morse_keys)/sizeof(morse_t); i++)
	{
		if (ch == morse_keys[i].ch) return morse_keys[i].key;
	}
	return CW_NOKEYFOUND;	
}

