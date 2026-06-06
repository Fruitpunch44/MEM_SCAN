#include "the_windows.h"
/*todo
add string search function
use wm_notify later
add better filtering
add write modifications
add status bar 
add threading to prevent ui blocking
*/ 
global_window_states gwin;

LRESULT CALLBACK PopupProc(HWND hwnd,UINT msg, WPARAM wparam, LPARAM lparam){
    switch(msg){
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_COMMAND:
            switch(LOWORD(wparam)){
                case ID_POPUP_WRITE:
                    char buff[90];
                    char test[220];
                    GetWindowText(gwin.address_form,buff,sizeof(buff));
                    gwin.form_value= strtoul(buff,NULL,10);
                    snprintf(test,sizeof(test),"this is a test val:%u",gwin.form_value);
                    MessageBox(NULL,test,"TEST",MB_OK);
                    char dbg[100];
                    snprintf(dbg, sizeof(dbg), "Posting WM_WRITE_VAL to: %p", GetParent(hwnd));//debugging
                    MessageBox(NULL,dbg,"dbug",MB_OK);
                    PostMessage(GetParent(hwnd),WM_WRITE_VAL,0,0);   
                    DestroyWindow(hwnd);
                    break;
                case ID_POPUP_CANCEL:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        default:
            return DefWindowProc(hwnd,msg,wparam,lparam);
    }
    return 0;
}

LRESULT CALLBACK ProcessProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam){
    switch(msg){
        case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

        case WM_COMMAND:
            //MessageBox(NULL, "Command received", "Info", MB_OK);//debuging
            //CONTROL CODE IS LOWRD NOTIFICATION CODE IS HIWORD
            if(LOWORD(wparam)==ID_SELECT && HIWORD(wparam)==BN_CLICKED){

                //MessageBox(NULL, "Select button clicked", "Info", MB_OK);//debuging
                char pid_buff[50];
                char name_buff[120];
                int pos =ListView_GetNextItem(gwin.hList,-1,LVNI_SELECTED);
                if(pos == -1){
                    MessageBox(hwnd, "No process selected", "Error", MB_OK);
                    break;
                }
                int pid_column = 1;
                int name_colunm =0;

                ListView_GetItemText(gwin.hList,pos,pid_column,pid_buff,sizeof(pid_buff));
                gwin.pid = atoi(pid_buff);
                if(gwin.pid == 0){
                MessageBox(hwnd, "Invalid PID", "Error", MB_OK);
                break;
                }  

                ListView_GetItemText(gwin.hList,pos,name_colunm,name_buff,sizeof(name_buff));

                //MessageBox(hwnd, name_buff, "Select process name", MB_OK);//debuging
                //MessageBox(hwnd, pid_buff, "Selected PID", MB_OK);//debuging
                snprintf(gwin.info_buff,sizeof(gwin.info_buff),"%s - %d",name_buff,gwin.pid);

               
                //char dbg[100];
                //snprintf(dbg, sizeof(dbg), "Posting WM_REFRESH to: %p", GetParent(hwnd));//debugging
                //MessageBox(NULL, dbg, "DEBUG POST", MB_OK);//debugging
                PostMessage(GetParent(hwnd),WM_REFRESH,0,0);
                DestroyWindow(hwnd);

            }
            break;
        default:
            return DefWindowProc(hwnd,msg,wparam,lparam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam){
    switch(msg){
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_CREATE:
            //test for now find a way to get the adress from the list
            CREATE_LEFT_SIDE_Table(hwnd,&global_address_info);//
            CREATE_BOTTOM_LIST(hwnd);
            CREATE_SIDE_OPTIONS(hwnd);
            CREATE_COMBO_BOX(hwnd);
            CREATE_GROUP_BOX(hwnd);
            CREATE_SCAN(hwnd);
            NEXT_SCAN(hwnd);
            show_selected_process(hwnd);
            create_status_bar(hwnd);
            
            HMENU hmenu,hsub,hsub2;
            hmenu=CreateMenu();

            //create pop up
            hsub=CreatePopupMenu();
            AppendMenu(hsub,MF_STRING,ID_OPEN_PROCESS,"Select process");
            AppendMenu(hsub,MF_STRING,ID_FILE_EXIT,"Exit");
            AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT_PTR)hsub, "File");

            hsub2=CreatePopupMenu();
            AppendMenu(hsub2,MF_STRING,ID_HELP_ABOUT,"Help");
            AppendMenu(hmenu,MF_STRING|MF_POPUP,(UINT_PTR)hsub2,"Go");

            SetMenu(hwnd,hmenu);
            break;
        case WM_COMMAND:
            switch(LOWORD(wparam)){
                //implement range selection scan
                case ID_FILE_EXIT:
                    PostMessage(hwnd,WM_CLOSE,0,0);
                    break;
                case ID_HELP_ABOUT:
                    MessageBox(NULL,"A work in progress memory hacker","LOL 2.O",MB_OK|MB_ICONINFORMATION);
                    break;
                case ID_OPEN_PROCESS:
                    CREATE_LIST(hwnd,&global_process);
                    break;
                case ID_RC_CANCEL:
                    ListView_DeleteItem(gwin.hlist_bottom,0);
                case ID_SCAN_BUTTON:
                    if(HIWORD(wparam)==BN_CLICKED){
                        MessageBeep(MB_ICONINFORMATION);
                        get_target_value();
                        //create thread to scan memory so ui doesnt freeze
                        thread_params *params = malloc(sizeof(thread_params));
                        params->pid=gwin.pid;
                        params->Target=gwin.value;
                        params->hwnd_test = hwnd;
                        HANDLE threads=CreateThread(NULL,0,scan_thread,params,0,NULL);
                        CloseHandle(threads);
                        SendMessage(gwin.bar,SB_SETTEXT,2,(LPARAM)"Scanning");
                        //do not wait for thread it kinda crashes my computer
                    }
                    break;
                case ID_NEXT_SCAN_BUTTON:
                    if(HIWORD(wparam)==BN_CLICKED){
                        MessageBeep(MB_ICONINFORMATION);
                        char address_buff[100];
                        snprintf(address_buff,sizeof(address_buff)," filtered adrress: %d",global_filtered_info.count);
                        compare_changes(gwin.pid,&global_address_info);
                        refresh_left_table_filter(hwnd);
                        SendMessage(gwin.bar,SB_SETTEXT,1,(LPARAM)address_buff);
                    }
                    break;
                case ID_COMBO_BOX_DROP:
                    if(HIWORD(wparam)==CBN_SELCHANGE){
                        //does nothing for now
                        int Item_index = SendMessage((HWND)lparam,(UINT)CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
                        SendMessage((HWND)lparam,(UINT)CB_GETLBTEXT,(WPARAM)Item_index,(LPARAM)gwin.selection);
                    }
                    break;
            }
            break;
        case WM_REFRESH:
            //this is not being called after the post message for some reason
            //the issue was due to the process window being an overlappedwindow 
            SetWindowText(gwin.info, gwin.info_buff);
            SendMessage(gwin.bar,SB_SETTEXT,0,(LPARAM)gwin.info_buff);
            break;
        case WM_SCAN_THREAD_FINISHED:
            refresh_left_table(hwnd);
            char address_buff[100];
            snprintf(address_buff,sizeof(address_buff),"Address Count: %d",global_address_info.count);
            SendMessage(gwin.bar,SB_SETTEXT,1,(LPARAM)address_buff);
            SendMessage(gwin.bar,SB_SETTEXT,2,(LPARAM)"Done");

            break;     
        case WM_SIZE:
            SendMessage(gwin.bar,WM_SIZE,0,0);
            break;
            
        case WM_NOTIFY:
            LPNMHDR pnmh = (LPNMHDR)lparam;
            switch(pnmh->code){
                case NM_DBLCLK:
                    int pos = ListView_GetNextItem(gwin.hlist_left_table,-1,LVNI_SELECTED);
                    if(pnmh->idFrom == ID_LEFT_TABLE){
                        CREATE_CHECK_BOX(pos);
                    }
                    //check to make sure the item is selected first before calling write function
                    if(pnmh->idFrom ==ID_BOTTOM_TABLE){
                    char addr[64];
                    char buff[128];
                    int addr_column  = 0;
                    if(pos != -1){
                        ListView_GetItemText(gwin.hlist_left_table,pos,addr_column,addr,sizeof(addr));
                        gwin.write_address= strtoull(addr,NULL,16);
                        snprintf(buff,sizeof(buff),"the address u selected is 0x%llx",gwin.write_address);
                        MessageBox(NULL,buff,"test",MB_OK);
                        create_popup(hwnd);
                    } 
                }
                case NM_RCLICK:
                    HMENU hpop;
                    POINT cur_point;
                    if(pnmh->idFrom == ID_BOTTOM_TABLE){
                        if(pos != -1){
                            hpop=CreatePopupMenu();
                            if(GetCursorPos(&cur_point)){
                                AppendMenu(hpop,MF_STRING,ID_RC_WRITE,"Place holder lol");
                                AppendMenu(hpop,MF_STRING,ID_RC_CANCEL,"Remove");
                                //create shortcut menu
                                TrackPopupMenu(hpop,TPM_CENTERALIGN,cur_point.x,cur_point.y,
                                0,hwnd,NULL);
                            }
                        }
                    }
                    DestroyMenu(hpop);
                break;
            
            }
            break;
        case WM_WRITE_VAL:
            write_to_address(gwin.write_address,gwin.form_value);   
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd,msg,wparam,lparam);
        }
    return 0;
}

HWND create_popup(HWND parent){
    HWND popup;
    HWND write_button;
    HWND cancel_button;
    HWND form_label;
    char address_buff[64];

    int x_padding = 140;
    int y_padding = 50;
    int x_gap = 100;
    int y_gap = 20;
   
    snprintf(address_buff,sizeof(address_buff)," Writting to 0x%llx",gwin.write_address);
    popup=CreateWindowEx(WS_EX_CLIENTEDGE,"Popupwindow",address_buff,
        WS_POPUPWINDOW|WS_VISIBLE|WS_CAPTION,
        20,20,400,200,parent,
        NULL,GetModuleHandle(NULL),NULL);
    
    
    form_label =CreateWindowEx(0,
        "STATIC",
        "Value",
        WS_CHILD|WS_VISIBLE,
        x_padding-100,y_padding,300,20,
        popup,NULL,GetModuleHandle(NULL),NULL);
        y_padding+=y_gap;

    gwin.address_form =CreateWindowEx(WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD|WS_VISIBLE|WS_BORDER,
        x_padding-100,y_padding,300,25,
        popup,(HMENU)ID_POPUP_FORM,GetModuleHandle(NULL),NULL);
        y_padding+= y_gap*2;

    write_button = CreateWindowEx(0,
        "BUTTON",
        "WRITE",
        BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE,
        x_padding,y_padding,80,20,
        popup,(HMENU)ID_POPUP_WRITE,GetModuleHandle(NULL),NULL);
        x_padding+=x_gap;

    cancel_button = CreateWindowEx(0,
        "BUTTON",
        "CANCEL",
        BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE,
        x_padding,y_padding,80,20,
        popup,(HMENU)ID_POPUP_CANCEL,GetModuleHandle(NULL),NULL);   
    
    return popup;

}

void handle_address_range(){
    char start_address_buff[124];
    char end_address_buff[124];
    char pid_dgb[50];
    snprintf(pid_dgb,sizeof(pid_dgb),"PID: %d",gwin.pid);
    GetDlgItem(gwin.group_box,ID_GROUP_BOX);
    GetDlgItemText(gwin.group_box,ID_START_EDIT,start_address_buff,sizeof(start_address_buff));
    GetDlgItemText(gwin.group_box,ID_STOP_EDIT,end_address_buff,sizeof(end_address_buff));
    unsigned long long start_address =strtoull(start_address_buff,NULL,16);
    unsigned long long end_address =strtoull(end_address_buff,NULL,16);
    //scan_memory((DWORD)gwin.pid,start_address,end_address);

}
HWND CREATE_SCAN(HWND Parent){
    HWND scan_button;
    scan_button=CreateWindowEx(WS_EX_WINDOWEDGE,
    "BUTTON",
    "FIRST SCAN",
    BS_PUSHBUTTON|WS_VISIBLE|WS_CHILD,
    520,30,100,20,
    Parent,(HMENU)ID_SCAN_BUTTON,
    GetModuleHandle(NULL),
    NULL);
    return scan_button;
    }

HWND CREATE_COMBO_BOX(HWND Parent){
    gwin.my_drop_down =CreateWindowEx(WS_EX_CLIENTEDGE,WC_COMBOBOX,
    "",
    CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE|WS_VSCROLL,
    570,65,200,300,//set the x and y to be of reasonable size so the fulldrop down can show
    Parent,(HMENU)ID_COMBO_BOX_DROP,
    GetModuleHandle(NULL),
    NULL);
    //find a way to get the type value you want to search for and use that in the scan
    //for now i can just scan 4 bytes
    SendMessage(gwin.my_drop_down,(UINT)CB_ADDSTRING,(WPARAM)0,(LPARAM)"<Select Type>");
    SendMessage(gwin.my_drop_down,(UINT)CB_ADDSTRING,(WPARAM)0,(LPARAM)"INT");
    SendMessage(gwin.my_drop_down,(UINT)CB_ADDSTRING,(WPARAM)0,(LPARAM)"FLOAT");
    SendMessage(gwin.my_drop_down,(UINT)CB_SETCURSEL,(WPARAM)0,(LPARAM)0);
    return gwin.my_drop_down;
    
}

HWND NEXT_SCAN(HWND Parent){
    HWND next_scan_button;
    next_scan_button=CreateWindowEx(WS_EX_WINDOWEDGE,
    "BUTTON",
    "NEXT SCAN",
    BS_PUSHBUTTON|WS_VISIBLE|WS_CHILD,
    640,30,100,20,
    Parent,(HMENU)ID_NEXT_SCAN_BUTTON,
    GetModuleHandle(NULL),
    NULL);

    return next_scan_button;
    }

HWND CREATE_GROUP_BOX(HWND Parent){
    int x_padding = 20;
    int y_padding = 40;
    int gap_y = 20;
    
    gwin.group_box = CreateWindowEx(0,
    "BUTTON",
    "Mem scan options",
    BS_GROUPBOX |WS_VISIBLE|WS_CHILD,
    520,130,270,210,
    Parent,
    NULL,
    GetModuleHandle(NULL),
    NULL);
    CreateWindowEx(
        0,
        "STATIC",          // this is the label
        "Start:",          // text to display
        WS_VISIBLE | WS_CHILD,
        x_padding,y_padding,100,25,
        gwin.group_box,
        (HMENU)ID_GROUP_BOX,
        GetModuleHandle(NULL),
        NULL
    );
     y_padding +=gap_y;
    CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_BORDER|WS_CHILD|WS_VISIBLE,
        x_padding,y_padding,100,20,
        gwin.group_box,
        (HMENU)ID_START_EDIT,
        GetModuleHandle(NULL),
        NULL);
        y_padding +=gap_y;


    CreateWindowEx(
        0,
        "STATIC",          //this is the label
        "Stop:",          //text to display
        WS_VISIBLE | WS_CHILD,
        x_padding,y_padding,100,25,
        gwin.group_box,
        NULL,
        GetModuleHandle(NULL),
        NULL);
        y_padding +=gap_y;

    CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_BORDER|WS_CHILD|WS_VISIBLE,
        x_padding,y_padding,100,20,
        gwin.group_box,
        (HMENU)ID_STOP_EDIT,
        GetModuleHandle(NULL),
        NULL);
        y_padding+=gap_y;

    CreateWindowEx(WS_EX_CLIENTEDGE,
    "BUTTON",
    "WRITEABLE",
    WS_CHILD|BS_CHECKBOX|WS_VISIBLE,
    x_padding,y_padding,100,20,
    gwin.group_box,
    (HMENU)ID_WRITEABLE_CHECK,
    GetModuleHandle(NULL),
    NULL
    );
    y_padding+=gap_y+10;

    return gwin.group_box;
}
HWND CREATE_LEFT_SIDE_Table(HWND PARENT,address_arr *addr_arr){
    gwin.hlist_left_table = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            WC_LISTVIEW,
            "",
            WS_CHILD | WS_VISIBLE | LVS_REPORT,
            20,40,400,300,
            PARENT,(HMENU)ID_LEFT_TABLE,
            GetModuleHandle(NULL),NULL
            );
            
    LVCOLUMN col;
    col.mask = LVCF_TEXT | LVCF_WIDTH;
    col.cx = 200;
    col.pszText = "Address";
    ListView_InsertColumn(gwin.hlist_left_table,0,&col);

    LVCOLUMN col2;
    col2.mask = LVCF_TEXT | LVCF_WIDTH; 
    col2.cx = 200;
    col2.pszText = "Value";
    ListView_InsertColumn(gwin.hlist_left_table,1,&col2);

    LVCOLUMN col3;
    col3.mask = LVCF_TEXT | LVCF_WIDTH; 
    col3.cx = 200;
    col3.pszText = "Previous";
    ListView_InsertColumn(gwin.hlist_left_table,2,&col3);

    return gwin.hlist_left_table;
}
        
HWND CREATE_SIDE_OPTIONS(HWND Parent){
    int y_padding =65;
    int gap_y =25;
        CreateWindowEx(
        0,
        "STATIC",          //this is the label
        "Value:",          // text to display
        WS_VISIBLE | WS_CHILD,
        520,y_padding,220,25,
        Parent,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    y_padding+=gap_y;

    gwin.h_options =CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_BORDER|WS_CHILD|WS_VISIBLE,
        520,y_padding,300,20,
        Parent,
        (HMENU)ID_EDIT_VALUE,
        GetModuleHandle(NULL),
        NULL);
    return gwin.h_options;
}

void get_target_value(){
    //char dbg[120];
    //char dbg_2[120];
    char target_value[64];
    GetWindowText(gwin.h_options, target_value, sizeof(target_value));
    gwin.value = atoi(target_value);
    //snprintf(dbg,sizeof(dbg),"%d",value);
    //MessageBox(NULL,dbg,"DEBUG",MB_OK);
    //snprintf(dbg_2,sizeof(dbg_2),"%d",gwin.pid);
    //MessageBox(NULL,dbg_2,"DEBUG",MB_OK);

}
//add pid serach
//add memory addition
void CREATE_CHECK_BOX(int pos){
    ListView_SetExtendedListViewStyle(
    gwin.hlist_bottom,
    LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

    char value_buff[64];
    char address_buff[64];
    int value_col = 3;
    int type_col = 2;
    ListView_GetItemText(gwin.hlist_left_table,pos,1,value_buff,64);
    ListView_GetItemText(gwin.hlist_left_table,pos,0,address_buff,64);  

    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT;
    lvi.iItem = pos;  // or a tracked count
    lvi.pszText=address_buff;
    ListView_InsertItem(gwin.hlist_bottom, &lvi);
    ListView_SetItemText(gwin.hlist_bottom,pos,value_col,value_buff);
    ListView_SetItemText(gwin.hlist_bottom,pos,type_col,gwin.selection);

    

}
HWND CREATE_BOTTOM_LIST(HWND PARENT){
    gwin.hlist_bottom = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            WC_LISTVIEW,
            "",
            WS_CHILD | WS_VISIBLE | LVS_REPORT,
            20,390,800,200,
            PARENT,(HMENU)ID_BOTTOM_TABLE,
            GetModuleHandle(NULL),
            NULL
            );
    // Add column
    LVCOLUMN col;
    col.mask = LVCF_TEXT | LVCF_WIDTH;
    col.cx = 200;
    col.pszText = "ADDRESS";
    ListView_InsertColumn(gwin.hlist_bottom,0,&col);

    LVCOLUMN col2;
    col2.mask = LVCF_TEXT | LVCF_WIDTH; 
    col2.cx = 200;
    col2.pszText = "DESCRIPTION";
    ListView_InsertColumn(gwin.hlist_bottom,1,&col2);

    LVCOLUMN col3;
    col3.mask = LVCF_TEXT | LVCF_WIDTH; 
    col3.cx = 200;
    col3.pszText = "TYPE";
    ListView_InsertColumn(gwin.hlist_bottom,2,&col3);

    LVCOLUMN col4;
    col4.mask = LVCF_TEXT | LVCF_WIDTH; 
    col4.cx = 200;
    col4.pszText = "VALUE";
    ListView_InsertColumn(gwin.hlist_bottom,3,&col4);



    return gwin.hlist_bottom;

}

HWND create_status_bar(HWND Parent){
    gwin.bar=CreateWindowEx(0,STATUSCLASSNAME,NULL,CCS_BOTTOM|WS_CHILD|WS_VISIBLE,
        0,0,0,0,
        Parent,(HMENU)ID_STATUS_BAR,GetModuleHandle(NULL),NULL);
    
    int widths[] ={100,400,-1};
    SendMessage(gwin.bar,SB_SETPARTS,sizeof(widths)/sizeof(int),(LPARAM)widths);
    return gwin.bar;

}
HWND CREATE_LIST(HWND PARENT,process_arr *array){ 
        //might move proc list here
        //when posting to a parent window avoid using WS_OVERLAPPEDWINDOW
        getproclist();
        HWND hlist_main;
        HWND button;
        //create main window for list
        hlist_main = CreateWindowEx(WS_EX_CLIENTEDGE,
            "ProcessListWindow",
            "test",
            WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_SYSMENU,
            20,20,500,600,
            PARENT,
            NULL,
            GetModuleHandle(NULL),
            NULL);
            
        //add button to select process
        button=CreateWindow(
        "BUTTON",
        "SELECT",
        WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON|BS_CENTER,
        380,470,80,20,
        hlist_main,
        (HMENU)ID_SELECT,
        (HINSTANCE)GetWindowLongPtr(hlist_main,GWLP_HINSTANCE),
        NULL);
        
        
        // Create ListView
        gwin.hList = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            WC_LISTVIEW,
            "Processes",
            WS_VISIBLE |WS_CHILD| LVS_REPORT |LVS_AUTOARRANGE|LVS_SINGLESEL,
            20,20,350,500,
            hlist_main,
            NULL,
            GetModuleHandle(NULL),
            NULL
            );

            // Add column
            LVCOLUMN col;
            col.mask = LVCF_TEXT | LVCF_WIDTH;
            col.cx = 200;
            col.pszText = "Process Name";
            ListView_InsertColumn(gwin.hList,0,&col);

            LVCOLUMN col2;
            col2.mask = LVCF_TEXT | LVCF_WIDTH; 
            col2.cx = 200;
            col2.pszText = "PID";
            ListView_InsertColumn(gwin.hList,1,&col2);

            LVCOLUMN col3;
            col3.mask = LVCF_TEXT | LVCF_WIDTH; 
            col3.cx = 200;
            col3.pszText = "Thread";
            ListView_InsertColumn(gwin.hList,2,&col3);


            // Add row
            for(size_t i =0 ; i<array->count;i++){
            LVITEM item = {0};
            item.mask = LVIF_TEXT;
            item.iItem = (int)i;
            item.iSubItem = 0;
            item.pszText = array->entries[i].NAME;
            ListView_InsertItem(gwin.hList,&item);

            char pid[32];
            snprintf(pid,sizeof(pid),"%d",array->entries[i].PID);
            ListView_SetItemText(gwin.hList,i,1,pid);

            char thread[32];
            snprintf(thread,sizeof(thread),"%d",array->entries[i].Thread);
            ListView_SetItemText(gwin.hList,i,2,thread);
            }


    return gwin.hList;
}
HWND show_selected_process(HWND Parent){
        int x_padding = 20;
        int y_padding = 10;
        gwin.info= CreateWindowEx(WS_EX_CLIENTEDGE,
        "STATIC",
        gwin.info_buff,
        WS_CHILD|WS_VISIBLE,
        x_padding,y_padding,280,20,
        Parent,
        NULL,
        GetModuleHandle(NULL),
        NULL);
        return gwin.info;
}
void refresh_left_table(HWND hwnd){
        ListView_DeleteAllItems(gwin.hlist_left_table);
        if(global_address_info.count == 0 ){
            MessageBox(hwnd,"no readable memory found","info",MB_OK);
            return;
        }
        else if(gwin.pid ==0)
        {
            MessageBox(hwnd,"invalid pid for process","info",MB_OK);
            return;
        }
        else{
                MessageBox(hwnd,"refreshing list","info",MB_OK);
        }
        for(size_t i =0 ; i<global_address_info.count;i++){
            LVITEM item;
            item.mask = LVIF_TEXT;
            item.iItem = i;
            item.iSubItem = 0;
            char addr_buff[43];
            sprintf(addr_buff,"0x%llx",global_address_info.info[i].addr);
            item.pszText = addr_buff;
            ListView_InsertItem(gwin.hlist_left_table,&item);

            char value[64];
            snprintf(value,sizeof(value),"%u",global_address_info.info[i].value);
            ListView_SetItemText(gwin.hlist_left_table,i,1,value);

            char prev[64];
            snprintf(prev,sizeof(prev),"%u",global_address_info.info[i].previous);
            ListView_SetItemText(gwin.hlist_left_table,i,2,prev);
        }
}

void refresh_left_table_filter(HWND hwnd){
        MessageBox(NULL, "WM_REFRESH received", "DEBUG", MB_OK);
        
            ListView_DeleteAllItems(gwin.hlist_left_table);
            if(global_filtered_info.count == 0 ){
                MessageBox(hwnd,"no readable memory found","info",MB_OK);
                return;
            }
            else if(gwin.pid ==0)
            {
                MessageBox(hwnd,"invalid pid for process","info",MB_OK);
                return;
            }
            else{
                MessageBox(hwnd,"refreshing list","info",MB_OK);
            }
            for(size_t i =0 ; i<global_filtered_info.count;i++){
                LVITEM item;
                item.mask = LVIF_TEXT;
                item.iItem = i;
                item.iSubItem = 0;
                char addr_buff_filter[43];
                sprintf(addr_buff_filter,"0x%llx",global_filtered_info.info[i].addr);
                item.pszText = addr_buff_filter;
                ListView_InsertItem(gwin.hlist_left_table,&item);

                char value_filter[64];
                snprintf(value_filter,sizeof(value_filter),"%u",global_filtered_info.info[i].value);
                ListView_SetItemText(gwin.hlist_left_table,i,1,value_filter);

                char prev_filter[64];
                snprintf(prev_filter,sizeof(prev_filter),"%u",global_filtered_info.info[i].previous);
                ListView_SetItemText(gwin.hlist_left_table,i,2,prev_filter);
        }
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
    WNDCLASSEX wc = {0};
    WNDCLASSEX wc2 = {0};
    WNDCLASSEX wc3 = {0};
    HWND hwnd;
    MSG msg;
    INITCOMMONCONTROLSEX icex = {0};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);
    
    //main window class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
    wc.lpszClassName = "myWindowClass";
    wc.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

     if(!RegisterClassEx(&wc)){
        char err_mess[50];
        DWORD err= GetLastError();
        MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        snprintf(err_mess,sizeof(err_mess),"%d",err);
        MessageBox(NULL,err_mess,"error code",MB_ICONEXCLAMATION|MB_OK); 
        return 0;
    }
    //process list window class
    wc2.cbSize=sizeof(WNDCLASSEX);
    wc2.lpfnWndProc =ProcessProc;
    wc2.hInstance=GetModuleHandle(NULL);
    wc2.lpszClassName="ProcessListWindow";
    wc2.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc2.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    if(!RegisterClassEx(&wc2)){
        char err_mess[50];
        DWORD err= GetLastError();
        MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        snprintf(err_mess,sizeof(err_mess),"%d",err);
        MessageBox(NULL,err_mess,"error code",MB_ICONEXCLAMATION|MB_OK); 
        return 0;
    }

    //process window class
    wc3.cbSize=sizeof(WNDCLASSEX);
    wc3.lpfnWndProc = PopupProc;
    wc3.hInstance= GetModuleHandle(NULL);
    wc3.lpszClassName = "Popupwindow";
    wc3.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc3.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    if(!RegisterClassEx(&wc3)){
        char err_mess[50];
        DWORD err= GetLastError();
        MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        snprintf(err_mess,sizeof(err_mess),"%d",err);
        MessageBox(NULL,err_mess,"error code",MB_ICONEXCLAMATION|MB_OK); 
        return 0;
    }

    //create main window
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"myWindowClass","Sucky scan",
        WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,860,680,NULL,NULL,hInstance,NULL);

    if(hwnd == NULL){
        MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);
    while(GetMessage(&msg,NULL,0,0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}