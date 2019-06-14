#include <string>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <nvml.h>
std::vector<std::string> getGpuUtil();
std::string getProgressBar(std::string percent){

    std::string result = " ";
    int _size= 100;
    int  boundaries;
    if (stof(percent) < 10)
        percent = " " + percent;
    try {
        boundaries = (stof(percent)/100.0)*_size;
    } catch (...){
    boundaries = 0;
    }

    for(int i=0;i<_size;i++){
        if(i<=boundaries){
        result +="|";
        }
        else{
        result +=" ";
        }
    }
    result +=" " + percent.substr(0,5) + "%% ";
    return result;
}


char* getCString(std::string str){
    char * cstr = new char [str.length()+1];
    strcpy(cstr, str.c_str());
    return cstr;
}
void writeSysInfoToConsole(std::vector<std::string>& result, WINDOW* sys_win){
    //sys.setAttributes();
    //This is what you would use to print the historic ``Hello World'' in the upper left corner of the screen:

        //(mvw)printw( mywindow, 0, 0, "Hello World %d", 123 );
    //Notice that y and x start at 0, unlike the DOS conio routines that start at 1. Also, y is always before x
    //mvwprintw(sys_win,2,2,getCString(( "OS: " + sys.getOSName())));
    //mvwprintw(sys_win,3,2,getCString(( "Kernel version: " + sys.getKernelVersion())));
    mvwprintw(sys_win,2,2,getCString( "GPU: "));
    //w attr on
    wattron(sys_win,COLOR_PAIR(1));
    //wprintw(sys_win,getCString(getProgressBar(utl.gpu)));
    //wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,3,2,getCString(( "cores:")));
    wattron(sys_win,COLOR_PAIR(1));
    //std::vector<std::string> val = sys.getCoresStats();
    for(int i=0;i<result.size();i++){
     mvwprintw(sys_win,(4+i),2,getCString(result[i]));
    }
    wattroff(sys_win,COLOR_PAIR(1));
    //wrefresh(sys_win);
}

void printMain(std::vector<std::string>& result){
        initscr();                      /* Start curses mode              */
    noecho(); // not printing input values
    cbreak(); // Terminating on classic ctrl + c
    start_color(); // Enabling color change of text
    int yMax,xMax;
    getmaxyx(stdscr,yMax,xMax); // getting size of window measured in lines and columns(column one char length)
    //newwin(w, h, starty, startx)
        WINDOW *sys_win = newwin(17,xMax-1,0,0);
        //WINDOW *proc_win = newwin(15,xMax-1,18,0);

    //init_pair(1,COLOR_BLUE,COLOR_BLACK);
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    int counter = 0;
    while(1){
    box(sys_win,0,0);
    //box (proc_win,0,0);
    //procs.refreshList();
    //std::vector<std::vector<std::string>> processes = procs.getList();
    //result = getGpuUtil();
    writeSysInfoToConsole(result,sys_win);
    wrefresh(sys_win);
    refresh();
    //result = getGpuUtil();
    sleep(1);
    result = getGpuUtil();
    /* 
    if(counter ==  (processes.size() -1)){
        counter = 0;
    }
    else {
        counter ++;
    }
    */
    }
        endwin();
}

std::vector<std::string> getGpuUtil()
{
    nvmlReturn_t result;
    unsigned int device_count, i;
    std::vector<std::string> result_bar = std::vector<std::string>();
    // First initialize NVML library
    result = nvmlInit();
    if (NVML_SUCCESS != result)
    { 
        printf("Failed to initialize NVML: %s\n", nvmlErrorString(result));
        printf("Press ENTER to continue...\n");
        //getchar();
        //return 1;
    }

    result = nvmlDeviceGetCount(&device_count);
    if (NVML_SUCCESS != result)
    { 
        printf("Failed to query device count: %s\n", nvmlErrorString(result));
        goto Error;
    }
    //printf("Found %d device%s\n\n", device_count, device_count != 1 ? "s" : "");

    //printf("Listing devices:\n");    
    
    

    for (i = 0; i < device_count; i++)
    {
        nvmlDevice_t device;
        char name[64];
        nvmlPciInfo_t pci;
        nvmlUtilization_t utl;
        //nvmlUtilization_t::gpu g_p;
        //nvmlUtilization_t::memory m_p;
        nvmlComputeMode_t compute_mode;
        // Query for device handle to perform operations on a device
        // You can also query device handle by other features like:
        // nvmlDeviceGetHandleBySerial
        // nvmlDeviceGetHandleByPciBusId
        result = nvmlDeviceGetHandleByIndex(i, &device);
        if (NVML_SUCCESS != result)
        { 
            printf("Failed to get handle for device %i: %s\n", i, nvmlErrorString(result));
            goto Error;
        }
        result = nvmlDeviceGetName(device, name, sizeof(name)/sizeof(name[0]));
        if (NVML_SUCCESS != result)
        { 
            printf("Failed to get name of device %i: %s\n", i, nvmlErrorString(result));
            goto Error;
        }
        // pci.busId is very useful to know which device physically you're talking to
        // Using PCI identifier you can also match nvmlDevice handle to CUDA device.
        result = nvmlDeviceGetPciInfo(device, &pci);
        if (NVML_SUCCESS != result)
        { 
            printf("Failed to get pci info for device %i: %s\n", i, nvmlErrorString(result));
            goto Error;
        }
        result = nvmlDeviceGetUtilizationRates(device, &utl);
        if (NVML_SUCCESS != result)
        { 
            printf("Failed to get ultilization info for device %i: %s\n", i, nvmlErrorString(result));
            goto Error;
        }
        std::string temp = ("GPU" + std::to_string(i) + ": ");
        temp += getProgressBar(std::to_string(utl.gpu));
        result_bar.push_back(temp);
        //printf("%d. %s [%s] %d %d \n", i, name, pci.busId, utl.gpu, utl.memory);

    }
    return result_bar;
    result = nvmlShutdown();
    if (NVML_SUCCESS != result)
        printf("Failed to shutdown NVML: %s\n", nvmlErrorString(result));
    printf("All done.\n");
    //printf("Press ENTER to continue...\n");
    //getchar();
    throw ;
Error:
    result = nvmlShutdown();   
    if (NVML_SUCCESS != result)  
        printf("Failed to shutdown NVML: %s\n", nvmlErrorString(result));  
    printf("Press ENTER to continue...\n");  
    //getchar();    
}

int main( int   argc, char *argv[] )
{
    std::vector<std::string> result = getGpuUtil();
    printMain(result);
    return 0;
}