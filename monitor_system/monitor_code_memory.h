

#ifndef _MONITOR_CODE_MEMORY_H__
#define _MONITOR_CODE_MEMORY_H__

#include <windows.h>

#define LEN_LONG 4

typedef struct {
	int  MemoryUseRate;
	long MemoryTotalSize;
	long MemoryUseSize;
	long MemoryAvailSize;
}memory_data;

class MemoryData {

 public:
  MemoryData(void) {memset(&data,0,sizeof(data));};
  ~MemoryData() {};

  void refresh(void) {
    MEMORYSTATUS get_data;
	memset(&get_data,0,sizeof(get_data));
	GlobalMemoryStatus(&get_data);

	data.MemoryUseRate=get_data.dwMemoryLoad;
	data.MemoryTotalSize=get_data.dwTotalPhys;
	data.MemoryAvailSize=get_data.dwAvailPhys;
	data.MemoryUseSize=get_data.dwTotalPhys-get_data.dwAvailPhys;
  };

  int  get_memorydata_userate   (void) const {
      return data.MemoryUseRate;
  };

  long get_memorydata_totalsize (void) const {
      return data.MemoryTotalSize;
  };

  long get_memorydata_usesize   (void) const {
      return data.MemoryUseSize;
  };

  long get_memorydata_availsize (void) const {
      return data.MemoryAvailSize;
  };

 private:
  memory_data data;
};
/*
inline memory_data *get_current_size_memory(void) {
	MEMORYSTATUS get_data;
	memset(&get_data,0,sizeof(get_data));
	GlobalMemoryStatus(&get_data);

	memory_data *rtn=(memory_data *)malloc(LEN_MEMORY_DATA);
	memset(rtn,0,sizeof(rtn));

	rtn->MemoryUseRate=get_data.dwMemoryLoad;

	rtn->MemoryTotalSize=get_data.dwTotalPhys;
	rtn->MemoryAvailSize=get_data.dwAvailPhys;
	rtn->MemoryUseSize=get_data.dwTotalPhys-get_data.dwAvailPhys;

	return rtn;
}*/

#endif