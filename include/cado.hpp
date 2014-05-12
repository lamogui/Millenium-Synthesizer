#ifndef __CADO
#define __CADO

#include "instrument.hpp"
#include "oscillator.hpp"
#include "interface.hpp"
#include "enveloppe.hpp"
#include "filter.hpp"



class CadoVoice : public InstrumentVoice
{
  public:
    CadoVoice(AbstractInstrument* creator);
    virtual ~CadoVoice();

    void beginNote(Note& n);
    void endNote();
    void step(Signal* leftout, Signal* rightout=0);
    
  private:
    Oscillator* _osc1;
	Oscillator* _osc2;
	Oscillator* _osc3;
    Enveloppe _env;
    Note _currentNote;
};


class Cado : public Instrument<CadoVoice>
{
  public: 
    Cado();
    virtual ~Cado();
    
    void step(Signal* leftout, Signal* rightout=0);
    
}; 


class CadoInterface : public Interface
{
  public:
    CadoInterface(Cado* instrument, const sf::Vector2f& size);
    virtual ~CadoInterface();
    
  protected:
    sf::Texture _texture;
    sf::Sprite _back;
    Cado* _instrument;
};



#endif
