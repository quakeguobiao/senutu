#pragma once

#ifndef _CFFMPEGDECODER_HEAD_
#define _CFFMPEGDECODER_HEAD_ 


#include "IDecoder.h"
#include <iostream>
//#include "debug_new.h" // +

EXTERN_C {
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
}

class CFfmpegDecoder :
    public IDecoder
{
public:
    CFfmpegDecoder(void);
    virtual ARESULT Open( LPWSTR strFileName);
    virtual ARESULT Stop();
    virtual ARESULT Sync(int minBufTime=1500,int MaxBufTime=3000,int BufferSpan=DEFAULT_BUFFER_SPAN);
    
    virtual int GetFullTime() ;
    virtual int GetCurTime() ;
    
    virtual ARESULT SetCurTime(int time);
    virtual ARESULT Close()/* = 0*/;
    virtual ~CFfmpegDecoder(void);
    
private:

/**
 * resize a |array| from |fromSize| bytes to |toSize| bytes,the original data
 * in the buffer is kept during resizing.
 */
    int resizeArray(BYTE *&array,int fromSize,int toSize);
    
/**
 * ffmpeg stuff
 */
    AVFormatContext  *pFormatContext;
    AVCodecContext  *pCodeContext;
    AVCodec    *pCodec;
    AVPacket   packet;
    int     nAudioStream;
/************************************************************************/
/* useless things that still needed                                      */
/************************************************************************/     
    int64_t startTimeStamp;
};

#endif //_CFFMPEGDECODER_HEAD_