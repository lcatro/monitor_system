

#include <string.h>

#include "monitor_system_chip.h"
#include "monitor_system_code_trance.h"

char recv_buffer[LEN_TRANCE_DATA]={0};
int  recv_length=0;
bool recv_succed=false;
recall_recv_packet_address recall_address=null;

static void reset_recv_buffer(void) {
	memset(recv_buffer,0,LEN_TRANCE_DATA);
	recv_length=0;
	recv_succed=false;
}

static void recall_recv_data(int recv_data) {
	if (recv_length<LEN_TRANCE_DATA && !recv_succed) {
		recv_buffer[recv_length]=recv_data;
		++recv_length;
		if (recv_length>=LEN_TRANCE_DATA) {
			recv_succed=true;
			recall_address();
		}
		return;
	}
	reset_recv_buffer();
}

void trance_init(recall_recv_packet_address in_recall_address) {
	port_init(&recall_recv_data);
	recall_address=in_recall_address;
}

void send_reqution_packet(Data_Code reqution_code,char *additional_data,int len_additional_data) {
	int i=0;
	char *send_buffer=null;
	Data_Trance send_packet;
	memset(&send_packet,0,LEN_TRANCE_DATA);

	switch (reqution_code) {
		case Get_Data_CPU_UseRate:
			send_packet.code_data=reqution_code;					  
			break;
		case Get_Data_Memory_Size:
			send_packet.code_data=reqution_code;
			break;
		case Get_Data_Memory_UseRate:  
			send_packet.code_data=reqution_code;
			break;
		default:
			return;
	}
	send_packet.code_trance=DATA_REQ;
	if (null!=additional_data)
		memcpy(&send_packet.data_additional,additional_data,len_additional_data);

	for	(send_buffer=(char *)&send_packet;i<LEN_TRANCE_DATA;++i)
		port_send(*(send_buffer+i));
}
//  TIPS!What this string copy will not take null-terminated.. 
Data_Trance_Code recv_reqution_packet(char *out_additional_data,Data_Code *out_reqution_code) {
	Data_Trance_Code  rtn        =DATA_ERR;

	if (recv_succed) {																								   
		memcpy(out_reqution_code,recv_buffer,LEN_ENUM);																   
		memcpy(&rtn,recv_buffer+LEN_ENUM,LEN_ENUM);
		if (Get_Password==rtn || Set_Password==rtn) {
			memcpy(out_additional_data,recv_buffer+(LEN_TRANCE_DATA-LEN_TRANCE_DATA_ADDITIONAL),LEN_TRANCE_DATA_ADDITIONAL);
		} else {
			// TIPS!The number data which receive for computer USB-port is low to height ..
			// TIPS!sizeof(int)=2 is not 4 ..
			*out_additional_data =*(recv_buffer+LEN_ENUM*2);
		}
		reset_recv_buffer();
	}

	return rtn;
}								






