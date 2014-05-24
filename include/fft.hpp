#ifndef __FASTFURIOUS8TRANSFORM
#define __FASTFURIOUS8TRANSFORM

#include "signal.hpp"
#include <algorithm>

class FFT
{
  public:
    FFT(unsigned int size);
    ~FFT();

    inline unsigned int size() {
      return _size;
    }

    //get nearest inferior power of 2
    static unsigned short getInfPow2(unsigned int v);

    //get nearest superior power of 2
    static unsigned short getSupPow2(unsigned int v);

    void compute(const sample* s, unsigned int size);
    void compute(const Signal &s);

    //Realloc 
    void realloc(unsigned int size);

    inline sample* get_real() {
      return _real;
    }
    inline sample* get_imaginary() {
      return _imaginary;
    }

    void compute_module();

  private:
    sample* _real;
    sample* _imaginary;
    sample* _module;
    unsigned int* _indexTable;
    sample* _twidleFactor;
    sample* _twidleFactorI;
    unsigned int _size;
    unsigned short _pow2;


};


#endif

