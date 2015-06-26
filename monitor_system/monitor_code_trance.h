

#ifndef _MONITOR_CODE_TRANCE_H__
#define _MONITOR_CODE_TRANCE_H__

#include <stdio.h>

#include <windows.h>
#include <winspool.h>

#define LEN_NAME_PORT 6

typedef char Name_Port[LEN_NAME_PORT];

enum err_code {
    PORT_RECV_ERR=-1,
};

const int  len_buffer_read  =64;
const int  max_port_list_max=32;

class PORT_OPEN_ERR{
 public:
  PORT_OPEN_ERR() {};
  ~PORT_OPEN_ERR(){};
};

int get_port(char *&get_port_name_list) {
	int num_port=0;
	char *name_list=(char *)malloc(LEN_NAME_PORT*max_port_list_max+1);
	memset(name_list,0,sizeof(name_list));
	
    COMMCONFIG get={0};
    DWORD get_size=sizeof(get);

	for (int i=0;i<max_port_list_max;++i) {
		char port_name[LEN_NAME_PORT]={0};
		sprintf(port_name,"%s%d","COM",i);
		if (GetDefaultCommConfig((LPCTSTR)port_name,&get,&get_size)) {
			memcpy(name_list+num_port*LEN_NAME_PORT,port_name,LEN_NAME_PORT);
			++num_port;
		}
	}

	get_port_name_list=name_list;
	return num_port;
}

inline void free_name_list(char* in) {
	free((void*)in);
}

inline void free_recv_buffer(char* in) {
	free((void*)in);
}

class Port {

 public:
  Port(void) {};
  Port(Name_Port open_port_name) {
      port_handle=CreateFile((LPCTSTR)open_port_name,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

      if (INVALID_HANDLE_VALUE==port_handle)
          throw PORT_OPEN_ERR();
  }

  ~Port() {
      CloseHandle(port_handle);
      port_handle=INVALID_HANDLE_VALUE;
  }

  long read(char*& recv_data_point) const {
	  char *read_data=(char *)malloc(len_buffer_read+1);
  	  memset(read_data,0,len_buffer_read+1);
      long size_read=0;

	  if (ReadFile(port_handle,(LPVOID)recv_data_point,len_buffer_read,(LPDWORD)&size_read,NULL)) {
		  recv_data_point=read_data;
		  return size_read;
      }

      free_recv_buffer(read_data);
	  recv_data_point=NULL;
  	  return PORT_RECV_ERR;
  }

  template <class Type>
  bool send(Type* send_data_point,int send_data_size) const {
      DWORD  size_write=0;
      return WriteFile(port_handle,(LPCVOID)send_data_point,send_data_size,&size_write,NULL)?true:false;
  }

 private:
  HANDLE port_handle;
};

#endif