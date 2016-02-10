
#include "command.h"
#include <stdio.h>
#include <string.h>  // strcmp
#include <ctype.h>   // toupper
#include <stdlib.h>
#include "usbd_cdc_if.h"

/** Globals */
static char cmdstr_buf [1 + MAX_CMD_LEN];
static char argstr_buf [1 + MAX_CMD_LEN];
char newline [1 + MAX_CMD_LEN];


extern uint8_t  refresh_lcd;


/**
 *
 * Command identifiers	
 */
enum
  {
		// Responses from BLE113
		CMD_ON,
		CMD_OFF,
		CMD_STATUS,
		CMD_SENDBYTE,
		CMD_LCDPRINT,
		// Add more 
		CMD_LAST
  };

// command table
struct cmd_st
  {
  const char *cmdstr;
  int id;
  };

	
const char helptext[] = 
" ------------- HELP --------------\n\r"
" \n\r ";
	
/**
 *	Command strings - match command with command ID 
 */
const struct cmd_st cmd_tbl [] =
  {
		{	"ON",						CMD_ON, },
		{ "OFF",	 				CMD_OFF },
		{ "STATUS",				CMD_STATUS },
		{ "ST",						CMD_STATUS },
		{ "SENDBYTE", 		CMD_SENDBYTE },
		{ "SB", 					CMD_SENDBYTE },
		{ "LCDPRINT", 		CMD_LCDPRINT },
		{ "LP",						CMD_LCDPRINT },
  };
	
#define CMD_TBL_LEN (sizeof (cmd_tbl) / sizeof (cmd_tbl [0]))
	
/********** Command functions ***********/
void cmd_on(void);
void cmd_off(void);
void cmd_status(void);
void cmd_sendbyte(char *argstr_buf);
void cmd_lcdprint(char *argstr_buf);
void cmd_unknown(char *argstr_buf);



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
		case CMD_ON:
			cmd_on();	
		break;
		
		case CMD_OFF:
			cmd_off();	
		break;

		case CMD_STATUS:
			cmd_status();	
		break;

		case CMD_SENDBYTE:
			cmd_sendbyte(argstr_buf);	
		break;
		
		case CMD_LCDPRINT:
			cmd_lcdprint(argstr_buf);	
		break;
		
		case CMD_LAST:
			cmd_unknown(cmdstr_buf);
		break;
	}
}


void cmd_on(void)
{
	
	
}



void cmd_off(void)
{
	
	
}




void cmd_status(void)
{
	
	
}




void cmd_sendbyte(char *argstr_buf)
{
	
	
}





void cmd_lcdprint(char *argstr_buf)
{
	refresh_lcd = 1;
}




void cmd_unknown(char *argstr_buf)
{
	CDC_Transmit_FS((uint8_t *)argstr_buf, strlen(argstr_buf));
}

