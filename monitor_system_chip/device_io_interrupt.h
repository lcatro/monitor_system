
#ifndef _DEVICE_IO_INTERRUPT_H__
#define _DEVICE_IO_INTERRUPT_H__
					  
#define PORT_INTERRUPT_0 0
#define PORT_INTERRUPT_1 1

typedef enum {
	Interrupt_Stop=0,
	Interrupt_Run,
} Interrupt_Stat_Code;

typedef void (*recall_io_interrupt)(int io_port_num,int io_port_stat);

void io_interrupt_init(int io_port,int io_interrupt_select,recall_io_interrupt recall_fun_address);

void io_interrupt_on(void);
void io_interrupt_off(void);
void set_interrupt_0(Interrupt_Stat_Code stat_code);
void set_interrupt_1(Interrupt_Stat_Code stat_code);
void set_io_interrupt_select_0(int io_interrupt_select);
void set_io_interrupt_select_1(int io_interrupt_select);

#endif