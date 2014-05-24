/*******************************************************
Nom ......... : enveloppe.hpp
Role ........ : les enveloppe (pas pour laposte)
Auteur ...... : Julien DE LOOR & Kwon-Young CHOI
Version ..... : V1.0 olol
Licence ..... : © Copydown™
********************************************************/

#ifndef ENVELOPPE_H
#define ENVELOPPE_H

#include "signal.hpp"

class Enveloppe : public AbstractSignalGenerator {
   public:
     Enveloppe() : AbstractSignalGenerator(), expatk(false), _releaseTime(0) {};
     virtual ~Enveloppe() {};
    
     virtual void step(Signal* output);

     inline bool hasEnded()
     {
       
       return _releaseTime && _time > release + _releaseTime;
     }
     
     virtual inline void resetTime()
     {
       _time=0;
       _releaseTime=0;
     }

     inline void beginRelease()
     {
      _releaseTime=_time;
     }
    
     unsigned int attack;
     unsigned int decay;
     float sustain;
     unsigned int release;
     
     bool expatk;
     
   protected:
     float _releaseLevel;
     unsigned int _releaseTime;
};


#endif
