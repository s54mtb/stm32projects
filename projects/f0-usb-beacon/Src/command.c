/**
  ******************************************************************************
  * @file    command.c
  * @brief   Command line interpreter.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 S54MTB</center></h2>
  *
  ******************************************************************************
  */ 


#include "stm32f0xx.h"                  // Device header

#include "command.h"
#include <stdio.h>
#include <string.h>  // strcmp
#include <ctype.h>   // toupper
#include <stdlib.h>
#include "usbd_cdc_if.h"
#include "settings.h"
#include "audio.h"
#include "morse.h"



extern settings_t settings;

/** Globals */
static char cmdstr_buf [1 + MAX_CMD_LEN];
static char argstr_buf [1 + MAX_CMD_LEN];
char newline [1 + MAX_CMD_LEN];

/* Flash storage for settings at absolute address */
const settings_t settings_Store[SETTINGS_CATLEN] __attribute__((at(FLASH_STORE_ADR)));
const settings_t backup_settings_Store[SETTINGS_CATLEN] __attribute__((at(FLASH_BACKUP_ADR)));



/**
 *
 * Command identifiers	
 */
enum
  {
		CMD_AUDIO,
		CMD_MSG,
		CMD_MODE,
		CMD_RPT,
		CMD_RUN,
		CMD_STOP,
		CMD_OUT,
		CMD_CAT,
		CMD_STORE,
		CMD_LOAD,
		CMD_DEL,
		CMD_ID,
		CMD_START,
		// Add more 
		CMD_LAST
  };


// command table
struct cmd_st
  {
  const char *cmdstr;
  int id;
  };


	/**
   * @brief Help text 
   */
const char helptext[] = 
	"Commands:\n\r"
  " AUD V/R/F/C/D\n\r"
  " MSG ...Text...\n\r"
  " MODE N/S/D\n\r"
  " RPT T/N/D\n\r"
  " RUN\n\r"
  " OUT P\n\r"
  " CAT\n\r"
  " STORE x\n\r"
  " LOAD x\n\r"
  " DEL x\n\r"
  " ID ...Text...\n\r"
  " STOP\n\r"
	" START x\n\r"
  "\n\r";

/**
 *	Command strings - match command with command ID 
 */
const struct cmd_st cmd_tbl [] =
  {
		{	"AUD",				CMD_AUDIO, },
		{ "MSG",        CMD_MSG,   },
		{ "MODE",       CMD_MODE,  },
		{ "RPT",        CMD_RPT,   },		
		{ "RUN", 				CMD_RUN,   },
		{ "OUT", 				CMD_OUT,   },
		{ "STOP", 			CMD_STOP,  },
		{ "CAT",  			CMD_CAT,   },
		{ "STORE", 			CMD_STORE, },
		{ "LOAD", 			CMD_LOAD,  },
		{ "DEL",  			CMD_DEL,  },
		{ "ID",   			CMD_ID,  },
		{ "START",   		CMD_START,  },
  };
	
#define CMD_TBL_LEN (sizeof (cmd_tbl) / sizeof (cmd_tbl [0]))
	
/********** Command functions ***********/
void cmd_audio(char *argstr_buf);
void cmd_msg(char *argstr_buf);
void cmd_mode(char *argstr_buf);
void cmd_repeat(char *argstr_buf);
void cmd_out(char *argstr_buf);
void cmd_run(void);
void cmd_stop(void);
void cmd_cat(void);
void cmd_id(char *argstr_buf);
void cmd_store(char *argstr_buf, uint8_t del, uint8_t echo);
void cmd_load(char *argstr_buf);
void cmd_del(char *argstr_buf);
void cmd_start(char *argstr_buf);

void cmd_unknown(char *argstr_buf);

void DisplaySettings(settings_t *set);

/*********************************************************************
 * Function:        static unsigned char cmdid_search
 * PreCondition:    -
 * Input:           command string  
 * Output:          command identifier
 * Side Effects:    -
 * Overview:        This function searches the cmd_tbl for a specific 
 *					command and returns the ID associated with that 
 *					command or CID_LAST if there is no matching command.
 * Note:            None
 ********************************************************************/
static int cmdid_search (char *cmdstr) {
	const struct cmd_st *ctp;

	for (ctp = cmd_tbl; ctp < &cmd_tbl [CMD_TBL_LEN]; ctp++) {
		if (strcmp (ctp->cmdstr, cmdstr) == 0) return (ctp->id);
	}

	return (CMD_LAST);
}


/*********************************************************************
 * Function:        char *strupr ( char *src)
 * PreCondition:    -
 * Input:           string  
 * Output:          Uppercase of string
 * Side Effects:    -
 * Overview:        change to uppercase
 * Note:            None
 ********************************************************************/
char *strupr (char *src) {
	char *s;

	for (s = src; *s != '\0'; s++)
		*s = toupper (*s);

	return (src);
}


/*********************************************************************
 * Function:        void cmd_proc (const char *cmd)
 * PreCondition:    -
 * Input:           command line  
 * Output:         	None
 * Side Effects:    Depends on command
 * Overview:        This function processes the cmd command.
 * Note:            The "big case" is here
 ********************************************************************/
void cmd_proc (char *cmd)
{
	char *argsep;
	unsigned int id;
//	char tmpstr_buf[MAX_CMD_LEN];

/*------------------------------------------------
First, copy the command and convert it to all
uppercase.
------------------------------------------------*/
	strncpy (cmdstr_buf, cmd, sizeof (cmdstr_buf) - 1);
	cmdstr_buf [sizeof (cmdstr_buf) - 1] = '\0';
	strupr (cmdstr_buf);
	//skip empty commands
  if (cmdstr_buf[0] == '\0')
		return;
/*------------------------------------------------
Next, find the end of the first thing in the
buffer.  Since the command ends with a space,
we'll look for that.  NULL-Terminate the command
and keep a pointer to the arguments.
------------------------------------------------*/
	argsep = strchr (cmdstr_buf, ' ');
	
	if (argsep == NULL) {
	  argstr_buf [0] = '\0';
	} else {
	  strcpy (argstr_buf, argsep + 1);
	  *argsep = '\0';
	}

/*------------------------------------------------
Search for a command ID, then switch on it.  Each
function invoked here.
------------------------------------------------*/
	id = cmdid_search (cmdstr_buf);
	
	switch (id)
	{
		case CMD_AUDIO:
			cmd_audio(argstr_buf);	
		break;
				
		case CMD_MSG:
			cmd_msg(argstr_buf);	
		break;
				
		case CMD_MODE:
			cmd_mode(argstr_buf);	
		break;
				
		case CMD_RPT:
			cmd_repeat(argstr_buf);	
		break;
		
		case CMD_RUN:
			cmd_run();
		break;
				
		case CMD_STOP:
			cmd_stop();
		break;
				
		case CMD_OUT:
			cmd_out(argstr_buf);
		break;
		
		case CMD_ID:
			cmd_id(argstr_buf);
		break;
		
		case CMD_CAT:
			cmd_cat();
		break;		
				
		case CMD_STORE:
			cmd_store(argstr_buf,0, 1);
		break;
				
		case CMD_LOAD:
			cmd_load(argstr_buf);
		break;
				
		case CMD_DEL:
			cmd_store(argstr_buf,1, 1);
		break;

		case CMD_START:
			cmd_start(argstr_buf);
		break;
		
		case CMD_LAST:
			cmd_unknown(cmdstr_buf);
		break;
	}
}


/**
   * @brief Set audio parameters 
   * @param Arguments string from AUD command
   * @param None
   * @retval None
   */
void cmd_audio(char *argstr_buf)
{
	char *argsep;
	char param[32];
	int x;

	argsep = strchr (argstr_buf, ' ');

	if (argsep != NULL) 
	{
		strcpy (param, argsep + 1);
    *argsep = '\0';
  }	else argstr_buf[0] = 'D';

	switch 	(argstr_buf[0])
	{
		case 'V' : // Volume
			x = atoi(param);
			if ((x<=100) & (x>0)) settings.audio.volume = x;	
				Audio_Init(settings);
		break;
		
		case 'R' : // Run
			x = atoi(param);
			Audio_Run(x);
		break;
		
		case 'F' : // Frequency
			x = atoi(param);
			if ((x<10000) & (x>100)) settings.audio.frequency = x;	
				Audio_Init(settings);
		break;
		
		case 'C' : // CW
			x = atoi(param);
			if (x>0) settings.audio.cw = 1;	else settings.audio.cw = 0;
		break;
		
		case 'D' :
			snprintf(param, 32, "Audio.Volume = %d%%\n\r", settings.audio.volume);
			USB_write((uint8_t *)param, strlen(param));
			snprintf(param, 32, "Audio.Frequency = %dHz\n\r", settings.audio.frequency);
			USB_write((uint8_t *)param, strlen(param));
			snprintf(param, 32, "Audio.CW = %d\n\r", settings.audio.cw);
			USB_write((uint8_t *)param, strlen(param));
		break;
		
	}
	
}


/**
   * @brief Set mode parameters 
   * @param Arguments string from MODE command
   * @param None
   * @retval None
   */
void cmd_mode(char *argstr_buf)
{
	char *argsep;
	char param[32];
	int x;

	argsep = strchr (argstr_buf, ' ');

	if (argsep != NULL) 
	{
		strcpy (param, argsep + 1);
    *argsep = '\0';
  }	else argstr_buf[0] = 'D';

	switch 	(argstr_buf[0])
	{
		case 'N' : // Normal --- WPM
			x = atoi(param);
		if ((x>0) & (x<61)) settings.cw_message.dottime = 60000U/(50*x);
		break;
		
		case 'S' : // Slow --- dot duration in seconds
			x = atoi(param);
		  if ((x>999) & (x<100000)) settings.cw_message.dottime = x;
		  //  settings............
		break;
				
		case 'D' :
			snprintf(param, 32, "CWmessage.DotTime = %dms\n\r", settings.cw_message.dottime);
			USB_write((uint8_t *)param, strlen(param));
		  snprintf(param, 32, "CWmessage.Message:");
			USB_write((uint8_t *)param, strlen(param));
			USB_write((uint8_t *)settings.cw_message.message, strlen(settings.cw_message.message));
		  snprintf(param, 32, "\n\r");
			USB_write((uint8_t *)param, strlen(param));
		break;
		
	}
	
}


/**
   * @brief Set message 
   * @param Message string
   * @param None
   * @retval None
   */
void cmd_msg(char *argstr_buf)
{
	int i = 0;
	int cwok =1;
	char izp[32];
	
	if ((argstr_buf != NULL) & (strlen(argstr_buf)>0))
	{		
		
		strupr(argstr_buf);
		
		while ((i<CW_MAX_MSG_LENGTH) & (argstr_buf[i]>0))
		{
			if (Morse_Valid_Char(argstr_buf[i])==0) 
			{
				cwok = 0;
			  snprintf(izp, 32, "Invalid char: [%c]!\n\r", argstr_buf[i]); 
			  USB_write((uint8_t *)izp, strlen(izp));
			}
			i++;
		}
		if (cwok)
		{
			strncpy(settings.cw_message.message, argstr_buf, CW_MAX_MSG_LENGTH);
			Morse_Restart();
		}
  }
	else
	{
		USB_write((uint8_t *)settings.cw_message.message, 
		           strlen(settings.cw_message.message));	
		snprintf(izp, 32, "\n\r"); 
		USB_write((uint8_t *)izp, strlen(izp));
		
	}
}



/**
   * @brief Set repeat parameters 
   * @param Arguments string from RPT command
   * @param None
   * @retval None
   */
void cmd_repeat(char *argstr_buf)
{
	char *argsep;
	char param[32];
	int x;

	argsep = strchr (argstr_buf, ' ');

	if (argsep != NULL) 
	{
		strcpy (param, argsep + 1);
    *argsep = '\0';
  }
	else argstr_buf[0] = 'D';
	
	switch 	(argstr_buf[0])
	{
		case 'T' : // Message period
			x = atoi(param);
			if ((x<=60000) & (x>=0)) settings.cw_message.msgperiod = x;	
		break;
		
		case 'N' : // Repetitions ... 0 = forever
			x = atoi(param);
			settings.cw_message.repeat = x;
		break;
		
		case 'D' :
			snprintf(param, 32, "CWmessage.period = %ds\n\r", settings.cw_message.msgperiod);
			USB_write((uint8_t *)param, strlen(param));
			snprintf(param, 32, "CWmessage.repeat = %d times\n\r", settings.cw_message.repeat);
			USB_write((uint8_t *)param, strlen(param));
		break;
		
	}
	
}



/**
   * @brief Set output parameters 
   * @param Arguments string from OUT command
   * @param None
   * @retval None
   */
void cmd_out(char *argstr_buf)
{
	char *argsep;
	char param[32];
	int x;

	argsep = strchr (argstr_buf, ' ');

	if (argsep != NULL) 
	{
		strcpy (param, argsep + 1);
    *argsep = '\0';
  }
	else argstr_buf[0] = 'D';
	
	switch 	(argstr_buf[0])
	{
		case 'P' : // Output polarity
			x = atoi(param);
			if ((x<=3) & (x>=0)) settings.out.polarity = x;	
		break;
		
		case 'C' : // CW
			x = atoi(param);
			if (x>0) settings.out.cw = 1;	else settings.out.cw = 0;
		break;
				
		case 'D' :
			snprintf(param, 32, "Out.polarity = %d\n\r", settings.out.polarity);
			USB_write((uint8_t *)param, strlen(param));
			snprintf(param, 32, "Out.CW = %d\n\r", settings.out.cw);
			USB_write((uint8_t *)param, strlen(param));
		break;
		
	}
	
}



/**
   * @brief Define settings ident 
   * @param Message string
   * @param None
   * @retval None
   */
void cmd_id(char *argstr_buf)
{
	char izp[32];
	
	if ((argstr_buf != NULL) & (strlen(argstr_buf)>0))
	{		
		strupr(argstr_buf);
		strncpy(settings.Ident, argstr_buf, 8);
  }
	else
	{
		USB_write((uint8_t *)settings.Ident, 
		           strlen(settings.Ident));	
		snprintf(izp, 32, "\n\r"); 
		USB_write((uint8_t *)izp, strlen(izp));
		
	}
}

/**
   * @brief Store settinfgs to specified position 
   * @param Message string
   * @param del - 0 = normal write, 1 - delete 
   * @retval None
   */
void cmd_store(char *argstr_buf, uint8_t del, uint8_t echo)
{
	char izp[32];
	int x,i;
	HAL_StatusTypeDef FLstatus;
	FLASH_EraseInitTypeDef eraseinit;
	uint32_t PageError;
	uint32_t stadr;
	uint64_t buf;
	uint32_t fladr;
	uint8_t *adr;
	settings_t 	tmp_settings;
	
	if ((argstr_buf != NULL) & (strlen(argstr_buf)>0))
	{		
    x = atoi(argstr_buf);
		if ((x>0) & (x<=SETTINGS_CATLEN))
		{
			x--; // x = 1...n 
			FLstatus = HAL_FLASH_Unlock();  

			// Erase backup sector
			eraseinit.NbPages = 1;
			eraseinit.PageAddress = FLASH_BACKUP_ADR;
			eraseinit.TypeErase = TYPEERASE_PAGES;
			FLstatus = HAL_FLASHEx_Erase(&eraseinit, &PageError);   
			
			// copy store to backup
			for (i = 0; i< SETTINGS_CATLEN; i++)
			{
				adr = (uint8_t *)&tmp_settings;
				memcpy(adr, &settings_Store[i], sizeof(settings_t));
				fladr = FLASH_BACKUP_ADR+sizeof(settings_t)*(i);
				for (stadr = 0; stadr<(sizeof(settings_t)/8); stadr++)
				{
					memcpy(&buf, adr, 8); 
					FLstatus =  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, fladr, buf);
					adr+=8;
					fladr+=8;
				}

		  }
			if (echo)
			{
			  snprintf(izp, 32, "Backup... status %d\n\r", FLstatus); 
		    USB_write((uint8_t *)izp, strlen(izp));
			}
			// erase store 
			eraseinit.NbPages = 1;
			eraseinit.PageAddress = FLASH_STORE_ADR;
			eraseinit.TypeErase = TYPEERASE_PAGES;
			FLstatus = HAL_FLASHEx_Erase(&eraseinit, &PageError);   
			
			// and write to it from new data and backup

			for (i = 0; i< SETTINGS_CATLEN; i++)
			{
        if (i == x) // actual location is written with new data
				{
					if (del == 1) // delete
					{
						// do nothing, keep erased flash
					}
					else
					{							
						adr = (uint8_t *)&settings;
						fladr = FLASH_STORE_ADR+sizeof(settings_t)*x;
						for (stadr = 0; stadr<(sizeof(settings_t)/8); stadr++)
						{
							memcpy(&buf, adr, 8); 
							FLstatus =  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, fladr, buf);
							adr+=8;
							fladr+=8;
						}
					}
				}  // if i==x
				else
				{
					adr = (uint8_t *)&tmp_settings;
					memcpy(adr, &backup_settings_Store[i], sizeof(settings_t));
					fladr = FLASH_STORE_ADR+sizeof(settings_t)*(i);
					for (stadr = 0; stadr<(sizeof(settings_t)/8); stadr++)
					{
						memcpy(&buf, adr, 8); 
						FLstatus =  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, fladr, buf);
						adr+=8;
						fladr+=8;
					}
					
				}
			}  /*  for ... */
			if (echo)
			{
				snprintf(izp, 32, "Writing... status %d\n\r", FLstatus); 
				USB_write((uint8_t *)izp, strlen(izp));
			}


			FLstatus = HAL_FLASH_Lock();  
		}
  }
}



/**
   * @brief Load settinfgs from specified position 
   * @param Message string
   * @param del - 0 = normal write, 1 - delete 
   * @retval None
   */
void cmd_load(char *argstr_buf)
{
	int x;
	uint8_t *adr;
	char izp[32];
	
	if ((argstr_buf != NULL) & (strlen(argstr_buf)>0))
	{		
    x = atoi(argstr_buf);
		if ((x>0) & (x<=SETTINGS_CATLEN))
		{
			x--; // x = 1...n 
			adr = (uint8_t *)&settings; 
			memcpy(adr, &settings_Store[x], sizeof(settings_t));
			snprintf(izp, 32, "Loaded %d: ", x+1); 
		  USB_write((uint8_t *)izp, strlen(izp));			
			DisplaySettings(&settings);
			snprintf(izp, 32, "\n\r"); 
			USB_write((uint8_t *)izp, strlen(izp));		
    }
  }
}





/**
   * @brief Catalog stored settings
   * @param None
   * @param None
   * @retval None
   */
void cmd_cat(void)
{
	int i;
	char izp[32];
	uint8_t firstchar;
	
	for (i=0; i<SETTINGS_CATLEN; i++)
	{
		// Print number
		snprintf(izp, 32, "%x: ",i+1);  // avoid 0
		USB_write((uint8_t *)izp, strlen(izp));
		// Print out settings ID
		firstchar = settings_Store[i].Ident[0];
		if ((strlen(settings_Store[i].Ident) > 0) & (firstchar!=0xff))
		{
			DisplaySettings((settings_t *)&settings_Store[i]);
		}
		else
		{
			snprintf(izp, 32, " <Empty> ");
			USB_write((uint8_t *)izp, strlen(izp));
		}

		snprintf(izp, 32, "\n\r"); 
		USB_write((uint8_t *)izp, strlen(izp));
		
	}
	
}



void DisplaySettings(settings_t *set)
{
	char izp[32];
	
	snprintf(izp, 32, "%8s - ",set->Ident); 
	USB_write((uint8_t *)izp, strlen(izp));
	snprintf(izp, 32, " a.V=%d%% ", set->audio.volume);
	USB_write((uint8_t *)izp, strlen(izp));
	snprintf(izp, 32, " a.f=%dHz ", set->audio.frequency);
	USB_write((uint8_t *)izp, strlen(izp));
	snprintf(izp, 32, " a.cw=%d ", set->audio.cw);
	USB_write((uint8_t *)izp, strlen(izp));

	snprintf(izp, 32, "CW.t=%dms ", set->cw_message.dottime);
	USB_write((uint8_t *)izp, strlen(izp));
	snprintf(izp, 32, " CW.msg :'");
	USB_write((uint8_t *)izp, strlen(izp));
	USB_write((uint8_t *)set->cw_message.message, strlen(set->cw_message.message));
	snprintf(izp, 32, "' ");
	USB_write((uint8_t *)izp, strlen(izp));

	snprintf(izp, 32, " Out.p=%d ", set->out.polarity);
	USB_write((uint8_t *)izp, strlen(izp));
	snprintf(izp, 32, " Out.cw=%d ", set->out.cw);
	USB_write((uint8_t *)izp, strlen(izp));
	if (set->autorun > 0)
	{
		if (set->autorun == 1) snprintf(izp, 32, " AR! "); 
		if (set->autorun == 2) snprintf(izp, 32, " SAR! "); 
	  USB_write((uint8_t *)izp, strlen(izp));
  }
	
}




/**
   * @brief Run command
   * @param None
   * @param None
   * @retval None
   */
void cmd_run(void)
{
	Morse_start();
}


/**
   * @brief Stop command
   * @param None
   * @param None
   * @retval None
   */
void cmd_stop(void)
{
	Morse_stop();
}


/**
   * @brief Mark selected entry as startup/autorun
   * @param String 1...8 or 101...108
	 *                1...8 ---> "Normal" autorun, 101...108 --> Selective autorun 
   * @param None
   * @retval None
   */
void cmd_start(char *argstr_buf)
{
	int x;

	if ((argstr_buf != NULL) & (strlen(argstr_buf)>0))
	{		
    x = atoi(argstr_buf);
		if ((x>=0) & (x<=2))
		{
			settings.autorun = x;
			
//			x--; // x = 1...n 
//			memcpy(&tmpsettings, &settings, sizeof(settings_t));  // temp. storage
//			for (n=0; n<SETTINGS_CATLEN; n++)
//			{
//				snprintf(izp, 32, "%d", n+1); 
//				cmd_load(izp);
//				if (x<100)
//				{
//				  settings.autorun = (n==x) ? 1 : 0;
//				}
//				else
//				{
//				  settings.autorun = ((n+100)==x) ? 100 : settings.autorun;					
//				}
//				cmd_store(izp, 0, 0);		// normal write
//			} // for
//			memcpy(&settings, &tmpsettings, sizeof(settings_t));  // temp. storage
    }
  }
}


uint8_t selectpins(void)
{
	uint8_t sp =0;
	GPIO_PinState pins;

	pins = HAL_GPIO_ReadPin(AUTORUN_SEL0PORT, AUTORUN_SEL0PIN);
	sp |= (pins==GPIO_PIN_SET) ? 0x01 : 0;
	pins = HAL_GPIO_ReadPin(AUTORUN_SEL1PORT, AUTORUN_SEL1PIN);
	sp |= (pins==GPIO_PIN_SET) ? 0x02 : 0;
	pins = HAL_GPIO_ReadPin(AUTORUN_SEL2PORT, AUTORUN_SEL2PIN);
	sp |= (pins==GPIO_PIN_SET) ? 0x04 : 0;
	
	return sp;
}




int cmd_isautorun(void)
{
	int i;
	uint8_t firstchar;
	int autorun = -1;
	
	for (i=0; i<SETTINGS_CATLEN; i++)
	{
		firstchar = settings_Store[i].Ident[0];
		if ((strlen(settings_Store[i].Ident) > 0) & (firstchar!=0xff)) // is not empty? 
		{
			if (settings_Store[i].autorun == 1) 
			{
				autorun = i+1;  // remember first autorun parameter equal to 1 in catalog
				break;
			}
			if (settings_Store[i].autorun == 2) 
			{
				if (i == selectpins()) 
				{
					autorun = i+1;  // remember first autorun parameter equal to 2 in catalog at location equal to selectpins()
					break;					
				}
			}
		}
	}
	
	return autorun;
}

void cmd_autorun(int ar)
{	
	memcpy(&settings, &settings_Store[ar-1], sizeof(settings_t));
	Morse_start();
}



void cmd_unknown(char *argstr_buf)
{
	USB_write((uint8_t *)helptext, strlen(helptext));
}

