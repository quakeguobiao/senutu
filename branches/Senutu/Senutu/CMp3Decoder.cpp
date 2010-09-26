//Date: 2010.3.18

#include "CMp3Decoder.h"
#include <dxerr.h>

CMp3Decoder::CMp3Decoder()
{
	 int ErrorCode = mpg123_init();
	 if (ErrorCode != MPG123_OK)
		throw(AR_CO_E_NOTINITIALIZED);  //mpg123 not initialized

	m_pwfx = new WAVEFORMATEX;
	m_pMpg123Info = new mpg123_frameinfo;
	if (m_pwfx == NULL || m_pMpg123Info == NULL)
		throw(AR_CO_E_NOTINITIALIZED);  //bad alloc

}

CMp3Decoder::~CMp3Decoder()
{
	XACtrl.Stop();
	XACtrl.FlushSourceBuffers();
	if (m_pMp3Handle != NULL){
		mpg123_close(m_pMp3Handle);
		mpg123_delete(m_pMp3Handle);
		mpg123_exit();
		SAFE_DELETE(m_pwfx);
		SAFE_DELETE(m_pMpg123Info);
		m_pMp3Handle = NULL;
	}
}

ARESULT CMp3Decoder::Open(LPWSTR strFileName)
{
	if (strFileName == NULL)
		return AR_NULL_STRING_ARG;
	int err;
	m_pMp3Handle = mpg123_new(NULL, &err) ;
	if (m_pMp3Handle == NULL)
		return atrace_error(mpg123_strerror(m_pMp3Handle), AR_E_FAIL);  //fail to create a mpg123 handle
	
	int length = WideCharToMultiByte(CP_ACP, 0, strFileName, -1, NULL, 0, NULL, NULL); 
	char * fileName = new char[length+1]; 
	WideCharToMultiByte(CP_ACP, 0, strFileName, -1, fileName, length, NULL, NULL);   //convert LPWSTR to char*
	
	err = mpg123_open(m_pMp3Handle,fileName); 
	SAFE_DELETE_ARRAY(fileName);
	if (err !=  MPG123_OK){
		Close();
		return atrace_error(mpg123_plain_strerror(err),  AR_ERROR_OPEN_FILE);  //cannot open file
	}
    
	err = mpg123_scan(m_pMp3Handle);
	if (err !=  MPG123_OK){
		Close();
		return atrace_error(mpg123_plain_strerror(err),  AR_ERROR_OPEN_FILE);  //cannot open file
	}

	err = mpg123_info(m_pMp3Handle,m_pMpg123Info);
	if (err !=  MPG123_OK){
		Close();
		return atrace_error(mpg123_plain_strerror(err),  AR_ERROR_OPEN_FILE);  //cannot open file
	}
	m_nSampleCount = mpg123_length(m_pMp3Handle);   //get total samples
	m_dTimePerFrame = mpg123_tpf(m_pMp3Handle);  //get the seconds per frame

	int  channels,encoding;
	long rate;
	err = mpg123_getformat(m_pMp3Handle, &rate, &channels, &encoding);
	if (err != MPG123_OK){
		mpg123_close(m_pMp3Handle);
		mpg123_delete(m_pMp3Handle);
		mpg123_exit();
		return atrace_error(mpg123_plain_strerror(err), AR_FORMAT_ERR);  //something goes wrong with file format
	}

	//load the tag information
	mpg123_id3v1 * v1 = NULL;
	mpg123_id3v2 * v2 = NULL;

	if ((mpg123_meta_check(m_pMp3Handle) & MPG123_ID3) && mpg123_id3(m_pMp3Handle, &v1, &v2) == MPG123_OK) { //has some ID3 info, go ahead
		if (v1){    //id3 v1 is not NULL
			m_tag.Title = v1->title;
			m_tag.Artist = v1->artist;
			m_tag.Album = v1->album;
			m_tag.Year = v1->year;
			m_tag.Comment = v1->comment;
			m_tag.Genre = v1->genre;
		}
		else if (v2){
			m_tag.Title = v2->title->p;
			m_tag.Artist = v2->artist->p;
			m_tag.Album = v2->album->p;
			m_tag.Year = v2->year->p;
			m_tag.Comment = v2->comment->p;
			m_tag.Genre = v2->genre->p;
		}
		m_tag.TotalTime = GetFullTimeSerialized();
	}

	m_pwfx->nChannels = channels;    
	m_pwfx->nSamplesPerSec = rate;
	m_pwfx->wBitsPerSample = 16;   //TBD
	m_pwfx->nAvgBytesPerSec = channels * rate * m_pwfx->wBitsPerSample / 8;
	m_pwfx->wFormatTag = WAVE_FORMAT_PCM;
	m_pwfx->cbSize = 0;
	m_pwfx->nBlockAlign = channels * m_pwfx->wBitsPerSample / 8;   //set the WAVEFORMATEX structure
	
	return XACtrl.SetFormat(m_pwfx);
}

ARESULT CMp3Decoder::Play()
{
    m_bIsPlaying = true;
    m_bIsPause = false;
    m_bIsStop = false;
    return AR_OK;
}

ARESULT CMp3Decoder::Pause()
{
    XACtrl.FlushSourceBuffers();
    if (m_bIsStop) 
		return AR_OK;
    m_bIsPause=true;
    m_bIsStop=false;
    m_bIsPlaying=false;
	return AR_OK;
}

ARESULT CMp3Decoder::Stop()
{
	XACtrl.Stop();
    XACtrl.FlushSourceBuffers();
	mpg123_seek_frame(m_pMp3Handle,0,SEEK_SET);
    m_bIsStop = true;
    m_bIsPlaying = false;
    m_bIsPause = false;
    XACtrl.Start();
    return AR_OK;
}

ARESULT CMp3Decoder::Close()
{
	XACtrl.Stop();
	XACtrl.FlushSourceBuffers();
	if (m_pMp3Handle) {
		mpg123_close(m_pMp3Handle);
		mpg123_delete(m_pMp3Handle);
		mpg123_exit();
		SAFE_DELETE(m_pMpg123Info);
		SAFE_DELETE(m_pwfx);
		m_pMp3Handle = NULL;
	}

	return AR_OK;
}

ARESULT CMp3Decoder::Sync(int minBufTime, int MaxBufTime, int BufferSpan)
{
    if (m_bIsPlaying) {
        while (XACtrl.bufferedTime() < minBufTime) {
            XAUDIO2_BUFFER *buffer = new XAUDIO2_BUFFER();
            size_t done=0;
         
            DWORD toread = m_pwfx->nSamplesPerSec * m_pwfx->nBlockAlign * BufferSpan / 1000;  //100ms' data
            buffer->pAudioData=new BYTE[toread];
			mpg123_read(m_pMp3Handle, buffer->pAudioData, toread, &done ); //read the mp3 file and decode it to pcm
            buffer->AudioBytes = done;
            if (done == toread) {
                XACtrl.SubmitSourceBuffer(buffer);        
            }
			else {
                SAFE_DELETE_XABUFFER(buffer);
				Close();
                //Stop();
                break;
            }
		}
	}
    return AR_OK;
}

ARESULT CMp3Decoder::SetCurTime(int time)
{
	time = min(time,GetFullTime());
	time = max(time,0);
	XACtrl.Stop();
    XACtrl.FlushSourceBuffers();	
	off_t FrameOff = off_t(1 / m_dTimePerFrame * time / 1000);   //calculate how the exact frames from the very beginning
	off_t err = mpg123_seek_frame(m_pMp3Handle,FrameOff,SEEK_SET);   //seek from the beginning
    XACtrl.Start();
	if (err >= 0)
		return AR_OK;
	else
		return atrace_error(mpg123_plain_strerror(err) ,AR_E_FAILTOSEEK);
}


int CMp3Decoder::GetFullTime()
{
	return m_nSampleCount / m_pMpg123Info->rate * 1000;
}

int CMp3Decoder::GetCurTime()
{
	return int(m_dTimePerFrame * mpg123_tellframe(m_pMp3Handle) * 1000);
}

