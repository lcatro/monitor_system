
#include "device_delay.h"

//  在12MHz 下的精确延时
void delay_50us(unsigned int time_delay_count) {
	unsigned char j=19;  
	for(;time_delay_count>0;time_delay_count--)   
		for(;j>0;j--);
}