

#include <reg52.h>
						  	
#include "device_delay.h"
#include "device_lcd_1602.h"

/*
P2口输出数据
P1^0 命令端
P1^1 读写数据端
P1^2 使能端*/
		 
sbit lcdrs=P1^0;
sbit lcdrw=P1^1;
sbit lcden=P1^2; 

static int code_column=5;
static int code_line=8;

static const unsigned int time_calcu=1000;

static void wait_for_lcd(void) {
	delay_50us(time_calcu/10);
}

static void write_com(char com)		
{
	lcdrs=0;
	P2=com;	
	lcden=1;
	wait_for_lcd();						 			  
	lcden=0;
}

static void write_data(char data_in)
{
	lcdrs=1;
	P2=data_in;
	lcden=1;
	wait_for_lcd();
	lcden=0;
}

//  启用光标时光标会闪烁
/*
void lcd_cursor_on(void) {		 
    write_com(0x0E);
}
*/
void lcd_cursor_off(void) {		 
    write_com(0x0C);
}

void lcd_print_crlf(void)
{
	write_com(0x80+0x40); 		//更改数据指针，让字符换行
	wait_for_lcd();
}

void lcd_print(char *str)	
{
  	int i;
	for(i=0;i<16;i++) {
	   	if (str[i]=='\0') break;		
		write_data(str[i]);
	}	
}

void lcd_print_line(char *str) {  
  	int i;
	for(i=0;i<16;i++) {
	   	if (str[i]=='\0') break;		
		write_data(str[i]);
	}
	lcd_print_crlf();
}

/*  暂时还没有什么思路来写这个东西
void lcd_clear_backspace(void) {
}*/

static void lcd_set_address_ddram(char address_ddram) {
	write_com(0x80+address_ddram);
} 

void lcd_set_write_at_line (int num_line) {
	if (1==num_line)
		lcd_set_address_ddram(ADDRESSS_LINE_1);
	else   
		lcd_set_address_ddram(ADDRESSS_LINE_2);
}

void lcd_clear(void) {
    write_com(0x01);	
}

void lcd_clear_line(int num_line) {
	lcd_set_write_at_line((1==num_line)?num_line:2);
	lcd_print_line(STR_SPACE); 
	lcd_set_write_at_line((1==num_line)?num_line:2);
}
  
void lcd_init()	
{	
	lcdrw=0;
	write_com(0x0f);
	write_com(0x38);		
	lcd_cursor_off();
	lcd_clear();		       
	write_com(0x80);			//设置初始化数据指针，是在读指令的操作里进行的
}
//  在CGRAM 中写图像 [注意!这里面的代码还没有成功运行!.. --2013.11.30]
/*
static int calcu_cgram_address(int line,int column) {
	if (line<0x8 || column<0x8)
		return column<<4+line;
	return 0;
}

void create_new_code(const char *code_all_line,int address) {
	int i=0;
	write_com(1<<7+address);	

	for (;i<code_line;i++)
			write_data(*(code_all_line+i));
}

void print_new_code(int new_code_address) {
	write_data(new_code_address);
} */
//  拓展函数
void lcd_draw_report(char *line1,char *line2) {
	lcd_clear();
	lcd_print_line(line1);
	lcd_print_line(line2);
}

void lcd_draw_progress(int progress_max,int progress_value) {
	int i=0;
	if (progress_value>progress_max) return;
	for (;i<progress_value;i++)
		lcd_print(ASCII_CHAR_FULL);
	for (;i<progress_max;i++) 
		lcd_print(ASCII_CHAR_EMPTY);
}