
#ifndef _DEVICE_LCD_1602_H__
#define _DEVICE_LCD_1602_H__

char    char_full[2]={255,0};

#define ASCII_CHAR_FULL	 char_full
#define ASCII_CHAR_EMPTY "_"
					  
#define PROGRESS_MIN        0
#define PROGRESS_MAX       16

#define STR_SPACE        "                "

#define ADDRESSS_LINE_1  0x00 
#define ADDRESSS_LINE_2  0x40

void lcd_init(void);

void lcd_set_write_at_line (int num_line);

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

#endif