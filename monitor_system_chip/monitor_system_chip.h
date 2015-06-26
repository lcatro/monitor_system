
#ifndef _MONITOR_SYSTEM_CHIP_H__
#define _MONITOR_SYSTEM_CHIP_H__

#include <reg52.h>

#define null 0
								
#define ELECT_HEIGHT 1
#define ELECT_LOW    0

#define ASCII_OFFSET_NUMBER 48

#define STR_PASSWORD_RESET     "1234"
#define LEN_STR_PASSWORD_RESET     4

typedef enum {	
	true=1,
	false=0,
} bool;

//  º¯ÊýÉùÃ÷

//#include "device_lcd_1602.h"
//#include "device_52_trance.h"
//#include "device_eeprom.h"
//#include "device_key.h"

void lcd_init(void);

void lcd_print(char *str);	   
void lcd_print_crlf(void);
void lcd_print_line(char *str);

void lcd_clear(void);
void lcd_clear_line(int num_line);

void lcd_cursor_off(void);
void lcd_cursor_on(void);

void create_new_code(const char *code_all_line,int address);
void print_new_code(int address);

void lcd_draw_report(char *line1,char *line2);
void lcd_draw_progress(int progress_max,int progress_value);



typedef void (*recall_recv)(int recv_data);

void port_init(recall_recv in);
void port_send(char send_data);

void interrupt_trance_on(void) ;
void interrupt_trance_off(void);



void AT24C02_init();
void AT24C02_writeData(unsigned char address,unsigned char numBytes,unsigned char* buf);
void AT24C02_readData(unsigned char beginAddr,unsigned char dataSize,unsigned char* buf);



int scankey(void);

#endif