#pragma once
#ifndef _IDECODER_H
#define _IDECODER_H

#include "aerror.h"
#include "XAudio2Manip.h"
 
//-----------------------------------------------------------------------------
// Name: class CDecoder
// Desc: the base class of all decoders
//-----------------------------------------------------------------------------
class IDecoder{
public:
    IDecoder() :m_pwfx(NULL),m_bIsPlaying(false),m_bIsPause(false),m_bIsStop(true) {}
    virtual ~IDecoder() {}

    virtual ARESULT Open( LPWSTR strFileName) = 0;
    virtual ARESULT Play();
    virtual ARESULT Pause();
    
    virtual ARESULT Stop() = 0;//the file is still open, use |close()| to release the file;
    virtual ARESULT Sync(int minBufTime=500,int MaxBufTime=1000,int BufferSpan=DEFAULT_BUFFER_SPAN)=0;
    virtual int GetFullTime() = 0;
    virtual int GetCurTime() = 0;
	virtual bool isPlaying() {return m_bIsPlaying;}
	virtual bool isPause() {return m_bIsPause;}
    virtual ARESULT SetCurTime(int time) = 0;
    virtual ARESULT Close() = 0;
	virtual float GetVolume();
	virtual ARESULT SetVolume(float theVolume);

protected:
    WAVEFORMATEX * m_pwfx;       //pointer to waveformatex structure
    XAudio2Manip XACtrl;     //an instance of XAudio2Manip

//play status
    bool m_bIsPlaying;
    bool m_bIsPause;
    bool m_bIsStop;
     
   

};



#endif //_IDECODER_H