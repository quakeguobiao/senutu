#include "CFfmpegDecoder.h"

#include <iostream>
#include "debug_new.h" // +


 
#include "atlbase.h"
#include "atlstr.h"
//#include "comutil.h"

using namespace std;

CFfmpegDecoder::CFfmpegDecoder(void) 
    :nAudioStream(-1),pFormatContext(NULL),pCodeContext(NULL),pCodec(NULL),startTimeStamp(0)
{
    memset(&packet,0,sizeof packet);
    SAFE_DELETE(m_pwfx);
    m_pwfx=new WAVEFORMATEX();
    av_register_all();
}

 
CFfmpegDecoder::~CFfmpegDecoder(void)
{
    this->Close();
    SAFE_DELETE(m_pwfx);
}

ARESULT CFfmpegDecoder::Open( LPWSTR strFileName )
{
    
    CW2A pFileName(strFileName);

     
    
    if (av_open_input_file (&pFormatContext,pFileName , NULL, 0, NULL) != 0)   //打开一个文件
    {
        return atrace_error("",AR_ERROR_OPEN_FILE);
        
    }
    if (av_find_stream_info (pFormatContext) < 0)  //获得一个流信息
    {
        return atrace_error("Error finding stream in a ffmpeg file",AR_ERROR_OPEN_FILE);
    }
#ifdef _DEBUG
    //display file info on screen
    
    dump_format (pFormatContext, 0, pFileName, false);
#endif
    nAudioStream = -1;
    for (int i = 0; i < (int)pFormatContext->nb_streams; i++) //查找一个音频流,
    {
        if (pFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)    //////// 
        { 
            nAudioStream = i; 
            break; 
        } 
    }
    if (nAudioStream==-1)
        return atrace_error("No Audio Stream",AR_ERROR_OPEN_FILE);
    
//     int nVideoStream = -1;
//     for (int i = 0; i < (int)pFormatContext->nb_streams; i++) //查找一个音频流,
//     {
//         if (pFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)    //////// 
//         { 
//             nVideoStream = i; 
//             break; 
//         } 
//     }
//     if (nVideoStream != -1) 
    
    pCodeContext = pFormatContext->streams[nAudioStream]->codec; //将音频流的传递给AVCodeContext结构体的变量,为以后使用做准备
     
    //播放时间设为0ms
    av_seek_frame(pFormatContext,nAudioStream,0,0);
   
	//set up m_pwfx , output is 16bit PCM
    m_pwfx->nChannels=pCodeContext->channels;
    m_pwfx->nSamplesPerSec=pCodeContext->sample_rate;
    m_pwfx->wBitsPerSample=16;
    m_pwfx->wFormatTag=WAVE_FORMAT_PCM;
    m_pwfx->nBlockAlign=m_pwfx->nChannels*16/8;
    m_pwfx->nAvgBytesPerSec=m_pwfx->nSamplesPerSec*m_pwfx->nBlockAlign;
    
    XACtrl.SetFormat(m_pwfx);//change the output format
    
    //已经准备好了,下面需要将音频进行解码,首先,找到一个解码器
    pCodec = avcodec_find_decoder(pCodeContext->codec_id);  
    if(!pCodec)
    {
        return atrace_error("Unsupported Codec!",AR_ERROR_OPEN_FILE);
    }
    
    if (avcodec_open(pCodeContext, pCodec)< 0 )
        return atrace_error("not supported codec",AR_FORMAT_ERR);
    return AR_OK;
    
}

ARESULT CFfmpegDecoder::Stop()
{
    if (m_bIsStop) 
        return AR_OK;
    m_bIsStop=true;
    m_bIsPlaying=false;
    m_bIsPause=false;
    
    XACtrl.FlushSourceBuffers();
    av_seek_frame(pFormatContext,nAudioStream,0,0);
    
    XACtrl.Start();
    return AR_OK;

}

ARESULT CFfmpegDecoder::Sync( int minBufTime/*=500*/,int MaxBufTime/*=1000*/,int BufferSpan/*=DEFAULT_BUFFER_SPAN*/ )
{   
    minBufTime=1500;MaxBufTime=3000;
    if (m_bIsPlaying) {
        //while(XACtrl.bufferedTime() < minBufTime) {
        while((double)XACtrl.bufferedBytes() * 1000 / m_pwfx->nAvgBytesPerSec  <minBufTime) {
            if (av_read_frame(pFormatContext, &packet)>=0) {
                if (packet.stream_index!=nAudioStream) {
                    av_free_packet(&packet);
                    continue;
                }
                XAUDIO2_BUFFER *buffer=NULL;
                buffer=new XAUDIO2_BUFFER();

                buffer->pAudioData=new BYTE[AVCODEC_MAX_AUDIO_FRAME_SIZE];
                int done=AVCODEC_MAX_AUDIO_FRAME_SIZE;
                if (avcodec_decode_audio2(pCodeContext,(int16_t *)buffer->pAudioData,&done
                    ,packet.data,packet.size) <= 0 ) {
                      atrace_error("glitch while decoding",AR_ERROR_WHILE_DECODING);
                      SAFE_DELETE_XABUFFER(buffer);
                      av_free_packet(&packet);
                      continue;
                    }
                if (done) {
                    buffer->AudioBytes=done;
                    resizeArray(buffer->pAudioData,AVCODEC_MAX_AUDIO_FRAME_SIZE,done);
                    av_free_packet(&packet);
                    XACtrl.SubmitSourceBuffer(buffer); 
                }
            }
            else {                
                if (XACtrl.bufferedTime()==0)
                    this->Stop();
                break;
            }

        }//while 
        XACtrl.SubmitSourceBuffer(NULL); //let the xactrl play ,instead of be trapped
    }//if
    return AR_OK;
}

int CFfmpegDecoder::GetFullTime() /*= 0*/
{
    
    return (int)(pFormatContext->streams[nAudioStream]->duration 
        * av_q2d(pFormatContext->streams[nAudioStream]->time_base) * 1000);
}

int CFfmpegDecoder::GetCurTime() /*= 0*/
{
    static int lastret=0;
    int ret= (int) ((pFormatContext->streams[nAudioStream]->cur_dts - startTimeStamp) 
        * av_q2d(pFormatContext->streams[nAudioStream]->time_base) *1000) ;
    ret-=XACtrl.bufferedBytes() * 1000 / m_pwfx->nAvgBytesPerSec ;
    
    if (ret<0)
        return lastret;
    else {
        lastret=ret;
        return ret;
    }
}

ARESULT CFfmpegDecoder::SetCurTime( int time ) /* = 0*/
{
    ARESULT ar=AR_E_FAILTOSEEK;
    int64_t timestamp=int64_t(time / av_q2d(pFormatContext->streams[nAudioStream]->time_base) /1000);
    timestamp=max(startTimeStamp,timestamp);
    timestamp=min(timestamp,pFormatContext->streams[nAudioStream]->duration);
    XACtrl.Stop();
    XACtrl.FlushSourceBuffers();
    int ret=av_seek_frame(pFormatContext,nAudioStream,timestamp,AVSEEK_FLAG_BACKWARD);
    if (ret>=0)
        ar= AR_OK;
    /*
    ret=av_seek_frame(pFormatContext,nAudioStream,timestamp,AVSEEK_FLAG_BACKWARD);
        if (ret>=0)
            ar= AR_OK;
            
        ret=av_seek_frame(pFormatContext,nAudioStream,timestamp,AVSEEK_FLAG_ANY);
        if (ret>=0)
            ar= AR_OK;
        */
        
    XACtrl.Start();
    if (ar!=AR_OK) {
        return atrace_error("seek error",ar);
    }
    return ar;
}

ARESULT CFfmpegDecoder::Close() /* = 0*/
{
    this->Stop();
    if (pFormatContext) {
        av_close_input_file(pFormatContext);
        pFormatContext=NULL;
        //SAFE_DELETE(pFormatContext);
        
    }
    
    return AR_OK;
}

int CFfmpegDecoder::resizeArray( BYTE *&array,int fromSize,int toSize )
{
    BYTE * newptr=new BYTE[toSize];
    memcpy(newptr,array,toSize);
    SAFE_DELETE(array);
    array=newptr;
    return 0;
}