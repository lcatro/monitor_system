
#ifndef _DEVICE_LCD_1602_H__
#define _DEVICE_LCD_1602_H__

typedef void (*recall_recv)(int recv_data);

void port_init(recall_recv in);
void port_send(char send_data);

void interrupt_trance_on(void) ;
void interrupt_trance_off(void);

#endif