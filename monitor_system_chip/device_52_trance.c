
#include <reg52.h>
#include <intrins.h>

#include "device_52_trance.h"

/*

   定时器初值=256-(fosc*(SMOD+1)/384*波特率)
			 =256-(11059200*(0+1)/384*9600)	 <- 24MHz晶振
			 =256-3=253
			 =0xFD

*/

recall_recv recall_fun=0;

static void delay_port_send(void) {_nop_();_nop_();}

void port_init(recall_recv in) {
	EA=1;
	ES=1; 			 
 	PCON=0x00;		 
	SM0=0;			
	SM1=1;
	REN=1;		
	TI=0;		
	TMOD=0x20;	
	TH1=0xfd;	
	TL1=0xfd;	
	TR1=1;			 

	recall_fun=in;
}

void port_send(char send_data) {
	SBUF=send_data;		  
	while(!TI);
	TI=0;
	delay_port_send();	   
}

void RecvInterrupt(void) interrupt 4 {
	if (RI==0) return;
	recall_fun(SBUF);
	if (RI==1) RI=0;  
}

void interrupt_trance_on(void) {
	ES=1;
}

void interrupt_trance_off(void) {
	ES=0;
}
