#include "CApeDecoder.h"

#include <iostream>
//#include "debug_new.h" // +


CApeDecoder::CApeDecoder()
{
    m_pIAPE=NULL;
    m_pwfx=NULL;
}

CApeDecoder::~CApeDecoder(void)
{
   SAFE_DELETE(m_pIAPE);
}

ARESULT CApeDecoder::Open( LPWSTR strFileName )
{
    //this->Close();
    //SAFE_DELETE(m_pIAPE);
    
    int nRetVal;
    m_pIAPE=CreateIAPEDecompress(strFileName,&nRetVal);
    if (nRetVal!=0) {
        return AR_FORMAT_ERR;
    }
    SAFE_DELETE(m_pwfx);
    m_pwfx=new WAVEFORMATEX;
    m_pIAPE->GetInfo(APE_INFO_WAVEFORMATEX,(int)m_pwfx);
    XACtrl.SetFormat(m_pwfx);

    return AR_OK;
}


ARESULT CApeDecoder::ResetFile() {
    
    if (m_pIAPE->Seek(0)==0)
        return AR_OK;
    else return AR_FAIL;
}
// ARESULT CApeDecoder::Play()
//{
//    m_bIsPlaying=true;
//    m_bIsPause=false;
//    m_bIsStop=false;
//    return AR_OK;
//} 

ARESULT CApeDecoder::Sync( int minBufTime/*=500*/,int MaxBufTime/*=1000*/ ,int BufferSpan/*=DEFAULT_BUFFER_SPAN*/)
{   

    if (m_bIsPlaying) {
        //cout<<XACtrl.bufferedTime()<<endl;
        while (XACtrl.bufferedTime()<minBufTime) {
            XAUDIO2_BUFFER *buffer = new XAUDIO2_BUFFER();
           // memset(&buffer,0,sizeof(XAUDIO2_BUFFER));
            int done=0;

            DWORD blockToRead = m_pwfx->nSamplesPerSec * BufferSpan / 1000;//the time of |bufferspan| ms data
            buffer->pAudioData=new BYTE[blockToRead*m_pwfx->nBlockAlign];
            m_pIAPE->GetData((char*)buffer->pAudioData,blockToRead,&done);
            //Read(buffer->pAudioData,toread,&done);
            buffer->AudioBytes = done*m_pwfx->nBlockAlign;
            if (done>0) {
                XACtrl.SubmitSourceBuffer(buffer);   
            }else {
                SAFE_DELETE_XABUFFER(buffer);
                if (XACtrl.bufferedTime()==0) {
                    //cout<<this->GetCurTime()<<"\t\n";
                    Stop();
                }
                break;
            }
        }
    } else {
        //XACtrl.FlushSourceBuffers();
    }
    return AR_OK;
}
 

int CApeDecoder::GetFullTime()
{
    return m_pIAPE->GetInfo(APE_INFO_LENGTH_MS);
}

int CApeDecoder::GetCurTime()
{
    int ret=m_pIAPE->GetInfo(APE_DECOMPRESS_CURRENT_MS);
    ret-=XACtrl.bufferedTime();
    
    return ret;
}

//ARESULT CApeDecoder::Pause()
//{
//    XACtrl.FlushSourceBuffers();
//    if (m_bIsStop) return AR_OK;
//    m_bIsPause=true;
//    m_bIsStop=false;
//    m_bIsPlaying=false;
//    return AR_OK;
//}


//bool CApeDecoder::isPlaying()
//{
//    return m_bIsPlaying;
//}
//
//bool CApeDecoder::isPause()
//{
//    return m_bIsPause;
//}

ARESULT CApeDecoder::SetCurTime( int time )
{   
    time = max(0,time);
    time = min(GetFullTime(),time);
    INT64 nBlock = (INT64)time * m_pwfx->nSamplesPerSec /1000;

    XACtrl.Stop();
    XACtrl.FlushSourceBuffers();

    m_pIAPE->Seek((int)nBlock);
    
    XACtrl.Start();
    return AR_OK;

}

ARESULT CApeDecoder::Stop()
{   

    m_bIsStop=true;
    m_bIsPlaying=false;
    m_bIsPause=false;
    XACtrl.FlushSourceBuffers();
    ResetFile();

    return AR_OK;
}

ARESULT CApeDecoder::Close()
{
    XACtrl.Stop();
    XACtrl.FlushSourceBuffers();
    SAFE_DELETE(m_pIAPE);
    return AR_OK;
}
//helper function 
static const char * safeGetTagField(CAPETag* pAPETag,const str_utf16 * fieldName) {
	if (pAPETag->GetTagField(fieldName))
		return pAPETag->GetTagField(fieldName)->GetFieldValue();
	else return "";
}

TAG CApeDecoder::GetTag()
{  
	CAPETag * pAPETag = (CAPETag *) m_pIAPE->GetInfo(APE_INFO_TAG);
	TAG retval;
	retval.Album = safeGetTagField(pAPETag,L"album");
	retval.Artist= safeGetTagField(pAPETag,L"artist");
	retval.Comment = safeGetTagField(pAPETag,L"comment");
	retval.Genre = safeGetTagField(pAPETag,L"genre");
	retval.Title = safeGetTagField(pAPETag,L"title");
	retval.Year = safeGetTagField(pAPETag,L"date");
	retval.TotalTime = GetFullTimeSerialized();
	return retval;
}

// string CApeDecoder::GetFullTimeSerialized()
// {
// 	int totalTime = GetFullTime();
// 	int hour = totalTime / 3600000;
// 	int minute = totalTime / 60000;
// 	int second = totalTime / 1000 - hour * 3600 - minute * 60;
// 	char result[10];
// 	if (hour > 0)
// 		sprintf(result,"%d%s%d%s%d",hour,":",minute,":",second);
// 	else
// 		sprintf(result,"%d%s%d",minute,":",second);
// 	return string(result);
// }
// 
// string CApeDecoder::GetCurTimeSeralized()
// {
// 
// 
// }
