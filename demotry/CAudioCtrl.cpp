#include <shlwapi.h>

#include "CAudioCtrl.h"
#include "CWavDecoder.h"
#include "CMp3Decoder.h"
#include "CApeDecoder.h"
#include "CFlacDecoder.h"
#include "CFfmpegDecoder.h"


CAudioCtrl::CAudioCtrl()
{
   m_pIDecoder=NULL;
  // m_pIDecoder= new CMp3Decoder();
	//m_pIDecoder = new CFlacDecoder;   //for debug
}

ARESULT CAudioCtrl::Open( LPWSTR lpFileName )
{  
    ARESULT ar=AR_OK;
 

	
	
    
	
	//try to open as *.wav
    SAFE_DELETE(m_pIDecoder);
    m_pIDecoder=new CWavDecoder();
    ar=m_pIDecoder->Open(lpFileName);
    if (ar==AR_OK)
        return AR_OK;

	//try to open as *.ape
	SAFE_DELETE(m_pIDecoder);    
    m_pIDecoder=new CApeDecoder;
    ar=m_pIDecoder->Open(lpFileName);
    if (ar==AR_OK)
		return AR_OK;

	//try to open as *.flac
	SAFE_DELETE(m_pIDecoder);    
    m_pIDecoder=new CFlacDecoder();
    ar=m_pIDecoder->Open(lpFileName);
    if (ar==AR_OK)
        return AR_OK;

    
    if (checkExtension(lpFileName,L"mp3")) {
        //try to open as *.mp3
        SAFE_DELETE(m_pIDecoder);    
        m_pIDecoder=new CMp3Decoder();
        ar=m_pIDecoder->Open(lpFileName);
        if (ar==AR_OK)
            return AR_OK;
    }

    //try to open as *.wma ( or other formats ffmpeg supports)
    SAFE_DELETE(m_pIDecoder);
    m_pIDecoder=new CFfmpegDecoder();
    ar=m_pIDecoder->Open(lpFileName);
    if (ar==AR_OK)
        return AR_OK;

	

    
    
    
    
    return AR_ERROR_OPEN_FILE;

    
}

CAudioCtrl::~CAudioCtrl()
{
    SAFE_DELETE(m_pIDecoder);
}


ARESULT CAudioCtrl::Sync()
{
    return m_pIDecoder->Sync();
}

ARESULT CAudioCtrl::Play()
{
    return m_pIDecoder->Play();
}

int CAudioCtrl::GetFullTime()
{
  if (m_pIDecoder) 
    return m_pIDecoder->GetFullTime();
  else return -1;
}

int CAudioCtrl::GetCurTime()
{
    if (m_pIDecoder) 
        return m_pIDecoder->GetCurTime();
    else return -1;
}

ARESULT CAudioCtrl::Pause()
{
    return m_pIDecoder->Pause();
}

ARESULT CAudioCtrl::SetCurTime( int time )
{
    m_pIDecoder->SetCurTime(time);
    return AR_OK;
}
float CAudioCtrl::GetVolume()
{
	return m_pIDecoder->GetVolume();
}

ARESULT CAudioCtrl::SetVolume(float theVolume)
{
	return m_pIDecoder->SetVolume(theVolume);
}



ARESULT CAudioCtrl::Stop()
{
    return m_pIDecoder->Stop();
}

ARESULT CAudioCtrl::Close()
{
    return m_pIDecoder->Close();
}

bool CAudioCtrl::checkExtension( LPCWSTR lpFileName,LPCWSTR lpExtName )
{   
    int lena=wcslen(lpFileName);
    int lenb=wcslen(lpExtName);
    if (lena<=lenb) return false;
    return StrCmpI(lpFileName+lena-lenb,lpExtName)==0; 
}