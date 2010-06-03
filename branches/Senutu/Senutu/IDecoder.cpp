#include "IDecoder.h"

float IDecoder::GetVolume()
{
	return XACtrl.GetVolume();
}

ARESULT IDecoder::SetVolume(float theVolume)
{
	return XACtrl.SetVolume(theVolume);
}


ARESULT IDecoder::Play()
{
	m_bIsPlaying = true;
    m_bIsPause = false;
    m_bIsStop = false;
    return AR_OK;
}

ARESULT IDecoder::Pause()
{   
    
    XACtrl.Stop();
    this->SetCurTime(this->GetCurTime());//将buffer造成的误差抹去
	XACtrl.FlushSourceBuffers();
	
	XACtrl.Start();
    if (m_bIsStop)
		return AR_OK;
    m_bIsPause = true;
    m_bIsStop = false;
    m_bIsPlaying = false;
    return AR_OK;
}