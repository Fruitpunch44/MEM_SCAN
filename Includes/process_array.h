#ifndef PROC_ARR_H
#define PROC_ARR_H

#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<windows.h>
#include "process_handle.h"

typedef struct{
    CHAR *NAME;
    DWORD PID;
    DWORD Thread;
}process_info;


typedef struct{
    unsigned long long addr;
    VALUE_TYPE value;
    VALUE_TYPE previous;
}address_info;


typedef struct{
    unsigned long long addr;
    VALUE_TYPE value;
    VALUE_TYPE previous;
}filtered_adderess_info;

typedef struct{
    unsigned long long addr;
    char *found_string;
}string_address_info;


typedef struct{
    process_info *entries;
    size_t capacity;
    size_t count;
}process_arr;


typedef struct {
    address_info *info;
    size_t capacity;
    size_t count;
}address_arr;

typedef struct {
    filtered_adderess_info *info;
    size_t capacity;
    size_t count;
}filtered_address_arr;

typedef struct {
    string_address_info *info;
    size_t capacity;
    size_t count;
}string_address_arr;

extern address_arr global_address_info;
extern process_arr global_process;
extern filtered_address_arr global_filtered_info;
extern string_address_arr global_string_info;

process_arr init_array();
void add_array(process_arr *array_list,process_info *entries);
address_arr init_addr_array();
void add_array_address_info(address_arr *arr,address_info *info);
void add_array_address_info_filter(filtered_address_arr *arr,filtered_adderess_info *info);
filtered_address_arr init_filtered_addr_array();
void free_address_array(address_arr *arr);
void free_filtered_address_array(filtered_address_arr *arr);
void compare_changes(DWORD proc_id,address_arr *arr);
#endif


