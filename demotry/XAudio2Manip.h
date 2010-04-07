#pragma once
#ifndef XAUDIO2_H
#define XAUDIO2_H 

#include <XAudio2.h>
#include <deque>
#include "aerror.h"
using namespace std;

extern void SAFE_DELETE_XABUFFER(XAUDIO2_BUFFER *&buffer);

class XAudio2Manip{
public :
  XAudio2Manip();
  ~XAudio2Manip();
  /************************************************************************/
  /* set output format to |pwfx| */
  /************************************************************************/
  ARESULT SetFormat(WAVEFORMATEX *pwfx);
  /************************************************************************/
  /* must *not* delete submited buffer by hand, this class will handle this 
  inside automatically inside*/
  /************************************************************************/
  ARESULT SubmitSourceBuffer(XAUDIO2_BUFFER *buffer);
  /*ARESULT GetState(XAUDIO2_VOICE_STATE *pVoiceState);
  bool Full();*/
  ARESULT FlushSourceBuffers();
  int bufferedTime();
  void Start() {
	  if (m_pSourceVoice)
		m_pSourceVoice->Start(0);
  }
  void Stop() {
	if (m_pSourceVoice)
		m_pSourceVoice->Stop();
  }
  
  float GetVolume();
  ARESULT SetVolume(float theVolume);
  UINT64 bufferedBytes() {
    int ret=0;
    for (int i = 0;i<m_qBufferToPlay.size();i++)
        ret+=m_qBufferToPlay[i]->AudioBytes;
    for (int i = 0;i<m_qBufferToRelease.size();i++)
        ret+=m_qBufferToRelease[i]->AudioBytes;
    return ret;
  }
  UINT64 GetSamplesPlayed() { return ui64_SamplesPlayed;}
  
protected :
      IXAudio2 * m_pXAudio2;//main xaudio2
      IXAudio2MasteringVoice * m_pMasteringVoice;
      IXAudio2SourceVoice * m_pSourceVoice;
      deque<XAUDIO2_BUFFER *>m_qBufferToPlay;//buffer to be played
      deque<XAUDIO2_BUFFER *>m_qBufferToRelease;//buffers to be released
	  UINT64 ui64_SamplesPlayed;
     
};

#endif //XAUDIO2_H