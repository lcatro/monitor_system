
#include <reg52.h>

#include "monitor_system_chip.h"
						
#include "device_io_interrupt.h"	   
#include "device_delay.h"

#define WAIT_TIME_RECHECK_INTERRUPT 200

sbit   io_interrupt_0=P3^2;
sbit   io_interrupt_1=P3^3;

static recall_io_interrupt recall_interrupt_0=null;
static recall_io_interrupt recall_interrupt_1=null;

static int  io_interrupt_select_0=ELECT_HEIGHT;
static int  io_interrupt_select_1=ELECT_HEIGHT;

void io_interrupt_on(void) {
	EA=1;
}

void io_interrupt_off(void) {
	EA=0;
}

void set_interrupt_0(Interrupt_Stat_Code stat_code) {
	EX0=stat_code;
}

void set_interrupt_1(Interrupt_Stat_Code stat_code) {
	EX1=stat_code;
}

void set_io_interrupt_select_0(int io_interrupt_select) {
	io_interrupt_select_0=io_interrupt_select;  		  
	IT0=io_interrupt_select_0;
}

void set_io_interrupt_select_1(int io_interrupt_select) {
	io_interrupt_select_1=io_interrupt_select;  		  
	IT1=io_interrupt_select_1;
}

void io_interrupt_init(int io_port,int io_interrupt_select,recall_io_interrupt recall_fun_address) {
	io_interrupt_on();
	switch (io_port) {
		case PORT_INTERRUPT_0:
			set_interrupt_0(Interrupt_Run);
			set_io_interrupt_select_0(io_interrupt_select);
			recall_interrupt_0=recall_fun_address;
			return;
		case PORT_INTERRUPT_1:
			set_interrupt_1(Interrupt_Run);
			set_io_interrupt_select_1(io_interrupt_select);
			recall_interrupt_1=recall_fun_address;
	}	
}

static void io_interrupt_server_0(void) interrupt 0 {
	delay_50us(WAIT_TIME_RECHECK_INTERRUPT/5);

	if (io_interrupt_select_0==io_interrupt_0) return;
	//  TIPS!可能是由于其他的电磁感应原因导致中断被引发
	//  等待一下然后再读取该中断引脚的电压看看是否是真的被激发中断..
	if (null!=recall_interrupt_0)
		recall_interrupt_0(PORT_INTERRUPT_0,io_interrupt_select_0);
}

static void io_interrupt_server_1(void) interrupt 1 {
	delay_50us(WAIT_TIME_RECHECK_INTERRUPT/5);

	if (io_interrupt_select_1==io_interrupt_1) return;
	if (null!=recall_interrupt_1)
		recall_interrupt_1(PORT_INTERRUPT_1,io_interrupt_select_1);
} 