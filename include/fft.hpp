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

    inline sample* get_values() {
      return _values;
    }

  private:
    sample* _values;
    unsigned int* _indexTable;
    sample* _twidleFactor;
    unsigned int _size;
    unsigned short _pow2;


};


#endif

