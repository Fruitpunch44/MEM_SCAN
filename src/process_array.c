#include "process_array.h"
//REFACTOR LATER THERE IS ALOT OF CODE RESUSE
process_arr init_array(){
    process_arr arr_ptr;
    arr_ptr.capacity=5;
    arr_ptr.count=0;
    arr_ptr.entries=malloc(arr_ptr.capacity *sizeof(process_info));
    return arr_ptr;
}

void add_array(process_arr *arr,process_info *info){
    size_t new_cap;
    if(arr->count == arr->capacity){
       new_cap = arr->capacity*=2;
        process_info *temp;
        temp=realloc(arr->entries,arr->capacity *sizeof(process_info));
        if(!temp){
            fprintf(stderr,"realloc failed");
            return;
        }
        arr->entries=temp;
        arr->capacity = new_cap;
    }
    arr->entries[arr->count++]= *info;
}

address_arr init_addr_array(){
    address_arr info_ptr;
    info_ptr.capacity =5 ;
    info_ptr.count =0;
    info_ptr.info = malloc(info_ptr.capacity*sizeof(address_info));
    return info_ptr;
}

filtered_address_arr init_filtered_addr_array(){
    filtered_address_arr info_ptr;
    info_ptr.capacity =5 ;
    info_ptr.count =0;
    info_ptr.info = malloc(info_ptr.capacity*sizeof(filtered_adderess_info));
    return info_ptr;
}

void add_array_address_info(address_arr *arr,address_info *info){
    size_t new_cap;
    if(arr->count == arr->capacity){
       new_cap = arr->capacity*=2;
       if(arr->capacity == 0){
        new_cap = arr->capacity = 10;
       }
        address_info *temp;
        temp=realloc(arr->info,arr->capacity *sizeof(address_info));
        if(!temp){
            fprintf(stderr,"realloc failed");
            return;
        }
        arr->info=temp;
        arr->capacity =new_cap;
    }
    arr->info[arr->count++]=*info;
}

void add_array_address_info_filter(filtered_address_arr *arr,filtered_adderess_info *info){
    size_t new_cap;
    if(arr->count == arr->capacity){
       new_cap = arr->capacity*=2;
       if(arr->capacity == 0){
        new_cap = arr->capacity = 10;
       }
        filtered_adderess_info *temp;
        temp=realloc(arr->info,arr->capacity *sizeof(filtered_adderess_info));
        if(!temp){
            fprintf(stderr,"realloc failed");
            return;
        }
        arr->info=temp;
        arr->capacity =new_cap;
    }
    arr->info[arr->count++]=*info;
}

void free_address_array(address_arr *arr){
    free(arr->info);
    arr->info=NULL;
    arr->capacity=0;
    arr->count=0;
}

void free_filtered_address_array(filtered_address_arr *arr){
    free(arr->info);
    arr->info=NULL;
    arr->capacity=0;
    arr->count=0;
}
