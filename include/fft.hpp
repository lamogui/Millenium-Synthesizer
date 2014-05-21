#ifndef __FASTFURIOUS8TRANSFORM
#define __FASTFURIOUS8TRANSFORM

#include "signal.hpp"

class FFT
{
  public:
    FFT(unsigned int size);
    ~FFT();

    inline unsigned int size() {
     return _size; //return size in bytes
    }
    
    //get nearest inferior power of 2
    static unsigned short getInfPow2(unsigned int v);

    void compute(const Signal&);

   //Realloc 
    void realloc(unsigned int size);

    


  private:
    sample* _values;
    unsigned int _size;
    unsigned short _pow2;
    

};


#endif
