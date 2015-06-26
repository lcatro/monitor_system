
#ifndef _MONITOR_CODE_CPU_H__
#define _MONITOR_CODE_CPU_H__

#include <conio.h>
#include <memory.h>

#include <windows.h>
#include <winbase.h>

#define STR_DLL_KERNEL_FILE_NAME "kernel32.dll"
#define LEN_STR_DLL_KERNEL_FILE_NAME strlen(STR_DLL_KERNEL_FILE_NAME)

#define LEN_STR_FILE_PATH_SYSTEM 256

const int wait_for_init=1000;

typedef bool (__stdcall * point_function_GetSystemTimes) (
							LPFILETIME lpIdleTime,
							LPFILETIME lpKernelTime,
							LPFILETIME lpUserTime);

HANDLE						  handle_kernel_dll=INVALID_HANDLE_VALUE;
point_function_GetSystemTimes GetSystemTimes=NULL;

static inline char *path_kernel(void) {
	char *path_system[LEN_STR_FILE_PATH_SYSTEM]={0};
	int len_path_system=(int)GetSystemDirectory((char *)path_system,LEN_STR_FILE_PATH_SYSTEM);

	char *rtn_path_kernel=(char *)malloc(len_path_system+LEN_STR_DLL_KERNEL_FILE_NAME+2);
	memset(rtn_path_kernel,0,len_path_system+LEN_STR_DLL_KERNEL_FILE_NAME+2);

	strcpy(rtn_path_kernel,(const char *)path_system);
	strcat(rtn_path_kernel,"\\");
	strcat(rtn_path_kernel,STR_DLL_KERNEL_FILE_NAME);

	return rtn_path_kernel;
}

static inline void kernel_dll_load(void) {
	handle_kernel_dll=(HANDLE)LoadLibrary(path_kernel());
	if (INVALID_HANDLE_VALUE!=handle_kernel_dll)
		GetSystemTimes=(point_function_GetSystemTimes)GetProcAddress((HMODULE)handle_kernel_dll,"GetSystemTimes");
}

static inline void kernel_dll_clean(void) {
	GetSystemTimes=NULL;
	FreeLibrary((HMODULE)handle_kernel_dll);
	handle_kernel_dll=INVALID_HANDLE_VALUE;
}

class CPUUseRate
{
public:
	CPUUseRate(void) {
		kernel_dll_load();

		FILETIME ftIdle, ftKernel, ftUser;
		BOOL flag = FALSE;
		if (flag = GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
		{
			m_fOldCPUIdleTime = FileTimeToDouble(ftIdle);
			m_fOldCPUKernelTime = FileTimeToDouble(ftKernel);
			m_fOldCPUUserTime = FileTimeToDouble(ftUser);

		}
		Sleep(wait_for_init);//等待1秒再调用GetCPUUseRate()
	}

	~CPUUseRate(void) {
		kernel_dll_clean();
	}

	int GetCPUUseRate()
	{
		int nCPUUseRate = -1;
		FILETIME ftIdle, ftKernel, ftUser;
		if (GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
		{
			double fCPUIdleTime = FileTimeToDouble(ftIdle);
			double fCPUKernelTime = FileTimeToDouble(ftKernel);
			double fCPUUserTime = FileTimeToDouble(ftUser);
			nCPUUseRate= (int)(100.0 - (fCPUIdleTime - m_fOldCPUIdleTime) 
				/ (fCPUKernelTime - m_fOldCPUKernelTime + fCPUUserTime - m_fOldCPUUserTime) 
				*100.0);
			m_fOldCPUIdleTime = fCPUIdleTime;
			m_fOldCPUKernelTime = fCPUKernelTime;
			m_fOldCPUUserTime = fCPUUserTime;
		}
		return nCPUUseRate;
	}
private:
	double FileTimeToDouble(FILETIME &filetime)
	{
		return (double)(filetime.dwHighDateTime * 4.294967296E9) + (double)filetime.dwLowDateTime;
	}
private:
	double m_fOldCPUIdleTime;
	double m_fOldCPUKernelTime;
	double m_fOldCPUUserTime;
};


inline DWORD get_current_num_process(void) {
	SYSTEM_INFO info;
	memset(&info,0,sizeof(info));
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}
/*
inline int get_current_num_cpu_userate(void) {
	return CPUUseRate.GetCPUUseRate();
}
*/
#endif