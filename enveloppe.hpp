#ifndef ENVELOPPE_H
#define ENVELOPPE_H

#include "signal.hpp"

class Enveloppe : public AbstractSignalGenerator {
   public:
     Enveloppe() : AbstractSignalGenerator(), _releaseTime(0) {};
     virtual ~Enveloppe() {};
    
     virtual void step(Signal* output);

     inline bool hasEnded()
     {
       return _time > release + _releaseTime;
     }
     
     virtual inline void resetTime()
     {
       _time=0;
       _releaseTime=0;
     }
    
     unsigned int attack;
     unsigned int decay;
     float sustain;
     unsigned int release;
     
   protected:
     unsigned int _releaseTime;
};


#endif