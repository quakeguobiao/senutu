#pragma once
#ifndef _CAUDIOCTRL_H
#define _CAUDIOCTRL_H 

#include "IDecoder.h"


class CAudioCtrl{
public:
    CAudioCtrl() ;
    ~CAudioCtrl(); 
    
    ARESULT Open(LPWSTR lpFileName);
    ARESULT Play();
    ARESULT Pause();
    ARESULT Stop();//jump to 0ms,file is still open,use close to release the file
    ARESULT Close();//release the file
    ARESULT Sync();
	ARESULT SetVolume(float theVolume);
	
	float GetVolume();
	
	//time unit is millisecond
	int GetFullTime();
    int GetCurTime();    
    ARESULT SetCurTime(int time);
    
    bool isPlaying() {
      return m_pIDecoder->isPlaying();
    }
    bool isPause() {
        return m_pIDecoder->isPause();
    }

private:
	HANDLE m_hPlayThread;
	DWORD m_dPlayThreadID;

    IDecoder * m_pIDecoder;

    //check if the filename is of a specific extension name 
    //sample usage : checkExtension("e:\music\apologize.mp3",".mp3");
    bool checkExtension(LPCWSTR lpFileName,LPCWSTR lpExtName);

	//helper function of play thread
	static unsigned int WINAPI playThreadHelper(LPVOID param);
	//multi-threaded implementation of play method;
	int playThread();


};


#endif //_CAUDIOCTRL_H