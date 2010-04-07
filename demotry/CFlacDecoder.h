//Date: 2010.3.23

#pragma once
#ifndef _CFLACDECODER_H
#define _CFLACDECODER_H 

#include <xaudio2.h>
#include "libflac\stream_decoder.h"
#include <map>
#include "IDecoder.h"
#include "XAudio2Manip.h"
using namespace std;

class CFlacDecoder : public IDecoder{
public:
	CFlacDecoder();
	virtual ~CFlacDecoder();
	virtual ARESULT Open( LPWSTR strFileName);
	virtual ARESULT Play();
	virtual ARESULT Pause();
	virtual ARESULT Stop();
	virtual ARESULT Close();
    virtual float GetVolume();
	virtual ARESULT SetVolume(float theVolume);
    
	virtual ARESULT Sync(int minBufTime = 500,int MaxBufTime=1000,int BufferSpan = DEFAULT_BUFFER_SPAN);
	virtual int GetFullTime();
	virtual int GetCurTime();
	virtual ARESULT SetCurTime(int time);
	virtual bool isPlaying() {return m_bIsPlaying;}
	virtual bool isPause() {return m_bIsPause;}
	
	void setMD5Checking(bool ok){m_bEnableMD5Checking = ok;}  //make sure this method is called before Open(), or it won't work 
    
protected:
	static FLAC__StreamDecoder * m_pFlacDecoder;  //pointer to flac stream decoder
	FLAC__StreamMetadata m_StreamInfo;
    static WAVEFORMATEX * m_pwfx;       //pointer to waveformatex structure
    static XAudio2Manip XACtrl; 
	
	static int m_nMinBufTime, m_nMaxBufTime, m_nBufferSpan;
	
	bool m_bEnableMD5Checking;  //enable MD5 check while decoding, this will reduce slow down the decoding speed
	static FLAC__uint64 m_dSampleCount;
	int m_dFrameCount;
	map<int,FLAC__uint64> m_mapFrameToBytes;  // maps the frame number to the bytes off the beginning of the stream 
	double m_dTimePerFrame;

//play status
    static bool m_bIsPlaying;
    static bool m_bIsPause;
    static bool m_bIsStop;
	
private:
	static FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data);
	static void MetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data){}
	static void ErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data); 

};


#endif