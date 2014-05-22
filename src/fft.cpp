#include "fft.hpp" 
#include <iostream>

FFT::FFT(unsigned int size) :
 _values(NULL),
  _indexTable(0),
 _size(0),
 _pow2(FFT::getSupPow2(size)+1)
{
  _size=1<<_pow2;
  this->realloc(size);
}

FFT::~FFT() {
  if (_values) {
    free(_values);
    free(_indexTable);
  }
}

unsigned short FFT::getInfPow2(unsigned int v)
{
  unsigned short bit=0;
  for (unsigned int i=0; i < sizeof(unsigned int) << 3; i++)
  {
    if (v & 1) bit = i; v >>=1; 
  }
  return bit;
}

unsigned short FFT::getSupPow2(unsigned int v)
{
  unsigned short bit=0;
  for (unsigned int i=0; i < sizeof(unsigned int) << 3; i++)
  {
    if (v & 1) bit = i; v >>=1; 
  }
  if (((unsigned)1<<bit)==v)
    return bit;
  else
    return bit+1;
}

void FFT::realloc(unsigned int size) {
  _pow2=getSupPow2(size)+1;
  if (_pow2) {
    _size = 1 << _pow2;
    _values=(sample*)std::realloc((void*)_values,_size*sizeof(float));
    _indexTable=(unsigned int*)std::realloc((void*)_indexTable,
                                            _size*sizeof(unsigned int));
  }
  for (unsigned int i=0; i<_size;i++) {
    _values[i]=0;
  }
  if (_pow2) {
    for (unsigned int j=0; j<_size; j++) {
      unsigned int g_init=0, g_fin=_pow2, index=0;
      unsigned int i_delta=_pow2;
      for (unsigned int i=0; i<(unsigned)(_pow2>>1); i++) {
        unsigned int b1=0,b2=0;
        b1=(j&(1<<g_init))<<i_delta;
        b2=(j&(1<<g_fin))>>i_delta;
        index=index|(b1|b2);
        g_init++;
        g_fin++;
        i_delta-=2;
      }
      _indexTable[j]=index;
      std::cout << _indexTable[j] << std::endl;
    }
  }
}

void FFT::compute(const Signal& s) {
  for (unsigned int i=0; i<_size; i++) {
    _values[i<<1]=s.samples[i];
  }

  for (unsigned int i=0; i<_size<<1; i++) {
  }
}
