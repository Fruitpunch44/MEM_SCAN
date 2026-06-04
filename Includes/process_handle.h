#ifndef PROC_HANDLE_H
#define PROC_HANDLE_H
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<winternl.h>
#define BUFF_SIZE 1024
#include "process_array.h"

typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation
}MEMORY_INFORMATION_CLASS;

typedef NTSTATUS(NTAPI *pZwReadVirtualMemory)(
	  HANDLE  hProcess,
	  LPCVOID lpBaseAddress,
	  LPVOID  lpBuffer,
	  SIZE_T  nSize,
	  SIZE_T *lpNumberOfBytesRead
);

typedef NTSTATUS(NTAPI *pZwWriteVirtualMemory)( 
    HANDLE hProcess,
    LPCVOID BaseAddress,
    LPVOID Buffer,
    SIZE_T nSize,
    SIZE_T *NumberOfBytesWritten
);

typedef NTSTATUS(NTAPI *pNtQueryVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    MEMORY_INFORMATION_CLASS MemoryInformationClass,
    PVOID MemoryInformation,
    SIZE_T MemoryInformationLength,
    PSIZE_T ReturnLength
);

//void string_search(char *string_to_find);

typedef struct{
    DWORD pid;
    DWORD Target;
    HWND  hwnd_test;
}thread_params;

typedef struct{
    HANDLE proc;
}global_process_handle;

unsigned int read_memory(HANDLE proc,unsigned long long addr);
void scan_memory(DWORD proc_id,DWORD target);
//void get_process_id(DWORD proc_id,unsigned long long start);
void compare_changes(DWORD proc_id,address_arr *arr);
DWORD WINAPI scan_thread(LPVOID lpParam);
void write_to_address(unsigned long long address,unsigned int value);
unsigned int write_memomry(HANDLE proc,UINT value,unsigned long long addr);
void NT_Error_Message(NTSTATUS status,unsigned long long addr);



#endif

/*so this is going over working with undocumented api functions opposed to the normal api call 
do note that they do both the same thing differnce being that the nt version isn't a wraper and you have to declare it manually */