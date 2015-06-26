

#ifndef _MONITOR_SYSTEM_TRANCE_DATA_H__
#define _MONITOR_SYSTEM_TRANCE_DATA_H__

#define LEN_TRANCE_DATA 18
#define LEN_TRANCE_DATA_ADDITIONAL 16 

#define Get_Run 0
#define Get_Password 1
#define Set_Password 2
#define Get_Data_CPU_UseRate 10
#define Get_Data_Memory_Size 11
#define Get_Data_Memory_UseRate 12

#define DATA_REQ 1
#define DATA_ACK 2

typedef struct {
	char code_data;
	char code_trance;
	char data[LEN_TRANCE_DATA_ADDITIONAL];
} Data_Trance;

#endif