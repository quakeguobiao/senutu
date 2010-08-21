//Date: 2010.3.18

#pragma once
#ifndef _CMP3DECODER_H
#define _CMP3DECODER_H 

#include <xaudio2.h>
#include "libmpg123\mpg123.h"
#include "IDecoder.h"
#include "XAudio2Manip.h"

//-----------------------------------------------------------------------------
// Name: class CMp3Decoder
// Desc: a class that provides mp3 decoding
// Note: This class uses libMPG123
//-----------------------------------------------------------------------------

class CMp3Decoder : public IDecoder{
public:
	CMp3Decoder();
	virtual ~CMp3Decoder();
	virtual ARESULT Open( LPWSTR strFileName);
	virtual ARESULT Play();
	virtual ARESULT Pause();
	virtual ARESULT Stop();
	virtual ARESULT Close();
    
	virtual ARESULT Sync(int minBufTime=500,int MaxBufTime=1000,int BufferSpan = DEFAULT_BUFFER_SPAN);
	virtual int GetFullTime();
	virtual int GetCurTime();
	virtual ARESULT SetCurTime(int time);

protected:
	mpg123_handle * m_pMp3Handle;   //an instance of libmpg123 decoder
	mpg123_frameinfo * m_pMpg123Info;
	long m_nSampleCount;
	double m_dTimePerFrame;
	
};


#endif