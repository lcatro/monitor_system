
#ifndef _MONITOR_SYSTEM_CODE_TRANCE_H__
#define _MONITOR_SYSTEM_CODE_TRANCE_H__

#include "monitor_system_trance_data.h"

typedef void   (*recall_recv_packet_address)(void);

void 			 trance_init(recall_recv_packet_address recall_fun_address);
void 			 send_reqution_packet(Data_Code reqution_code,char *additional_data,int len_additional_data);
Data_Trance_Code recv_reqution_packet(char *out_additional_data,Data_Code *out_reqution_code);

#endif