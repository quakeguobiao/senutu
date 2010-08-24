#include "XAudio2Manip.h"

//helper Function
void SAFE_DELETE_XABUFFER(XAUDIO2_BUFFER *&buffer) {
    SAFE_DELETE(buffer->pAudioData)
    SAFE_DELETE(buffer->pContext);
    SAFE_DELETE(buffer);
}

//------------------------------------------------------

XAudio2Manip::XAudio2Manip()
{
	HRESULT hr;
    m_pXAudio2 = NULL;
    m_pMasteringVoice = NULL;
    m_pSourceVoice = NULL;


    CoInitializeEx( NULL, COINIT_MULTITHREADED );  // Initialize XAudio2
    UINT32 flags = 0;
	#ifdef _DEBUG
		flags |= XAUDIO2_DEBUG_ENGINE;   //debug mode
	#endif

    hr = XAudio2Create(&m_pXAudio2,flags);
	if (FAILED(hr))
		throw(AR_CO_E_NOTINITIALIZED); // fail to create an XAudio2 instance
    hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
		throw(AR_ERROR_MASTERVOICE); //fail to create mastering voice
}

XAudio2Manip::~XAudio2Manip()
{
    if (m_pSourceVoice)
		m_pSourceVoice->DestroyVoice();
    if (m_pMasteringVoice)
		m_pMasteringVoice->DestroyVoice();
    SAFE_RELEASE(m_pXAudio2);
    CoUninitialize();
//  SAFE_DELETE(m_pSourceVoice);
//  SAFE_DELETE(m_pMasteringVoice);
    
}


ARESULT XAudio2Manip::SetFormat( WAVEFORMATEX *pwfx )
{
    if (m_pSourceVoice !=NULL) 
        m_pSourceVoice->DestroyVoice();
    HRESULT hr;
	if (FAILED(hr = m_pXAudio2->CreateSourceVoice(&m_pSourceVoice,pwfx)))
		return atrace_error(L"Error creating source voice",AR_ERROR_SRCVOICE);
    if (FAILED(hr = m_pSourceVoice->Start(0)))
		return atrace_error(L"Error submitting source voice",AR_ERROR_SRCVOICE);
    return AR_OK;
}



ARESULT XAudio2Manip::SubmitSourceBuffer( XAUDIO2_BUFFER *buffer )
{
	
   if (buffer) 
	   m_qBufferToPlay.push_back(buffer);//add buffer to |playqueue|
   //or just do the sync thing to keep the deque rolling.
    XAUDIO2_VOICE_STATE VoiceState;

    m_pSourceVoice->GetState(&VoiceState);
	ui64_SamplesPlayed = VoiceState.SamplesPlayed;
    
    while (m_qBufferToRelease.size()>VoiceState.BuffersQueued) {
        XAUDIO2_BUFFER *tmp=m_qBufferToRelease.front();
        m_qBufferToRelease.pop_front();
        SAFE_DELETE_XABUFFER(tmp);           

    }

    while (VoiceState.BuffersQueued<XAUDIO2_MAX_QUEUED_BUFFERS) {
        if (!m_qBufferToPlay.empty()) {
            m_qBufferToRelease.push_back(m_qBufferToPlay.front());
            m_pSourceVoice->SubmitSourceBuffer(m_qBufferToPlay.front());
            m_qBufferToPlay.pop_front();
            VoiceState.BuffersQueued++;
        }
		else {
            break;
        }

    }     
   return AR_OK;
}

ARESULT XAudio2Manip::FlushSourceBuffers()
{
    if (!m_pSourceVoice) return AR_OK;
	if (m_pSourceVoice)
		m_pSourceVoice->Stop();
    if (m_pSourceVoice)
		m_pSourceVoice->FlushSourceBuffers();
    
    while (!m_qBufferToPlay.empty()) {
        SAFE_DELETE_XABUFFER(m_qBufferToPlay.front());
        m_qBufferToPlay.pop_front();
        
    }
    while (!m_qBufferToRelease.empty()) {
        SAFE_DELETE_XABUFFER(m_qBufferToRelease.front());
        m_qBufferToRelease.pop_front();

    }
    //pending
    m_pSourceVoice->Start(0);
     
    return AR_OK;
}

int XAudio2Manip::bufferedTime()
{
   static XAUDIO2_VOICE_STATE vs;
   m_pSourceVoice->GetState(&vs);
   return (vs.BuffersQueued+m_qBufferToPlay.size()) * DEFAULT_BUFFER_SPAN;  
}

float XAudio2Manip::GetVolume()
{
	float result;
	m_pSourceVoice->GetVolume(&result);
	return result;
}

ARESULT XAudio2Manip::SetVolume(float theVolume)
{
	HRESULT hr = m_pSourceVoice->SetVolume(theVolume);
	if (FAILED(hr))
		return atrace_error(L"Error setting volume",AR_ERROR_SRCVOICE)
	else
		return AR_OK;
}