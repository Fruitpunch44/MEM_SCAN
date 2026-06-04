#include<windows.h>
#include<stdlib.h>
#include<commctrl.h>
#include <tlhelp32.h>
#pragma comment(lib, "comctl32.lib")
#include"process_array.h"
#include"process_handle.h"
#include"proclist.h"

//handle the type gotten
/*
doesn't work for shit sadly well damn 
#define Type(x) _Generic((x), \
    int: "int", \
    long: "long", \
    float: "float", \
    unsigned int: "unsigned int", \
    unsigned long long: "unsigned long long", \
    unsigned long: "unsigned long", \
    unsigned float: "unsigned float", \
    default: "unknown")
*/

HWND CREATE_LIST(HWND PARENT,process_arr *array);
HWND CREATE_BOTTOM_LIST(HWND PARENT);
HWND CREATE_SIDE_OPTIONS(HWND Parent);
HWND CREATE_GROUP_BOX(HWND Parent);
HWND CREATE_LEFT_SIDE_Table(HWND Parent,address_arr *addr_arr);
HWND CREATE_SCAN(HWND Parent);
void handle_address_range();
void refresh_left_table(HWND Parent);
void get_target_value();
void refresh_left_table_filter(HWND hwnd);
HWND show_selected_process(HWND Parent);
HWND CREATE_COMBO_BOX(HWND Parent);
HWND NEXT_SCAN(HWND Parent);
HWND create_popup(HWND parent);
HWND create_status_bar(HWND Parent);
void CREATE_CHECK_BOX(int pos);

typedef struct{
    HWND hList;//global handle for the process list to be able to
    HWND hlist_left_table;//global handle for list in main window
    HWND group_box;//global handle for group box
    HWND info;//handle for the little process id and name display above the address list
    HWND h_options;//handle for passing value to scan for
    HWND my_drop_down; //handle for the drop down selection
    HWND address_form;//handle for address field in the popup window for writting to an adddress
    HWND bar; //handle for status bar
    HWND hlist_bottom; //handle for the bottom list
    int pid;//make pid global
    unsigned int value;//value gotten from the input text field
    unsigned int form_value;//value gotten from the address form 
    unsigned long long write_address;//selected address from the address field u want to write to
    char info_buff[200];// process id and name
    char selection[128];
}global_window_states;

extern global_window_states gwin;


//objects within the window
#define IDR_MYMENU          101 //MY MENU
#define WM_REFRESH          (WM_USER+1) // handle this for refreshing the list after a scan
#define WM_WRITE_VAL        (WM_USER+3)// handle for writing values
#define WM_SCAN_THREAD_FINISHED (WM_USER+4) //handle for when the scan thread is finished so we can refresh the list
#define ID_FILE_EXIT        60001//pop up menu exit
#define ID_HELP_ABOUT       60002//pop up menu help
#define ID_OPEN_PROCESS     60003//popup menu open process
#define ID_SELECT           60004 //BUTTON
#define ID_EDIT_VALUE       60005 //EDIT FIELS    
#define ID_START_EDIT       60006 //START FIELD
#define ID_STOP_EDIT        60007 //STOP FIELD
#define ID_WRITEABLE_CHECK  60008 //CHECKBOX
#define ID_GROUP_BOX        60009 //
#define ID_SCAN_BUTTON      60010 //FIRST SCAN BUTTON
#define ID_NEXT_SCAN_BUTTON 60011 //NEXT SCAN BUTTON
#define ID_COMBO_BOX_DROP   60012 //DROP BOX OPTIONS
#define ID_POPUP_WRITE      60013 //WRITE IN POPUP WIN
#define ID_POPUP_CANCEL     60014 //CANCEL BUTTON IN POPUP WIN
#define ID_POPUP_FORM       60015 //INPUT FORM IN POPUP WIN
#define ID_STATUS_BAR       60016//STATUS BAR
#define ID_LEFT_TABLE       60017//LEFT TABLE
#define ID_BOTTOM_TABLE     60018//BOTTOM TABLE

