#include <shlwapi.h>
#include <process.h>

#include "CAudioCtrl.h"
#include "CWavDecoder.h"
#include "CMp3Decoder.h"
#include "CApeDecoder.h"
#include "CFlacDecoder.h"
#include "CFfmpegDecoder.h"
#include "COggDecoder.h"

//static variables declaration
CAudioCtrl * CAudioCtrl::m_pAudioCtrl = NULL;
HANDLE CAudioCtrl::m_hPlayThread = NULL;
IDecoder * CAudioCtrl::m_pIDecoder = NULL;
DWORD CAudioCtrl::m_dPlayThreadID;

CAudioCtrl::CAudioCtrl()
{
	m_pAudioCtrl = this;
	m_pIDecoder = NULL;
	m_bToQuit = false;
}

ARESULT CAudioCtrl::Open(LPWSTR lpFileName )
{  
 
	//TODO: exception handling
	
    ARESULT ar=AR_OK;

	//open as *.mp3
	if (m_pAudioCtrl->checkExtension(lpFileName,L"mp3")) {
		//try to open as *.mp3
		SAFE_DELETE(m_pAudioCtrl->m_pIDecoder);    
		m_pAudioCtrl->m_pIDecoder=new CMp3Decoder();
		ar=m_pAudioCtrl->m_pIDecoder->Open(lpFileName);
		if (ar==AR_OK)
			return AR_OK;
	}

	
	//try to open as *.ape
	SAFE_DELETE(m_pAudioCtrl->m_pIDecoder);    
	m_pAudioCtrl->m_pIDecoder=new CApeDecoder();
	ar=m_pAudioCtrl->m_pIDecoder->Open(lpFileName);
	if (ar==AR_OK)
		return AR_OK;

	//try to open as *.flac
	SAFE_DELETE(m_pAudioCtrl->m_pIDecoder);    
	m_pAudioCtrl->m_pIDecoder=new CFlacDecoder();
	ar=m_pAudioCtrl->m_pIDecoder->Open(lpFileName);
	if (ar==AR_OK)
		return AR_OK;

	//try to open as *.wma ( or other formats ffmpeg supports)
	if (/*!m_pAudioCtrl->checkExtension(lpFileName,L"mp3")*/true) {
		SAFE_DELETE(m_pAudioCtrl->m_pIDecoder);
		m_pAudioCtrl->m_pIDecoder=new CFfmpegDecoder();
		ar=m_pAudioCtrl->m_pIDecoder->Open(lpFileName);
		if (ar==AR_OK)
			return AR_OK;
	}
	

	//try to open as *.ogg
	SAFE_DELETE(m_pAudioCtrl->m_pIDecoder);
	try{
		m_pAudioCtrl->m_pIDecoder=new COggDecoder();
		ar=m_pAudioCtrl->m_pIDecoder->Open(lpFileName);
		if (ar==AR_OK)
			return AR_OK;
	}
	catch (exception /*e*/)
	{
		ar =  AR_ERROR_DECODER_NOT_INITIALIZED;
	}	

	//try to open as *.wav
    /*SAFE_DELETE(m_pAudioCtrl->m_pIDecoder);
    m_pIDecoder=new CWavDecoder();
    ar=m_pAudioCtrl->m_pIDecoder->Open(lpFileName);
    if (ar==AR_OK)
        return AR_OK;*/
    return AR_ERROR_OPEN_FILE;


    
}

void CAudioCtrl::Free()
{
	SAFE_DELETE(m_pAudioCtrl->m_pIDecoder);
	CloseHandle(m_pAudioCtrl->m_hPlayThread); 
	SAFE_DELETE(m_pAudioCtrl);
}

ARESULT CAudioCtrl::Sync()
{
	if (m_pIDecoder)
		return m_pAudioCtrl->m_pIDecoder->Sync();
	else return AR_E_FAIL;
}

ARESULT CAudioCtrl::Play()
{
	m_pAudioCtrl->m_bToQuit = false;
	ARESULT aresult = m_pAudioCtrl->m_pIDecoder->Play();
	if (aresult != AR_OK)
		return aresult;
	
	//m_pAudioCtrl->m_hPlayThread = (HANDLE)_beginthreadex(NULL,0,m_pAudioCtrl->playThreadHelper,(LPVOID)m_pAudioCtrl,0,(unsigned int*)&m_pAudioCtrl->m_dPlayThreadID);

	if (FAILED(m_pAudioCtrl->m_hPlayThread))
		return atrace_error(L"Fail to create a thread", AR_ERROR_WHILE_CREATING_THREAD);
	
	//WaitForSingleObject(m_hPlayThread, INFINITE);  //wait for play thread to exit

	return AR_OK;
}

int CAudioCtrl::GetFullTime()
{
  if (m_pAudioCtrl->m_pIDecoder) 
	  return m_pAudioCtrl->m_pIDecoder->GetFullTime();
  else 
	  return -1;
}

int CAudioCtrl::GetCurTime()
{
    if (m_pAudioCtrl->m_pIDecoder) 
        return m_pAudioCtrl->m_pIDecoder->GetCurTime();
    else 
		return -1;
}

ARESULT CAudioCtrl::Pause()
{
	if (m_pAudioCtrl->m_pIDecoder)
		return m_pAudioCtrl->m_pIDecoder->Pause();
	else
		return AR_ERROR_WHILE_DECODING;
}

ARESULT CAudioCtrl::SetCurTime( int time )
{
	if (CAudioCtrl::isPlaying())
		m_pAudioCtrl->m_pIDecoder->SetCurTime(time);
    return AR_OK;
}

float CAudioCtrl::GetVolume()
{
	if (m_pIDecoder)		
		return m_pAudioCtrl->m_pIDecoder->GetVolume();
	else return 1.0f;
}

ARESULT CAudioCtrl::SetVolume(float theVolume)
{
	if (m_pAudioCtrl->m_pIDecoder)
		return m_pAudioCtrl->m_pIDecoder->SetVolume(theVolume);
	else return AR_E_FAIL;
}

ARESULT CAudioCtrl::Stop()
{
	if (m_pIDecoder)
		return m_pIDecoder->Stop();
	else
		return AR_OK;
}

ARESULT CAudioCtrl::Close()
{
	if (m_pIDecoder) {
		m_pAudioCtrl->m_bToQuit = true;  //let the thread exit
		return m_pIDecoder->Close();
	}
	else
		return AR_OK;
}

bool CAudioCtrl::checkExtension( LPCWSTR lpFileName,LPCWSTR lpExtName )
{   
    int lena=wcslen(lpFileName);
    int lenb=wcslen(lpExtName);
    if (lena<=lenb) return false;
    return StrCmpI(lpFileName+lena-lenb,lpExtName)==0; 
}

unsigned int WINAPI CAudioCtrl::playThreadHelper(PVOID param)
{
	CAudioCtrl * pto = static_cast<CAudioCtrl*>(param);
	return pto->playThread();
}

int CAudioCtrl::playThread()
{
	while (!m_bToQuit) {
        Sync();
        Sleep(20);  //don't sleep more than the time buffered(500ms currently);
		if (m_bToQuit)
			break;
		if (GetCurTime() >= GetFullTime()){
			Stop();
			//Close();   //the end of a song, close the file
			m_bToQuit = true;
		}
    }	
	return 1;
}



TAG CAudioCtrl::GetTag()
{
	if (m_pIDecoder)
		return m_pIDecoder->GetTag();
	else
		return TAG("Unknown","Unnamed","Unknown","0","","","");
}

 