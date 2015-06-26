
#include <stdio.h>
#include <string.h>

#include "monitor_system_chip.h"				
#include "monitor_system_code_trance.h"
#include "monitor_system_code_password.h" 

#include "device_delay.h"
#include "device_io_interrupt.h"

#define LEN_LCD_PRINT_MAX     16
#define LEN_PROGRESS_MAX_SHOW 10

#define NUM_PASSWORD_MAX       3
								
#define KEY_CODE_ERROR		  -1
#define KEY_CODE_BACKSPACE 	  12
#define KEY_CODE_SUBMIT 	  14

#define TIME_WAIT_FOR_START_COMPUTER       		   10	// TIPS!1毫秒通电给开机开关时间
#define TIME_WAIT_FOR_PUT_PASSWORD       	    20000
#define TIME_WAIT_FOR_CHECK_NEW_DATA     	   100000	// TIPS!5秒刷新一次数据
#define TIME_WAIT_FOR_SHOW_CREATE_NEW_PASSWORD  80000	// TIPS!4秒
//如果重置新密码还需要等待4秒的话那总计需要等待9秒..

Interrupt_Stat_Code  stat_computer=Interrupt_Stop;
int                  password_err_num=0;

sbit io_switch_elect =P3^5;
	  
void block_forever(void) {
	while (true);
}		

void start_computer(void) {
	io_switch_elect=ELECT_HEIGHT;
	delay_50us(TIME_WAIT_FOR_START_COMPUTER);
	io_switch_elect=ELECT_LOW;
}

int get_key_code(void) {
	int  key=scankey();
	if (KEY_CODE_ERROR==key)
		return KEY_CODE_ERROR;
	else if (0<=key && key<=2)
		return ++key;
	else if (4<=key && key<=6)
		return key;
	else if (8<=key && key<=10)
		return --key;
	else if (13==key)
		return 0;
	return key;
}

void lcd_draw_data(Data_Code recv_code,Data_Trance_Code recv_trance_code,char *recv_str) {
	char use_rate_str[LEN_LCD_PRINT_MAX-LEN_PROGRESS_MAX_SHOW]={0};
	int  use_rate=0;

	if (DATA_ACK!=recv_trance_code)
		return;
	
	lcd_clear();
	switch (recv_code) {
		case Get_Data_CPU_UseRate:
			lcd_print_line("CPU_UseRate:");
			break;
		case Get_Data_Memory_Size:
		//  TIPS! 这里不会输出物理内存的长度
			return;
		case Get_Data_Memory_UseRate:
			lcd_print_line("Memory_UseRate:");
	}
	memcpy(&use_rate,recv_str,sizeof(use_rate));
	sprintf(use_rate_str,"  %2d%%",use_rate);

	lcd_draw_progress(LEN_PROGRESS_MAX_SHOW,use_rate/10);
	lcd_print(use_rate_str);
}

void recall_recv_packet(void) {				   
	Data_Code   	  get_code							   =Get_Run;
	Data_Trance_Code  rtn_code							   =DATA_ERR;
	char 			  recv_str[LEN_TRANCE_DATA_ADDITIONAL] ={0};

	unsigned char     password[LEN_PASSWORD]               ={0};
	int               len_password                         = 0 ; 

	interrupt_trance_off();

	rtn_code=recv_reqution_packet(recv_str,&get_code);

	if (DATA_ERR!=rtn_code) {
		if (Interrupt_Stop==stat_computer) {
			stat_computer=Interrupt_Run;
			password_err_num=0;
			set_io_interrupt_select_0(ELECT_LOW);
		}
		switch (get_code) {
			case Get_Run:
				send_reqution_packet(Get_Run,null,0);
				goto exit;		 
			case Get_Password:
				//  小心被破解	
				if (Interrupt_Stop==stat_computer) return;
				password_read(password,&len_password);
				
				send_reqution_packet(Get_Password,password,len_password);
				goto exit;
			case Set_Password:  //  以后再拓展更安全的改密码方法.. 2013.12.13
				//  小心被破解
				if (Interrupt_Stop==stat_computer) return;

				password_write(recv_str,strlen(recv_str));
				
				lcd_draw_report("PASSWORD!","had been change!");
				goto exit;
		}
		lcd_draw_data(get_code,rtn_code,recv_str); 
		send_reqution_packet(Get_Run,null,0);
	} else
		lcd_draw_report("RECV ERR!","recallrecvpacket");
exit:
	interrupt_trance_on();
}

void recall_check_computer_power(int io_port_num,int io_port_stat) {
	io_interrupt_off();

	if (PORT_INTERRUPT_0==io_port_num ) {
		if (ELECT_HEIGHT==io_port_stat)	{
			stat_computer=Interrupt_Run;
			set_io_interrupt_select_0(ELECT_LOW);
		} else {
			stat_computer=Interrupt_Stop;
			set_io_interrupt_select_0(ELECT_HEIGHT);
		}
	}

	io_interrupt_on();
}

void lcd_draw_password(void) {
	char password_buffer_sorc [LEN_PASSWORD]  ={0};
	char print_buffer         [LEN_PASSWORD]  ={0};
	int  len_password_buffer				  = 0 ;
	int  code_key							  = 0 ;
	int  index_input						  = 0 ;
 
	if (NUM_PASSWORD_MAX<=password_err_num) block_forever();

	password_read(password_buffer_sorc,&len_password_buffer);
	if (0==len_password_buffer) {
		lcd_draw_report("PASSWORD_RESET:","now input again!");
		password_write(STR_PASSWORD_RESET,LEN_STR_PASSWORD_RESET);
		delay_50us(TIME_WAIT_FOR_SHOW_CREATE_NEW_PASSWORD);
		return;
	}

	sprintf(print_buffer,"%s%d","Password:",password_err_num);
	lcd_clear();
	lcd_print_line(print_buffer);
	memset(print_buffer,0,LEN_PASSWORD);
					 
	while (true) {	
		code_key=get_key_code();
		if (KEY_CODE_SUBMIT==code_key)
			break;
		else if (KEY_CODE_BACKSPACE==code_key) {
			if (0<index_input) {
				print_buffer[index_input-1]=0;
				--index_input;

				lcd_clear_line(2);
				lcd_print(print_buffer);
			}
			goto next_loop;
		}
		else if (KEY_CODE_ERROR==code_key)
			goto next_loop;

		print_buffer[index_input]=ASCII_OFFSET_NUMBER+code_key;
		lcd_print(&print_buffer[index_input]);
		++index_input;
next_loop:
		delay_50us(TIME_WAIT_FOR_PUT_PASSWORD);
	}

	if (0==strcmp(password_buffer_sorc,print_buffer)) {
		start_computer(); 
		password_err_num=0;

		lcd_clear();	  
		lcd_print_line("START SUCCESS!");
		lcd_print_line("waitting for run");
		//  注意这里并没有直接修改中断方式
		//  而是让中断被触发的时候再去修改中断的触发方式
		stat_computer=Interrupt_Run;
	} else {
		++password_err_num;
		if (NUM_PASSWORD_MAX<password_err_num)
			block_forever();
	}
}

void get_new_data(void) {
	// 把传输数据的工作交给上位机	
	// 当主机没有启动时,那就应该是输入密码的时候
	if (Interrupt_Stop==stat_computer) 	 
		lcd_draw_password();
}

void main(void) {
	io_interrupt_init(PORT_INTERRUPT_0,ELECT_HEIGHT,&recall_check_computer_power);
	AT24C02_init();		
	lcd_init();
	trance_init(&recall_recv_packet);

	while (true) {
		get_new_data();
		delay_50us(TIME_WAIT_FOR_CHECK_NEW_DATA);
	} 
}
