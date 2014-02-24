#ifndef __SIGNAL
#define __SIGNAL

class Signal
{
   public:
      Signal();
      Signal(unsigned int size, unsigned int frequency);
      Signal(const Signal& s);
      virtual ~Signal();
      
      
      bool isReady();
      
      
   protected:
      unsigned int _size;      //taille des buffers
      unsigned int _frequency; //fréquence d'echantillonage
      
      float* _buffer1;         //double buffering
      float* _buffer2;         
      float* _currentBuffer;
      
};


#endif