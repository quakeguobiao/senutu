#pragma once
#ifndef _CWAVEDECODER_H 
#define _CWAVEDECODER_H  


#include "IDecoder.h"
#include "XAudio2Manip.h"
#include <xaudio2.h>

class CWavDecoder :
    public IDecoder
{
public:
    CWavDecoder(void);
    virtual ARESULT Open( LPWSTR strFileName);
 
    virtual ARESULT Stop();
    virtual ARESULT Close();
    
    virtual ARESULT Sync(int minBufTime=500,int MaxBufTime=1000,int BufferSpan = DEFAULT_BUFFER_SPAN);
    virtual int GetFullTime();
    virtual int GetCurTime();
    virtual ARESULT SetCurTime(int time);
     

    virtual ~CWavDecoder(void);

protected:
    ARESULT ReadMMIO();
    ARESULT ResetFile();
    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
 
    HMMIO m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO m_ck;          // Multimedia RIFF chunk
    MMCKINFO m_ckRiff;      // Use in opening a WAVE file
    DWORD m_dwSize;      // The size of the wave file
    MMIOINFO m_mmioinfoOut;
    ARESULT setUpXA();
  
    
    
    
    
};

#endif //_CWAVEDECODER_H 
 