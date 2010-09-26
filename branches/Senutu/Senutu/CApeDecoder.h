#pragma  once
#ifndef _CAPEDECODER_H_
#define _CAPEDECODER_H_ 
#include "maclib\all.h"
#include "maclib\MACLib.h"
#include "maclib\APETag.h"

#include "IDecoder.h"
#include "XAudio2Manip.h"
#include <string>
using namespace std;

class CApeDecoder : public IDecoder {
public :
    CApeDecoder() ;
    virtual ~CApeDecoder() ;

    virtual ARESULT Open( LPWSTR strFileName) ;
     
    virtual ARESULT Stop() ;
    virtual ARESULT Sync(int minBufTime=500,int MaxBufTime=1000,int BufferSpan=DEFAULT_BUFFER_SPAN);
    virtual int GetFullTime() ;
    virtual int GetCurTime() ;

	//virtual string GetFullTimeSerialized() ;
	//virtual string GetCurTimeSeralized() ;
   
    virtual ARESULT SetCurTime(int time) ;
	virtual TAG GetTag();
    virtual ARESULT Close() ;


private : 

    ARESULT ResetFile();
    
    IAPEDecompress *m_pIAPE;
     
};


#endif //_CAPEDECODER_H_
