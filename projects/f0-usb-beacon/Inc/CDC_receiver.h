#ifndef GETLINE_H_
#define GETLINE_H_

extern void process_rx_char(char rx_char);
extern unsigned char is_line_received(void);
extern char line_buf[256];
extern char *get_line_buffer(void);

#endif /*GETLINE_H_*/

