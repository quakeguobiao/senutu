#include "aerror.h"
#include <iostream>

//#include "debug_new.h" // +

using namespace std;

ARESULT atrace_error_IMP(LPCWSTR errstr,ARESULT ar,LPCSTR file,int line) {
	#ifdef _DEBUG
		if(ar!=AR_OK) {
			cout<< endl <<file<< ":"<<line;
			wcout<<"{"<<errstr<<"}"<<endl;
		}
	#endif	
    return ar;
}

ARESULT atrace_error_IMP(const char * errstr , ARESULT ar, LPCSTR file, int line){
	#ifdef _DEBUG
		if(ar != AR_OK) {
			cout<<endl << file<<":"<<line;
			cout<<"{"<<errstr<<"}"<<endl;
		}
	#endif	
    return ar;
}
