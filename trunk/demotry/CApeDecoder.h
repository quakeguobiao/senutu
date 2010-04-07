#pragma  once
#ifndef _CAPEDECODER_H_
#include "maclib\all.h"
#include "maclib\MACLib.h"
#include "IDecoder.h"
#include "XAudio2Manip.h"

class CApeDecoder : public IDecoder {
public :
    CApeDecoder() ;
    virtual ~CApeDecoder() ;

    virtual ARESULT Open( LPWSTR strFileName) ;
    //virtual ARESULT Play() ;
   // virtual ARESULT Pause() ;
    virtual ARESULT Stop() ;
    virtual ARESULT Sync(int minBufTime=500,int MaxBufTime=1000,int BufferSpan=DEFAULT_BUFFER_SPAN);
    virtual int GetFullTime() ;
    virtual int GetCurTime() ;
  //  virtual bool isPlaying() ;
  //  virtual bool isPause() ;
    virtual ARESULT SetCurTime(int time) ;
    virtual ARESULT Close() ;

	//virtual float GetVolume() { return 1.0f;}  //TBD
	//virtual ARESULT SetVolume(float theVolume){return AR_OK;}  //TBD


private : 

    ARESULT CApeDecoder::ResetFile();
    
    IAPEDecompress *m_pIAPE;
//    XAudio2Manip XACtrl;
   // WAVEFORMATEX *m_pwfx;
    
    //play status
  //  bool m_bIsPlaying;
  //  bool m_bIsPause;
 //   bool m_bIsStop;
    
};

#define _CAPEDECODER_H_ 
#endif //_CAPEDECODER_H_
