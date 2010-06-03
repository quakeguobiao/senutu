#include "COggDecoder.h"
#include <iostream>

COggDecoder::COggDecoder()
{
	//initialization
	m_pvf=new OggVorbis_File;
	m_pwfx= new WAVEFORMATEX;
	m_iEof = 0;
	if (m_pvf == NULL||m_pwfx == NULL)
		throw(AR_CO_E_NOTINITIALIZED);   
}

COggDecoder::~COggDecoder()
{
	XACtrl.Stop();
	XACtrl.FlushSourceBuffers();
    ov_clear(m_pvf);
	SAFE_DELETE(m_pwfx);
	SAFE_DELETE(m_pvf);
}

ARESULT COggDecoder::Open(LPWSTR strFileName)
{
	if (strFileName == NULL)
		return AR_NULL_STRING_ARG;

    int length = WideCharToMultiByte(CP_ACP, 0, strFileName, -1, NULL, 0, NULL, NULL); 
	char * fileName = new char[length+1]; 
	WideCharToMultiByte(CP_ACP, 0, strFileName, -1, fileName, length, NULL, NULL);   //convert LPWSTR to char*

	int error = 0;
	error = ov_fopen(fileName,m_pvf);
	SAFE_DELETE_ARRAY(fileName);

	if(error < 0)                                                
		 return atrace_error(GetError(error),AR_FORMAT_ERR);

//test tbd


	vorbis_info *vi=ov_info(m_pvf,-1);                 //pull the channel and bitrate info from the file 

	m_pwfx->nChannels = vi->channels;    
	m_pwfx->nSamplesPerSec = vi->rate;
	m_pwfx->wBitsPerSample = 16;   //TBD
	m_pwfx->nAvgBytesPerSec = vi->channels * vi->rate * m_pwfx->wBitsPerSample / 8;
	m_pwfx->wFormatTag = WAVE_FORMAT_PCM;
	m_pwfx->cbSize = 0;
	m_pwfx->nBlockAlign = vi->channels * m_pwfx->wBitsPerSample / 8;   //set the WAVEFORMATEX structure
	return XACtrl.SetFormat(m_pwfx);
	
}

ARESULT COggDecoder::Play()
{
    m_bIsPlaying = true;
    m_bIsPause = false;
    m_bIsStop = false;
    return AR_OK;
}

ARESULT COggDecoder::Pause()
{
    XACtrl.FlushSourceBuffers();
    if (m_bIsStop) 
		return AR_OK;
    m_bIsPause=true;
    m_bIsStop=false;
    m_bIsPlaying=false;
	return AR_OK;
}

ARESULT COggDecoder::Stop()
{
	XACtrl.Stop();
    XACtrl.FlushSourceBuffers();
	SetCurTime(0);
    m_bIsStop = true;
    m_bIsPlaying = false;
    m_bIsPause = false;
    XACtrl.Start();
    return AR_OK;
}

ARESULT COggDecoder::Close()
{
	XACtrl.Stop();
	XACtrl.FlushSourceBuffers();
    ov_clear(m_pvf);
	SAFE_DELETE(m_pwfx);
	SAFE_DELETE(m_pvf);
	return AR_OK;
}

ARESULT COggDecoder::Sync(int minBufTime, int MaxBufTime, int BufferSpan)
{
	if(m_bIsPlaying)
	{
		while (XACtrl.bufferedTime() < minBufTime) {
			if(!m_iEof){
				XAUDIO2_BUFFER *buffer = new XAUDIO2_BUFFER();
				DWORD toread = m_pwfx->nSamplesPerSec * m_pwfx->nBlockAlign * BufferSpan / 1000;//the time of 100ms data
				buffer->pAudioData=new BYTE[toread];
				long bytes_read=ov_read(m_pvf,(char*)buffer->pAudioData,toread,0,2,1,&m_iCurrentSection);
				buffer->AudioBytes = bytes_read;
				if (bytes_read == 0) {      /* m_iEof */
					m_iEof=1;
					SAFE_DELETE_XABUFFER(buffer);
					Stop();
					break;
				} else if (bytes_read < 0) {   /*error in the stream.*/
					return atrace_error_IMP(L"error in the stream.\n",AR_ERROR_WHILE_DECODING,"COggDecoder.cpp",111);
				} else {
					XACtrl.SubmitSourceBuffer(buffer);    
				}
			}
		}
	}
	return AR_OK;
}

int COggDecoder::GetFullTime()
{
	return int(ov_time_total(m_pvf,-1)*1000);
}

int COggDecoder::GetCurTime()
{
	return int(ov_time_tell(m_pvf)*1000);
}

ARESULT COggDecoder::SetCurTime(int time)
{
	time = min(time,GetFullTime());
	time = max(time,0);
	XACtrl.Stop();
    XACtrl.FlushSourceBuffers();	
	int error = ov_time_seek(m_pvf,double(time/1000));
    XACtrl.Start();
	if (error == 0)
		return AR_OK;
	else                                    
		 atrace_error_IMP(GetError(error),AR_FORMAT_ERR,"COggDecoder.cpp",148);
}

const char * COggDecoder::GetError(int error) 
{
	switch(error){
		case OV_EREAD: return "A read from media returned an error." ; break;
		case OV_ENOTVORBIS: return "Bitstream does not contain any Vorbis data."; break;
		case OV_EVERSION: return "Vorbis version mismatch."; break;
		case OV_EBADHEADER: return "Invalid Vorbis bitstream header.";break;
		case OV_EFAULT: return  "Internal logic fault; indicates a bug or heap/stack corruption. ";break;
		case OV_ENOSEEK: return "Bitstream is not seekable.";break;
		case OV_EINVAL :return "Invalid argument value; possibly called with an OggVorbis_File structure that isn't open. ";break;
		case OV_EBADLINK: return "Invalid stream section supplied to libvorbisfile, or the requested link is corrupt. ";break;
	}
}