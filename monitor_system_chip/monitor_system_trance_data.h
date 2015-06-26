

#ifndef _MONITOR_SYSTEM_TRANCE_DATA_H__
#define _MONITOR_SYSTEM_TRANCE_DATA_H__

#define LEN_TRANCE_DATA 		   18  //  TIPS!Size of enum is 1-byte at 89S52!..
#define LEN_TRANCE_DATA_ADDITIONAL 16 
#define LEN_ENUM 					1
#define LEN_INT			   sizeof(int)
#define LEN_BIT						1

typedef enum {
	Get_Run=0,
	Get_Password,
	Set_Password,
	Get_Data_CPU_UseRate=10,
	Get_Data_Memory_Size,
	Get_Data_Memory_UseRate,
} Data_Code;

typedef enum {
	DATA_ERR=0,
	DATA_REQ,
	DATA_ACK,
} Data_Trance_Code;

typedef struct {
	Data_Code		 code_data;
	Data_Trance_Code code_trance;
	char data_additional[LEN_TRANCE_DATA_ADDITIONAL];
} Data_Trance;

#endif