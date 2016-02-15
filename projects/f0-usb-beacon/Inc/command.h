#ifndef _COMMAND_H_
#define _COMMAND_H_


#define MAX_CMD_LEN 64 

void cmd_proc (char *cmd);
int cmd_isautorun(void);
void cmd_autorun(int ar);

#endif

