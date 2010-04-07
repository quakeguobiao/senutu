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
    ARESULT Stop();
    ARESULT Close();
    ARESULT Sync();
	ARESULT SetVolume(float theVolume);
	float GetVolume();
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
    bool checkExtension(LPCWSTR lpFileName,LPCWSTR lpExtName) ;

};
#endif //_CAUDIOCTRL_H