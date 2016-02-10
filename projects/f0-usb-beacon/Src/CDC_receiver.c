/**
 * Command line edit chars
 */
#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A

static unsigned char line_flag; 			// Flag to indicate new received line
char line_buf[256];
int line_idx = 0;

/**
 * Process received char, check if LF or CR received
 * Set flag when line is done
 */
void process_rx_char(char rx_char)
{

    if (rx_char == CR)  rx_char = LF;   
    if (rx_char == BACKSPACE  ||  rx_char == DEL) 
    {    // process backspace
      	if (line_idx != 0)  
      	{            
        	line_idx--;                      // decrement index
        	#ifdef LOCALECHO
        		putchar (BACKSPACE);               // echo backspace
        		putchar (' ');
        		putchar (BACKSPACE);
        	#endif
      	}
    }
    else 
    {
      	#ifdef LOCALECHO
      		putchar (rx_char);                   // echo 
      	#endif 
      	line_buf[line_idx++] = rx_char; 	   // store character and increment index
    }
    
    // check limit and end line feed
  	if ((line_idx == 0xff)  ||  (rx_char == LF))
  	{
  		line_buf[line_idx-1] = 0;                  // mark end of string
  		line_idx = 0;
			line_flag = 1;
  	}
}

/**
 * Indicate new line received via UART. 
 * Return 1 when new line received. Status flag is cleared to 0.
 * Return 0 if new line is not received yet
 */
unsigned char is_line_received(void)
{
	if (line_flag == 1)
	{
		line_flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

char *get_line_buffer(void)
{
	return line_buf;
}

