#pragma once

#ifndef _AERROR_H
#define _AERROR_H

#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
#endif
//#include "all.h"
#include <windows.h>

#ifndef DEFAULT_BUFFER_SPAN
#define DEFAULT_BUFFER_SPAN 20
#endif //DEFAULT_BUFFER_SPAN

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif   
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

enum ARESULT{   
    AR_FAIL = -10000,//general fail,use specific fail if possible
	AR_ERROR_DECODER_NOT_INITIALIZED, // can't initialize a decoder 
    AR_NULL_STRING_ARG,//string ptr is null
    AR_ERROR_OPEN_FILE,//can't open file
	AR_ERROR_SRCVOICE,//error when initializing xaudio2 source voice
    AR_FORMAT_ERR,//the decoder don't support the file or the format is not supported right now
	AR_ERROR_WHILE_DECODING, //error while decoding
    AR_CO_E_NOTINITIALIZED,//error initialize xaudio2 system
	AR_E_FAIL,//the same as AR_FAIL
	AR_E_FAILTOSEEK,//seek file fail
	AR_E_WARNING,//some thing not severe
	AR_XA2_NOT_INITIALIZED,//try to play music before xaudio2manip::setformat
	AR_ERROR_WHILE_CREATING_THREAD, //fail to creat a thread
	
    AR_OK=0,//all right

};

//extern ARESULT atrace_error(LPCWSTR errstr,ARESULT ar) ;
#define atrace_error(a,b) atrace_error_IMP(a,b,__FILE__,__LINE__);
extern ARESULT atrace_error_IMP(LPCWSTR errstr,ARESULT ar,LPCSTR file,int line);
extern ARESULT atrace_error_IMP(const char * errstr , ARESULT ar, LPCSTR file, int line);


#endif //_AERROR_H