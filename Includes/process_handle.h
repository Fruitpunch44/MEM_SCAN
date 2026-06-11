#ifndef PROC_HANDLE_H
#define PROC_HANDLE_H
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<winternl.h>
#define BUFF_SIZE 1024

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
typedef enum{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR
}value_type;

//unions to store the probable data type
typedef struct {
    value_type type;
    union {
        unsigned char  byte_value;
        unsigned int   int_value;
        float          float_value;
        double         double_value;
        unsigned long long ull_value;
        }VALUE_UNION;
}VALUE_TYPE;

typedef struct{
    DWORD pid;
    VALUE_TYPE Target;
    HWND  hwnd_test;
}thread_params;

typedef struct{
    HANDLE proc;
}global_process_handle;


size_t get_type_size(value_type type);

int read_memory(HANDLE proc,unsigned long long addr,void *buff,size_t size);
void scan_memory(DWORD proc_id,VALUE_TYPE *target);
//void get_process_id(DWORD proc_id,unsigned long long start);
DWORD WINAPI scan_thread(LPVOID lpParam);
void  write_to_address(unsigned long long address,void *value,size_t value_size);
int write_memomry(HANDLE proc,void *value,size_t value_size,unsigned long long addr);
void NT_Error_Message(NTSTATUS status,unsigned long long addr);



#endif

/*so this is going over working with undocumented api functions opposed to the normal api call 
do note that they do both the same thing differnce being that the nt version isn't a wraper and you have to declare it manually */