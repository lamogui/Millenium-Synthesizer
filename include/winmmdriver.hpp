/****************************************************************************
Nom ......... : winmmdriver.hpp
Role ........ : Contient le wrapper pour winmm
Auteur ...... : Julien DE LOOR
Version ..... : V2.0
Licence ..... : © Copydown™
****************************************************************************/

#ifndef ____WINMMMMMMMM
#define ____WINMMMMMMMM

#include "config.hpp"
#include "audiodriver.hpp"
#include "audiostream.hpp"

#ifdef COMPILE_WINDOWS


#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

class WinmmDriver : public DirectAudioStream
{
  public:
    WinmmDriver();
    virtual ~WinmmDriver();
    bool init(unsigned int rate);
    void free();

    //Push a signal block into driver buffers
    bool pushStereoSignal(const Signal& left,const Signal& right);  
    
    //Return samples which can be pushed into the stream (at least an estimation)
    unsigned int getFreeSamplesCount();
    
  protected:
    /*Win32 end block callback*/
    static void CALLBACK _waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);
    WAVEHDR* _waveBlocks;
    HWAVEOUT _hWaveOut;
    unsigned int _nbBlocks;
    CRITICAL_SECTION _waveCriticalSection;
    volatile unsigned int _waveFreeBlockCount;
    unsigned int _waveCurrentBlock;
};


#endif
#endif