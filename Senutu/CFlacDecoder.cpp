//Date: 2010.3.23

#include "CFlacDecoder.h"
#include "libflac\metadata.h"
#include <dxerr.h>
#include <stdio.h>
#include <iostream>

//static variables declarations
WAVEFORMATEX * CFlacDecoder::m_pwfx = NULL;
XAudio2Manip CFlacDecoder::XACtrl;
FLAC__StreamDecoder * CFlacDecoder::m_pFlacDecoder = NULL;
bool CFlacDecoder::m_bIsPause;
bool CFlacDecoder::m_bIsPlaying;
bool CFlacDecoder::m_bIsStop;
int CFlacDecoder::m_nBufferSpan;
int CFlacDecoder::m_nMinBufTime;
int CFlacDecoder::m_nMaxBufTime;
FLAC__uint64 CFlacDecoder::m_dSampleCount;
 

CFlacDecoder::CFlacDecoder()
{
	m_pFlacDecoder = NULL;
	m_bEnableMD5Checking = true;  //make it enabled by defalut
	m_pFlacDecoder =  FLAC__stream_decoder_new();
	m_pwfx = new WAVEFORMATEX;
	if (m_pFlacDecoder == NULL || m_pwfx == NULL)
		throw(AR_ERROR_DECODER_NOT_INITIALIZED);  //flac decoder not initialized
}

CFlacDecoder::~CFlacDecoder()
{
	if (m_pFlacDecoder != NULL){
		FLAC__stream_decoder_delete(m_pFlacDecoder);
		m_pFlacDecoder = NULL;
	}
	SAFE_DELETE(m_pwfx);
}

ARESULT CFlacDecoder::Open( LPWSTR strFileName)
{
	if (strFileName == NULL)
		return  AR_NULL_STRING_ARG;
	FLAC__stream_decoder_set_md5_checking(m_pFlacDecoder,m_bEnableMD5Checking);   //set MD5 check function
	
	int length = WideCharToMultiByte(CP_ACP, 0, strFileName, -1, NULL, 0, NULL, NULL); 	
	char * fileName = new char[length+1]; 
	WideCharToMultiByte(CP_ACP, 0, strFileName, -1, fileName, length, NULL, NULL);   //convert LPWSTR to char*

	FLAC__metadata_get_streaminfo(fileName,&m_StreamInfo);  //get matadata of the file
	if (m_StreamInfo.length < 0 || m_StreamInfo.type < 0)  //format error
		return  AR_FORMAT_ERR;
	
	FLAC__StreamDecoderInitStatus ErrorCode = FLAC__stream_decoder_init_file(m_pFlacDecoder, fileName, WriteCallback, NULL, ErrorCallback,NULL);
	SAFE_DELETE_ARRAY(fileName);
	
	if(ErrorCode != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
		FLAC__stream_decoder_delete(m_pFlacDecoder);
		m_pFlacDecoder = NULL;
		return atrace_error(FLAC__StreamDecoderInitStatusString[ErrorCode], AR_ERROR_OPEN_FILE); //fail to open the file
	}
	
	FLAC__stream_decoder_process_until_end_of_metadata(m_pFlacDecoder);
	int frame = 0;
	while ((FLAC__stream_decoder_get_state(m_pFlacDecoder)) != FLAC__STREAM_DECODER_END_OF_STREAM ){
		FLAC__uint64 position;
		FLAC__stream_decoder_get_decode_position(m_pFlacDecoder,&position);	
		m_mapFrameToBytes.insert(make_pair(frame++,position));   //build the map, for the GetCurTime() call
		FLAC__stream_decoder_skip_single_frame(m_pFlacDecoder);		
	}
	m_dFrameCount = frame;
	FLAC__stream_decoder_reset(m_pFlacDecoder);

	m_dSampleCount = m_StreamInfo.data.stream_info.total_samples;
	m_pwfx->nChannels = m_StreamInfo.data.stream_info.channels;
	m_pwfx->nSamplesPerSec = m_StreamInfo.data.stream_info.sample_rate;
	m_pwfx->wBitsPerSample = m_StreamInfo.data.stream_info.bits_per_sample;
	m_pwfx->nAvgBytesPerSec = m_pwfx->nChannels * m_pwfx->nSamplesPerSec * m_pwfx->wBitsPerSample / 8;
	m_pwfx->wFormatTag = WAVE_FORMAT_PCM;
	m_pwfx->cbSize = 0;
	m_pwfx->nBlockAlign = m_pwfx->nChannels  * m_pwfx->wBitsPerSample / 8;   //set the WAVEFORMATEX structure
	m_dTimePerFrame = m_dSampleCount / m_StreamInfo.data.stream_info.sample_rate  * 1000.0;
	return XACtrl.SetFormat(m_pwfx);
}

ARESULT CFlacDecoder::Play()
{
	m_bIsPlaying = true;
    m_bIsPause = false;
    m_bIsStop = false;	
    return AR_OK;
}

ARESULT CFlacDecoder::Pause()
{
	XACtrl.FlushSourceBuffers();
    if (m_bIsStop)
		return AR_OK;
    m_bIsPause = true;
    m_bIsStop = false;
    m_bIsPlaying = false;
    return AR_OK;
}

ARESULT  CFlacDecoder::Stop()
{	
	XACtrl.Stop();
    XACtrl.FlushSourceBuffers();
	FLAC__stream_decoder_reset(m_pFlacDecoder);
	m_bIsStop = true;
	m_bIsPlaying = false;
	m_bIsPause = false;
	return AR_OK;
	//if (FLAC__stream_decoder_seek_absolute(m_pFlacDecoder,0)){

	//	XACtrl.Start();
	//	return AR_OK;
	//}
	//else
	//	return atrace_error_IMP(L"Oops... I just can't stop!", AR_E_FAILTOSEEK, "CFlacDecoder.cpp",104); //fail to stop
}


ARESULT CFlacDecoder::Sync(int minBufTime,int MaxBufTime,int BufferSpan)
{
	if (m_bIsPlaying){
		if (FLAC__stream_decoder_get_state(m_pFlacDecoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
			Stop();
		m_bIsPlaying = true;
		minBufTime = 500;  //for debug use
		m_nMinBufTime = minBufTime;
		m_nMaxBufTime = MaxBufTime;
		m_nBufferSpan = BufferSpan;  // copy these parameters for later use
		if (XACtrl.bufferedTime() <= minBufTime){
			FLAC__bool ok = FLAC__stream_decoder_process_single(m_pFlacDecoder);
				//start working, proceed to call backs, decode a single frame or metadata block for each time
			if (!ok)
				return atrace_error(FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(m_pFlacDecoder)],	AR_ERROR_WHILE_DECODING);
		}
	}
	return AR_OK;
}


FLAC__StreamDecoderWriteStatus CFlacDecoder::WriteCallback(const FLAC__StreamDecoder * /*decoder*/, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void * /*client_data*/)
{
	XAUDIO2_BUFFER * pBuffer = new XAUDIO2_BUFFER();
	pBuffer->pAudioData = new BYTE[frame->header.blocksize * frame->header.channels * 2];
	pBuffer->AudioBytes = frame->header.blocksize * frame->header.channels * 2;
	DWORD done = 0;
	for(size_t i = 0; i < frame->header.blocksize; ++i) 
		for (size_t j = 0 ; j < frame->header.channels ; ++j){
			pBuffer->pAudioData[done++] = static_cast<BYTE>(buffer[j][i]);
			pBuffer->pAudioData[done++] = static_cast<BYTE>(buffer[j][i] >> 8);  //eliminate its high digit
		}
	XACtrl.SubmitSourceBuffer(pBuffer);   
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void CFlacDecoder::ErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
	fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
	atrace_error_IMP(FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(m_pFlacDecoder)],	AR_ERROR_WHILE_DECODING,"CFlacDecoder.cpp",117);
}


float CFlacDecoder::GetVolume()
{
	return XACtrl.GetVolume();
}

ARESULT CFlacDecoder::SetVolume(float theVolume)
{
	return XACtrl.SetVolume(theVolume);
}

ARESULT CFlacDecoder::Close()
{
	XACtrl.Stop();
	XACtrl.FlushSourceBuffers();
	if (m_pFlacDecoder != NULL){
		FLAC__stream_decoder_delete(m_pFlacDecoder);
		m_pFlacDecoder = NULL;
	}
	SAFE_DELETE(m_pwfx);
	return AR_OK;
}

int CFlacDecoder::GetFullTime()
{
	return int(m_dSampleCount / m_StreamInfo.data.stream_info.sample_rate  * 1000);
}

int CFlacDecoder::GetCurTime()
{
	FLAC__uint64 bytesOff;
	FLAC__stream_decoder_get_decode_position(m_pFlacDecoder, &bytesOff); //get the bytes off the beginning of the stream
	map<int,FLAC__uint64>::iterator iter = m_mapFrameToBytes.begin();
	int frame = 0;
	for ( ; iter != m_mapFrameToBytes.end() ; ++iter)
		if (iter->second == bytesOff){
			frame = iter->first;  //find its corresponding frame number
			break;
		}
	double timePerFrame = GetFullTime() / m_dFrameCount;
	int buffer = XACtrl.bufferedTime();
	return int(timePerFrame * double(frame) -  double(XACtrl.bufferedTime()) / double(DEFAULT_BUFFER_SPAN) * double(timePerFrame));
//	return double(XACtrl.GetSamplesPlayed()) /double( m_dSampleCount) * double(GetFullTime());
//	FLAC__uint64 position;
//	FLAC__stream_decoder_get_decode_position (m_pFlacDecoder, &position); 
//	return int( double(position) / double(m_pwfx->nAvgBytesPerSec) * 1000) - XACtrl.bufferedTime() / DEFAULT_BUFFER_SPAN * 104.5;
//	m_pwfx->nSamplesPerSec 
}

ARESULT CFlacDecoder::SetCurTime(int time)
{
	time = min(time,GetFullTime());
	time = max(time,0);
	XACtrl.Stop();
    XACtrl.FlushSourceBuffers();	
	FLAC__uint64 SamplesOff = FLAC__uint64(time) * m_pwfx->nSamplesPerSec / 1000;
	FLAC__bool ok = FLAC__stream_decoder_seek_absolute(m_pFlacDecoder,SamplesOff);
	if (ok)
		return AR_OK;
	else
		return atrace_error_IMP(L"Faied to seek",AR_E_FAILTOSEEK,"CFlacDecoder.cpp",222);
}