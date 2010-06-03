#pragma once
#ifndef _COGGDECODER_H
#define _COGGDECODER_H 

//#pragma comment(linker, "/nodefaultlib:ffmpeg\lib\avcodec.lib")
 
#include <xaudio2.h>
#include <math.h>


#include "libogg\vorbisfile.h"
#include "IDecoder.h"
#include "XAudio2Manip.h"

class COggDecoder : public IDecoder{
public:
	COggDecoder();
	virtual ~COggDecoder();
	virtual ARESULT Open( LPWSTR strFileName);
	virtual ARESULT Play();
	virtual ARESULT Pause();
	virtual ARESULT Stop();
	virtual ARESULT Close();
    virtual ARESULT Sync(int minBufTime=500,int MaxBufTime=1000,int BufferSpan = DEFAULT_BUFFER_SPAN);
	virtual int GetFullTime();
	virtual int GetCurTime();
	virtual ARESULT SetCurTime(int time) ;
	//virtual float GetVolume();
	//virtual ARESULT SetVolume(float theVolume);

protected:
    OggVorbis_File *m_pvf;
    int m_iEof;
	int m_iCurrentSection;	

private:
	const char* GetError(int error);
};

#endif