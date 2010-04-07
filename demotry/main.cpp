//#include<stdlib.h> 
//#include<crtdbg.h> 

//#include "debug_new.h"

#include "CAudioCtrl.h"

#include <Psapi.h>
#include <cstdio>
#include <iostream>
#include <conio.h>
#include <xaudio2.h>
using namespace std;

int wmain(int argc, wchar_t* argv[])
{
//REG_DEBUG_NEW

    //可以从命令行输入打开文件

    CAudioCtrl theCtrl;
    if (argc==2) {
        if (theCtrl.Open(argv[1])!=AR_OK) {
            cout<<"error open file"<<endl;
            return 0;
        }
    } else if (theCtrl.Open(L"h:\\mmm.mp3")!=AR_OK) {
        cout<<"error open file"<<endl;
        return 0;
    }
   // _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );


    theCtrl.Play();

    while (1) {
        
        if(kbhit())
        {
            char a = getch();
            if (a=='p') {
                if (theCtrl.isPlaying()) 
                    theCtrl.Pause();
                else theCtrl.Play();
                _CrtDumpMemoryLeaks();
            }
            if (a=='u')
                theCtrl.SetVolume( theCtrl.GetVolume()+0.2f);
            if (a=='d')
                theCtrl.SetVolume( theCtrl.GetVolume()-0.2f);
            if (a=='s')
                theCtrl.Stop();
            if (a=='h') 
                theCtrl.SetCurTime(theCtrl.GetCurTime()-5000);
                
            if (a=='l')
                theCtrl.SetCurTime(theCtrl.GetCurTime()+5000);
            if (a=='x') {
                theCtrl.Close();
                _CrtDumpMemoryLeaks();
               // getch();
                return 0;
            }
        }
        theCtrl.Sync();
        
        cout<<theCtrl.GetCurTime()<<" "<<theCtrl.GetFullTime()<<"\r";
        
        //don't use more than 50M memory!
        HANDLE h = GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(h, &pmc, sizeof(PROCESS_MEMORY_COUNTERS)))
        {
            if (pmc.WorkingSetSize > 50 *1024 * 1024)
            {
                printf("%d",pmc.WorkingSetSize);
                exit(0);
            }
        }
        CloseHandle(h);
        Sleep(20);//don't sleep more than the time buffered(500ms currently);

    }


    return 0;
}

