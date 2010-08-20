#pragma once
#ifndef _CAUDIOCTRL_H
#define _CAUDIOCTRL_H 

#include "IDecoder.h"

//This is a singleton class.

class CAudioCtrl{
public:
	static void Init()
	{
		if (m_pAudioCtrl)
			Free();
		m_pAudioCtrl = new CAudioCtrl();  //make sure that only one instance is running
	}
	static void Free();
   
    static ARESULT Open(LPWSTR lpFileName);
    static ARESULT Play();
    static ARESULT Pause();
    static ARESULT Stop();//jump to 0ms,file is still open,use close to release the file
    static ARESULT Close();//release the file
    static ARESULT Sync();
	static ARESULT SetVolume(float theVolume);
	
	static  float GetVolume();
	
	//time unit is millisecond
	static int GetFullTime();
    static int GetCurTime();    
    static ARESULT SetCurTime(int time);
    
    static bool isPlaying() {return m_pIDecoder->isPlaying();}
    static bool isPause() {return m_pIDecoder->isPause();}

private:
	CAudioCtrl();
	~CAudioCtrl() {}  //virtually do nothing. All things are done in  Free()

	static CAudioCtrl * m_pAudioCtrl;

	static HANDLE m_hPlayThread;
	static DWORD m_dPlayThreadID;

    static IDecoder * m_pIDecoder;

    //check if the filename is of a specific extension name 
    //sample usage : checkExtension("e:\music\apologize.mp3",".mp3");
    bool checkExtension(LPCWSTR lpFileName,LPCWSTR lpExtName);

	//helper function of play thread
	static unsigned int WINAPI playThreadHelper(LPVOID param);
	//multi-threaded implementation of play method;
	int playThread();
};


#endif //_CAUDIOCTRL_H










