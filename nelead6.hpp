
#ifndef __NELEAD6
#define __NELEAD6

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"

//modulation modes
#define NELEAD6_FM 0
#define NELEAD6_RM 1

class NELead6Voice : public InstrumentVoice
{
  public:
    NELead6Voice(AbstractInstrument* creator);
    virtual ~NELead6Voice();

    void beginNote(Note& n);
    void endNote();
    void step(Signal* output);
    
  private:
    Oscillator* _osc1;
    Oscillator* _osc2;
    Oscillator* _lfo1;
    Oscillator* _lfo2;
};


class NELead6 : public Instrument<NELead6Voice>
{
  public: 
    NELead6();
    virtual ~NELead6();
    
    virtual InstrumentParameter* getParameter(unsigned char id);
    virtual void step(Signal* output);
}; 

class NELead6Interface : public Interface
{
  public:
    NELead6Interface(NELead6* instrument, const sf::Vector2f& size);
    virtual ~NELead6Interface();
    
  protected:
    sf::Texture _texture;
    sf::Sprite _back;
    NELead6* _instrument;
};



#endif