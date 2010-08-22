#pragma once
#ifndef _IDECODER_H
#define _IDECODER_H

#include "aerror.h"
#include "XAudio2Manip.h"

//This struct defines the tag. e.g. ID3, APE tag, WMA tag...
struct TAG
{
	string Title;
	string Artist;
	string Album;
	string Year;
	string Comment;
	string Genre;
};
typedef struct TAG TAG;
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: class IDecoder
// Desc: the base class of all decoders
//-----------------------------------------------------------------------------
class IDecoder{
public:
    IDecoder() :m_pwfx(NULL),m_bIsPlaying(false),m_bIsPause(false),m_bIsStop(true) {}

	virtual ~IDecoder() {SAFE_DELETE(m_pwfx);}

    virtual ARESULT Open( LPWSTR strFileName) = 0;
    virtual ARESULT Play();
    virtual ARESULT Pause();
    virtual ARESULT Stop() = 0;//the file is still open, use |close()| to release the file;
    virtual ARESULT Sync(int minBufTime=500,int MaxBufTime=1000,int BufferSpan=DEFAULT_BUFFER_SPAN)=0;
    //time unit is millisecond
    
	virtual int GetFullTime() = 0;
	virtual ARESULT SetCurTime(int time) = 0;
    virtual int GetCurTime() = 0;

	virtual bool isPlaying() {return m_bIsPlaying;}
	virtual bool isPause() {return m_bIsPause;}
    virtual ARESULT Close() = 0;//release the file

	virtual float GetVolume();
	virtual ARESULT SetVolume(float theVolume);

	virtual TAG GetTag(){return m_tag;}
protected:
    WAVEFORMATEX * m_pwfx;       //pointer to waveformatex structure
    
    //see Xaudio2Manip.h for detail
    XAudio2Manip XACtrl;     //an instance of XAudio2Manip

	//playing status
    bool m_bIsPlaying;
    bool m_bIsPause;
    bool m_bIsStop;

	TAG m_tag; //stores the tag info
};



#endif //_IDECODER_H