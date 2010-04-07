
#include "CWavDecoder.h"
#include <dxerr.h>
#include <cstdio>
#include <iostream>

using namespace std;

CWavDecoder::CWavDecoder(void)
{
}

CWavDecoder::~CWavDecoder(void)
{
    
   // SAFE_DELETE(m_pwfx);
}

ARESULT CWavDecoder::Open( LPWSTR strFileName ) {
    ARESULT ar=AR_FAIL;
    HRESULT hr=-1;
    if( strFileName == NULL )
        return AR_NULL_STRING_ARG;
    //SAFE_DELETE_ARRAY( m_pwfx );

    m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );
    if(m_hmmio==NULL) {
        return AR_ERROR_OPEN_FILE;
    }

    if( FAILED( hr = ReadMMIO() ) )
    {
        // ReadMMIO will fail if its an not a wave file
        mmioClose( m_hmmio, 0 );
        return atrace_error( L"ReadMMIO",AR_FORMAT_ERR);
    }

    if( FAILED( hr = ResetFile() ) )
        return atrace_error( L"ResetFile", ar );

    // After the reset, the size of the wav file is m_ck.cksize so store it now
    m_dwSize = m_ck.cksize;
    //set XAudio2 to the current format
    _CrtDumpMemoryLeaks();
    setUpXA();
    _CrtDumpMemoryLeaks();
    return AR_OK;
}

ARESULT CWavDecoder::ResetFile() {
    if( m_hmmio == NULL )
        return AR_CO_E_NOTINITIALIZED;
    // Seek to the data
    if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof( FOURCC ),
        SEEK_SET ) )
        return atrace_error( L"mmioSeek", AR_E_FAIL );

    // Search the input file for the 'data' chunk.
    m_ck.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
    if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
        return atrace_error( L"mmioDescend", AR_E_FAIL );
    return AR_OK;
}

ARESULT CWavDecoder::ReadMMIO()
{
    MMCKINFO ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT pcmWaveFormat;  // Temp PCM structure to load in.

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
        return atrace_error( L"mmioDescend", AR_E_FAIL );

    // Check to make sure this is a valid wave file
    if( ( m_ckRiff.ckid != FOURCC_RIFF ) ||
        ( m_ckRiff.fccType != mmioFOURCC( 'W', 'A', 'V', 'E' ) ) )
        return atrace_error( L"mmioFOURCC", AR_E_FAIL );

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
        return atrace_error( L"mmioDescend", AR_E_FAIL );

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
    if( ckIn.cksize < ( LONG )sizeof( PCMWAVEFORMAT ) )
        return atrace_error( L"sizeof(PCMWAVEFORMAT)", AR_E_FAIL );

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, ( HPSTR )&pcmWaveFormat,
        sizeof( pcmWaveFormat ) ) != sizeof( pcmWaveFormat ) )
        return atrace_error( L"mmioRead", AR_E_FAIL );

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = ( WAVEFORMATEX* )new CHAR[ sizeof( WAVEFORMATEX ) ];
        if( NULL == m_pwfx )
            return atrace_error( L"m_pwfx", AR_E_FAIL);

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, ( CHAR* )&cbExtraBytes, sizeof( WORD ) ) != sizeof( WORD ) )
            return atrace_error( L"mmioRead", AR_E_FAIL );

        m_pwfx = ( WAVEFORMATEX* )new CHAR[ sizeof( WAVEFORMATEX ) + cbExtraBytes ];
        if( NULL == m_pwfx )
            return atrace_error( L"new", AR_E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, ( CHAR* )( ( ( BYTE* )&( m_pwfx->cbSize ) ) + sizeof( WORD ) ),
            cbExtraBytes ) != cbExtraBytes )
        {
            SAFE_DELETE( m_pwfx );
            return atrace_error( L"mmioRead", AR_E_FAIL );
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        SAFE_DELETE( m_pwfx );
        return atrace_error( L"mmioAscend", AR_E_FAIL );
    }

    return AR_OK;
}

//ARESULT CWavDecoder::Play()
//{
//    m_bIsPlaying=true;
//    m_bIsPause=false;
//    m_bIsStop=false;
//    return AR_OK;
//}

ARESULT CWavDecoder::setUpXA()
{ 
    XACtrl.SetFormat(m_pwfx);
    return AR_OK;
}

ARESULT CWavDecoder::Sync( int minBufTime/*=500*/,int MaxBufTime/*=1000*/ ,int BufferSpan/*=DEFAULT_BUFFER_SPAN*/)
{   
    
    if (m_bIsPlaying) {
        //cout<<XACtrl.bufferedTime()<<endl;
        while (XACtrl.bufferedTime()<minBufTime) {
            XAUDIO2_BUFFER *buffer = new XAUDIO2_BUFFER();
            DWORD done=0;
         
            DWORD toread = m_pwfx->nSamplesPerSec* m_pwfx->nBlockAlign * BufferSpan / 1000;//the time of 100ms data
            buffer->pAudioData=new BYTE[toread];
            Read(buffer->pAudioData,toread,&done);
            buffer->AudioBytes = done;
            if (done==toread) {
                XACtrl.SubmitSourceBuffer(buffer);   
                
                
            }else {
                SAFE_DELETE_XABUFFER(buffer);
                Stop();
                break;
            }
        }
    } else {
        //XACtrl.FlushSourceBuffers();
    }
    return AR_OK;
}

HRESULT CWavDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
    MMIOINFO mmioinfoIn; // current status of m_hmmio

    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;
    if( pBuffer == NULL || pdwSizeRead == NULL )
        return E_INVALIDARG;

    *pdwSizeRead = 0;

    if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
        return DXTRACE_ERR( L"mmioGetInfo", E_FAIL );

    UINT cbDataIn = dwSizeToRead;
    if( cbDataIn > m_ck.cksize )
        cbDataIn = m_ck.cksize;

    m_ck.cksize -= cbDataIn;

    for( DWORD cT = 0; cT < cbDataIn; cT++ )
    {
        // Copy the bytes from the io to the buffer.
        if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
        {
            if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
                return DXTRACE_ERR( L"mmioAdvance", E_FAIL );

            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
                return DXTRACE_ERR( L"mmioinfoIn.pchNext", E_FAIL );
        }

        // Actual copy.
        *( ( BYTE* )pBuffer + cT ) = *( ( BYTE* )mmioinfoIn.pchNext );
        mmioinfoIn.pchNext++;
    }

    if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
        return DXTRACE_ERR( L"mmioSetInfo", E_FAIL );

    *pdwSizeRead = cbDataIn;

    return S_OK;
}

int CWavDecoder::GetFullTime()
{
    return m_dwSize / m_pwfx->nBlockAlign * 1000 / m_pwfx->nSamplesPerSec;
}

int CWavDecoder::GetCurTime()
{
    int ret= mmioSeek(m_hmmio,0,SEEK_CUR);
    ret=ret / m_pwfx->nBlockAlign  * 1000 / m_pwfx->nSamplesPerSec;
    ret-=XACtrl.bufferedTime();
    return ret;
}

//ARESULT CWavDecoder::Pause()
//{
//    XACtrl.FlushSourceBuffers();
//    if (m_bIsStop) return AR_OK;
//    m_bIsPause=true;
//    m_bIsStop=false;
//    m_bIsPlaying=false;
//    return AR_OK;
//}


//bool CWavDecoder::isPlaying()
//{
//    return m_bIsPlaying;
//}
//
//bool CWavDecoder::isPause()
//{
//    return m_bIsPause;
//}

ARESULT CWavDecoder::SetCurTime( int time )
{   
    time = max(0,time);
    time = min(GetFullTime(),time);
    INT64 offset=time;
    offset=offset*m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec / 1000;
    
    XACtrl.Stop();
    XACtrl.FlushSourceBuffers();
    
    mmioSeek(m_hmmio,(LONG)offset,SEEK_SET);
    XACtrl.Start();
    return AR_OK;
    
}

ARESULT CWavDecoder::Stop()
{   

    m_bIsStop=true;
    m_bIsPlaying=false;
    m_bIsPause=false;
    XACtrl.FlushSourceBuffers();
    ResetFile();
    
    return AR_OK;
}

ARESULT CWavDecoder::Close()
{
  XACtrl.Stop();
  XACtrl.FlushSourceBuffers();
  mmioClose(m_hmmio,0);
  return AR_OK;
}