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
    IDecoder * m_pIDecoder;
    //check if the filename is of a specific extension name 
    //sample usage : checkExtension("c:\a.mp3",".mp3");
    bool checkExtension(LPCWSTR lpFileName,LPCWSTR lpExtName) ;

};
#endif //_CAUDIOCTRL_H