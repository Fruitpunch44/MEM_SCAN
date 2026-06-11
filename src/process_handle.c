#include "process_handle.h"
#include "process_array.h"



// void write_memory(HANDLE process, BYTE *address,BYTE value);
//add range scan dumb comment
address_arr global_address_info;
filtered_address_arr global_filtered_info;
global_process_handle global_proc;
//if void can handle various types i don't know;

void NT_Error_Message(NTSTATUS status,unsigned long long addr){
        char err_mess[50];
        char err_code[20];
        DWORD err= GetLastError();
        snprintf(err_mess,sizeof(err_mess),"error in writing to 0%p", (LPCVOID)addr);
        snprintf(err_code,sizeof(err_code),"%d",err);
        MessageBox(NULL,err_code,"error code",MB_OK);
        MessageBox(NULL,err_mess,"error",MB_OK);
}
size_t get_type_size(value_type type){
    switch(type){
        case TYPE_INT: return sizeof(int);
        case TYPE_FLOAT: return sizeof(float);
        case TYPE_DOUBLE: return sizeof(double);
        case TYPE_CHAR: return sizeof(char);
        default: return 0;
    }
}
int write_memomry(HANDLE proc,void *value,size_t value_size,unsigned long long addr){
    size_t number_of_bytes_to_written;
    HMODULE ntdll = GetModuleHandle("ntdll.dll");
    pZwWriteVirtualMemory ZwWriteVirtualMemory = (pZwWriteVirtualMemory)GetProcAddress(ntdll,"ZwWriteVirtualMemory");
    if(!ZwWriteVirtualMemory){
        MessageBox(NULL,"unable to get address of ZwReadVirtualMemory","error",MB_OK);
        return 0;
    }

    NTSTATUS status = ZwWriteVirtualMemory(proc,(LPCVOID)addr,value,value_size,&number_of_bytes_to_written);

    if(NT_ERROR(status)){
        NT_Error_Message(status,addr);
        return 0;
    }
    return 1;
}


int read_memory(HANDLE proc,unsigned long long addr,void *buff,size_t target_size){
    //you technically can read out all
    SIZE_T number_of_bytes_read;
    HMODULE ntdll = GetModuleHandle("ntdll.dll");
    pZwReadVirtualMemory ZwReadVirtualMemory = (pZwReadVirtualMemory)GetProcAddress(ntdll,"ZwReadVirtualMemory");
    if(!ZwReadVirtualMemory){
        MessageBox(NULL,"unable to get address of ZwReadVirtualMemory","error",MB_OK);
        return 0;
    }

    NTSTATUS status =ZwReadVirtualMemory(proc,(LPCVOID)addr,buff,target_size,&number_of_bytes_read);
    if(NT_ERROR(status)){
        NT_Error_Message(status,addr);
        return 0;
    }
    return 1;
}

DWORD WINAPI scan_thread(LPVOID lpParam){
    //cast the struct pointer to lparam 
    thread_params *params = (thread_params*)lpParam;
    scan_memory(params->pid,&params->Target);
    PostMessage(params->hwnd_test,WM_USER+4,0,0);//POST TO WM_SCAN_THREAD_FINISHED;

    char dbg[100];
    snprintf(dbg, sizeof(dbg), "Posting WM_SCAN_THREAD_FINISHED to: %p", params->hwnd_test);//debugging
    MessageBox(NULL, dbg, "DEBUG POST", MB_OK);//debugging
    free(params);
    return 0;
}

void scan_memory(DWORD proc_id,VALUE_TYPE *target){
    free_address_array(&global_address_info);
    global_address_info = init_addr_array();
    MEMORY_BASIC_INFORMATION mbi ={0};
    unsigned long long base_addr = 0;
    global_proc.proc= OpenProcess(PROCESS_QUERY_INFORMATION |PROCESS_VM_READ|
                                PROCESS_VM_WRITE|PROCESS_VM_OPERATION,FALSE,proc_id);

     if(global_proc.proc== NULL){
        fprintf(stderr,"unable to open process \n pls pass in a valid pid %s",strerror(GetLastError()));
        return;
    }
    size_t type_size = get_type_size(target->type);
    char DEBUG[64];
    snprintf(DEBUG,64,"current size type %llu",type_size);
    MessageBox(NULL,DEBUG,"TEST",MB_OK);
    
    //defining undocumented win calls
    HMODULE ntdll = GetModuleHandle("ntdll.dll");
    pNtQueryVirtualMemory NtQueryVirtualMemory = (pNtQueryVirtualMemory)GetProcAddress(ntdll,"NtQueryVirtualMemory");
    if(!NtQueryVirtualMemory){
        MessageBox(NULL,"unable to get address of NtQueryVirtualMemory","error",MB_OK);
        CloseHandle(global_proc.proc);
        return;
    }
    NTSTATUS status = NtQueryVirtualMemory(global_proc.proc,(LPVOID)base_addr,MemoryBasicInformation,&mbi,sizeof(mbi),0);

    void *buff = malloc(type_size);
    if(!buff){
        MessageBox(NULL,"error in mem alloc","error",MB_ICONERROR);
        CloseHandle(global_proc.proc);
        return;
    }

    while(NT_SUCCESS(status)){
        // memory filter 
        if(mbi.State == MEM_COMMIT && 
            ((mbi.Protect & PAGE_READONLY)|| (mbi.Protect & PAGE_READWRITE) ||(mbi.Protect & PAGE_EXECUTE_READ) 
            ||(mbi.Protect & PAGE_EXECUTE_READWRITE)) &&
            !(mbi.Protect & PAGE_NOACCESS) &&
            !(mbi.Protect & PAGE_GUARD)){
                unsigned long long start = (unsigned long long)(void*)mbi.BaseAddress;
                unsigned long long end = start + mbi.RegionSize;
                //fprintf(stdout,"Memory region at 0x%p is committed and accessible.\n", (void*)mbi.BaseAddress);
                
                while(start < end){
                    if(read_memory(global_proc.proc,start,buff,type_size)){
                        if(memcmp(buff,&target->VALUE_UNION,type_size)==0){
                        address_info *info = malloc(sizeof(address_info));
                        if(!info){
                            //fprintf(stderr,"error in mem alloc");
                            CloseHandle(global_proc.proc);
                            return;
                        }
                        info->addr = start;
                        info->value = *target;
                        info->previous = *target;
                        add_array_address_info(&global_address_info, info);
                    }
                }
                start += type_size; //increment by the size of the value to read 
            }
        }
            base_addr += mbi.RegionSize;//move to the next region
            status = NtQueryVirtualMemory(global_proc.proc, (LPVOID)base_addr, MemoryBasicInformation, &mbi, sizeof(mbi), NULL);
    }
    free(buff);
}

void compare_changes(DWORD proc_id,address_arr *arr){
    free_filtered_address_array(&global_filtered_info);
    global_filtered_info = init_filtered_addr_array();
    for(int i = 0 ;i< arr->count;i++){
        VALUE_TYPE re_read_value;
        re_read_value.type = arr->info[i].value.type;
        if(read_memory(global_proc.proc,arr->info[i].addr,&re_read_value.VALUE_UNION,get_type_size(arr->info[i].value.type))){
            if(memcmp(&re_read_value.VALUE_UNION,&arr->info[i].value.VALUE_UNION,get_type_size(arr->info[i].value.type)) != 0){
                filtered_adderess_info *filtered_info = malloc(sizeof(filtered_adderess_info));
                if(!filtered_info){
                    MessageBox(NULL,"error in mem alloc","error",MB_ICONERROR);
                    return;
                }
                filtered_info->addr = arr->info[i].addr;
                filtered_info->value = re_read_value;
                filtered_info->previous = arr->info[i].value;
                add_array_address_info_filter(&global_filtered_info,filtered_info);
            }
        }
    }
}

void  write_to_address(unsigned long long address,void *value,size_t value_size){
    write_memomry(global_proc.proc,value,value_size,address);
}
/*there is something to consider reagding this and how can i effectively filter out the
process after a 2nd scan i do not know how to go about that for now what happens is that you initially 
search for value and it pops out the various address associated with that value that's all fine and good 
but the issue now is how can i use next scan to now filter out the values so only that said 
values will be shown that is the main issue rn and i have to try to figure it out 
do i need to scan again maybe i will figure it out but that might cause issues*/
//scan from 0 to max
/*
LEAVE THIS OUT FOR NOW 
void get_process_id(DWORD proc_id,unsigned long long start){
    unsigned long long end =0xffffffffffffffff;
    scan_memory(proc_id,start,end,);
}
    */