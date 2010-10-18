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

string IDecoder::MilliSecondToString(int ms)
{
	int totalTime = GetFullTime();
	int hour = totalTime / 3600000;
	int minute = totalTime / 60000;
	int second = totalTime / 1000 - hour * 3600 - minute * 60;
	char result[10];
	if (hour > 0)
		sprintf(result,"%d%s%2d%s%2d",hour,":",minute,":",second);
	else
		sprintf(result,"%d%s%2d",minute,":",second);
	return string(result);
}
