// monitor_system.cpp : Defines the entry point for the application.
//

#include "monitor_code_trance.h"
#include "monitor_code_memory.h"
#include "monitor_code_cpu.h"

#include "monitor_system_trance_data.h"

const int wait_time=3000;
const int messagebox_flag_critical=16;

Name_Port port_name="COM8";

static inline change_password(Port& port,const char* new_password,int password_size) {
    Data_Trance port_send_data;
	memset(&port_send_data,0,sizeof(port_send_data));

	port_send_data.code_trance=DATA_ACK;
    port_send_data.code_data=Set_Password;
    memcpy(&port_send_data.data,new_password,password_size);

    port.send(&port_send_data,sizeof(port_send_data));
}

static inline trance_userate_cpu(Port& port,CPUUseRate& monitor_cpu) {
    int number_buffer=(short)monitor_cpu.GetCPUUseRate();
    number_buffer<<=8;

    Data_Trance port_send_data;
	memset(&port_send_data,0,sizeof(port_send_data));

	port_send_data.code_trance=DATA_ACK;
    port_send_data.code_data=Get_Data_CPU_UseRate;
    memcpy(&port_send_data.data,(const void *)&number_buffer,sizeof(short));

    port.send(&port_send_data,sizeof(port_send_data));
}

static inline trance_userate_memory(Port& port,MemoryData& monitor_memory) {
    monitor_memory.refresh();
    int number_buffer=(short)monitor_memory.get_memorydata_userate();
    number_buffer<<=8;

    Data_Trance port_send_data;
	memset(&port_send_data,0,sizeof(port_send_data));

	port_send_data.code_trance=DATA_ACK;
    port_send_data.code_data=Get_Data_Memory_UseRate;
    memcpy(&port_send_data.data,(const void *)&number_buffer,sizeof(short));

    port.send(&port_send_data,sizeof(port_send_data));
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

    try {
        Port        port(port_name);
        short       trance_code=Get_Data_CPU_UseRate;

        CPUUseRate  monitor_cpu;
        MemoryData  monitor_memory;

        while (true) {
			if (Get_Data_CPU_UseRate==trance_code) {
                trance_userate_cpu(port,monitor_cpu);
                trance_code=Get_Data_Memory_UseRate;
			} else if (Get_Data_Memory_UseRate==trance_code) {
                trance_userate_memory(port,monitor_memory);
                trance_code=Get_Data_CPU_UseRate;
			}
            Sleep(wait_time);
	    }

    } catch (PORT_OPEN_ERR) {
        MessageBox(NULL,"port open err!","monitor_system ERR",messagebox_flag_critical);
    } catch (...) {
        MessageBox(NULL,"unkonw err!","monitor_system ERR",messagebox_flag_critical);
    }

	return 0;
}
