/****************************************************************************
Nom ......... : winmmdriver.cpp
Role ........ : Contient le wrapper pour winmm
Auteur ...... : Julien DE LOOR
Version ..... : V2.0
Licence ..... : © Copydown™
****************************************************************************/
#include "winmmdriver.hpp"

#ifdef COMPILE_WINDOWS
#include <iostream>

WinmmDriver::WinmmDriver():
_waveBlocks(0),
_hWaveOut(0),
_nbBlocks(0),
_waveFreeBlockCount(0),
_waveCurrentBlock(0)
{
}

WinmmDriver::~WinmmDriver()
{
  this->free();
}

bool WinmmDriver::init(unsigned int rate)
{
  if (_hWaveOut) {
    std::cerr << "WinMM already initialized !" << std::endl;
    return true;
  }
  WAVEFORMATEX wfx;
  wfx.nSamplesPerSec = rate; /* sample rate */
  wfx.wBitsPerSample = 16; /* sample size */
  wfx.nChannels= 2; /* channels*/
  wfx.cbSize = 0; /* size of _extra_ info */
  wfx.wFormatTag = WAVE_FORMAT_PCM;
  wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
  wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

  if(waveOutOpen(
          &_hWaveOut, 
          WAVE_MAPPER, 
          &wfx, 
          (DWORD_PTR)&_waveOutProc, 
          (DWORD_PTR) this, 
          CALLBACK_FUNCTION
          ) != MMSYSERR_NOERROR)
    {
      std::cerr << "Failed to initialize WINMM driver !" << std::endl;
      return false;
    }
  InitializeCriticalSection(&_waveCriticalSection);
  std::cout << "WINMM Initialized with sample rate " << rate << "Hz  (" << wfx.nAvgBytesPerSec << " Bytes/sec)" << std::endl;
  unsigned char* buffer;
  _nbBlocks=3;
  _waveFreeBlockCount=_nbBlocks;
  _waveCurrentBlock=0;
  DWORD totalBufferSize = (Signal::size*4 + sizeof(WAVEHDR)) * _nbBlocks;
  /*
   * allocate memory for the entire set in one go
   */
  buffer = (unsigned char*) HeapAlloc(GetProcessHeap(), 
                                      HEAP_ZERO_MEMORY, 
                                      totalBufferSize); 

  /*
   * UGLY !!!
   */
  _waveBlocks = (WAVEHDR*)buffer;
  buffer += sizeof(WAVEHDR) * _nbBlocks;
  for(unsigned int i = 0; i < _nbBlocks; i++) {
    _waveBlocks[i].dwBufferLength = Signal::size*4;
    _waveBlocks[i].lpData = (LPSTR)buffer;
    buffer += Signal::size*4;
  }
  return true;
}

void WinmmDriver::free()
{
  if (_waveBlocks) {
    while (1) {
      EnterCriticalSection(&_waveCriticalSection); //Mutex Locking
      if (_waveFreeBlockCount==_nbBlocks) {
        break;
      }
      LeaveCriticalSection(&_waveCriticalSection);
      Sleep(10);
    }
    for(unsigned int i = 0; i < _nbBlocks; i++) {
      if(_waveBlocks[i].dwFlags & WHDR_PREPARED)
        waveOutUnprepareHeader(_hWaveOut, &_waveBlocks[i], sizeof(WAVEHDR));
    }
    HeapFree(GetProcessHeap(), 0, _waveBlocks);
    _waveBlocks=0;
    
  }
  if (_hWaveOut){
    waveOutClose(_hWaveOut);
    _hWaveOut=0;
    DeleteCriticalSection(&_waveCriticalSection); //Delete mutex
  }
  _nbBlocks=0;
  _waveFreeBlockCount=0;
  _waveCurrentBlock=0;
}

unsigned int WinmmDriver::getFreeSamplesCount()
{
  EnterCriticalSection(&_waveCriticalSection); //Mutex Locking
  unsigned int r=_waveFreeBlockCount*Signal::size*2;
  LeaveCriticalSection(&_waveCriticalSection);
  return r;
}

bool WinmmDriver::pushStereoSignal(const Signal& left,const Signal& right)
{
  EnterCriticalSection(&_waveCriticalSection); //Mutex Locking
  if (!_waveFreeBlockCount) {
    LeaveCriticalSection(&_waveCriticalSection);
    return false;
  }
  _waveFreeBlockCount--;
  LeaveCriticalSection(&_waveCriticalSection);
  
  WAVEHDR* current = &_waveBlocks[_waveCurrentBlock];
  if(current->dwFlags & WHDR_PREPARED) 
      waveOutUnprepareHeader(_hWaveOut, current, sizeof(WAVEHDR)); 
      
  
  short* p = (short*)current->lpData;
  
  for (unsigned int i=0; i < Signal::size; i++)
  {
    *p++ = (short)(left.samples[i]*MULTIPLIER_16);
    *p++ = (short)(right.samples[i]*MULTIPLIER_16);
  }
  
  current->dwBufferLength = Signal::size*4;
  
  waveOutPrepareHeader(_hWaveOut, current, sizeof(WAVEHDR));
  waveOutWrite(_hWaveOut, current, sizeof(WAVEHDR));
  
  
  _waveCurrentBlock++;
  _waveCurrentBlock %= _nbBlocks;
  return true;
} 

void CALLBACK WinmmDriver::_waveOutProc(HWAVEOUT, UINT uMsg, DWORD dwInstance, DWORD, DWORD)
{
  WinmmDriver* a = (WinmmDriver*)dwInstance;
  if(uMsg != WOM_DONE)
    return;
  EnterCriticalSection(&(a->_waveCriticalSection)); //Mutex Locking
  a->_waveFreeBlockCount++;
  LeaveCriticalSection(&(a->_waveCriticalSection));
}

#endif
