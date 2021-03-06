
/*******************************************************
Nom ......... : note.hpp
Role ........ : Déclare les classes permettant de 
                manipuler des données rythmiques en 
                lien avec un morceau et un instrument
Auteur ...... : Julien DE LOOR & Maxime CADORET
Version ..... : V1.7 olol
Licence ..... : © Copydown™
********************************************************/


#ifndef ___NOTES
#define ___NOTES

class InstrumentVoice;
class AbstractInstrument;

//a music note
class Note
{
  public:
    Note(unsigned int st, unsigned char id, float v=1.0);
    Note(const Note& c);
    virtual ~Note();
    
    Note& operator=(const Note&);
    //useless getters and setters
    
    /*inline unsigned int getStartTime()
    {
      return _start;
    }
    inline unsigned int getEndTime()
    {
      return _end;
    }*/
    
    inline unsigned char id()
    {
      return _id;
    }
	
	inline void setId(unsigned char i)
	{
	  _id=i;
	}
	
    inline float velocity()
    {
      return _velocity;
    }
    
	inline void setVelocity(float v)
	{
	  _velocity=v;
	}
	
	inline unsigned int start()
	{
	  return _start;
	}
	
	inline void setStart(unsigned int s)
	{
	  _start=s;
	}
	
	inline unsigned int length()
	{
	  return _length;
	}
	
	inline void setLength(unsigned int l)
	{
	  _length=l;
	}
	
    //how it's working : 
    // - the InstrumentVoice tell to the note that she will be played by it
    void receivePlayedSignal(InstrumentVoice* v);
    // - when the note is finished she should tell it the voice actually used;
    void sendStopSignal();
    
    inline float frequency() { return (float) noteFrequency[_id]; }
    
    static inline float getFrequencyFromID(unsigned char id)
    {
      return (float) noteFrequency[id];
    }
	
  
  private:
    static double noteFrequency[256];
    InstrumentVoice* _voiceUsed;
    
    unsigned int _start; //time where the note is appeared
    unsigned int _length; //note duration
	  float _velocity;
	  unsigned char _id;

};

class InstrumentParameter
{
  public:
    InstrumentParameter(short value, short min, short max);
    InstrumentParameter(const InstrumentParameter& p);
    InstrumentParameter& operator=(const InstrumentParameter& p);
  
    inline bool active() { return _value ? true : false; }
    inline void toggle() { _value = _value ? 0 : _max; notify(); }
    inline void on() { _value = _max; notify(); }
    inline void off() { _value = 0; notify(); }
    
    //Set the value with an other value between 0 and max (from the GUI)
    inline void setValueFromUnsigned(unsigned v, unsigned max) { 
      _value = _min + (v*(_max - _min))/max;
      notify();
      //_value = (v/(float)max)*(float)(_max - _min) + _min;
    }
    
    //get value in range from 0 to max (for the GUI)
    inline unsigned getValueToUnsigned(unsigned max) {
      return (_value-_min)*max/(_max-_min) ;
    }
    
    //get automation value
    inline unsigned getAutoToUnsigned(unsigned max) {
      return (_autoval-_min)*max/(_max-_min) ;
    }
    
    //get value (for the instrument)
    inline short getValue() {
      return _value;
    }
    
    //Set a notification receiver
    inline void notifyOnChange(AbstractInstrument* i) {
      _instrument=i;
    }
    
    
    //direct set (do checks)
    bool setValue(short v);
    bool setAuto(bool a, short v);
    
    
    //getters
    inline short getMin() { return _min; }
    inline short getMax() { return _max; }
    inline unsigned getRange() { return (_max-_min); }
    inline bool newValue() { return _modified; }
    inline void valueUsed() { _modified=false; }
    inline bool isAuto() { return _auto;}
    
    short operator++(int);
    short operator--(int);
    
    
    
  private:
    void notify(); 
    AbstractInstrument* _instrument;
    bool _auto; //parameter automatically controlled
    bool _modified;
    short _value;
    short _autoval;
    short _min;
    short _max;
    
    
};

class InstrumentParameterEvent 
{
  public:
    InstrumentParameterEvent();
    InstrumentParameterEvent(unsigned int  a, unsigned short v, unsigned char i);
    InstrumentParameterEvent(const InstrumentParameterEvent& e);
    
    InstrumentParameterEvent& operator=(const InstrumentParameterEvent&);
    
    
    unsigned int appear;  //time when the event is appeared
    unsigned short value; //Instrument Parameter Value
    unsigned char id;     //Instrument Parameter ID
    
};

#define LA_1   0
#define SIb_1  1
#define SI_1   2
#define DO_1   3
#define REb_1  4
#define RE_1   5
#define MIb_1  6
#define MI_1   7
#define FA_1   8
#define SOLb_1 9
#define SOL_1  10
#define LAb_1  11

#define LA_2   12
#define SIb_2  13
#define SI_2   14
#define DO_2   15
#define REb_2  16
#define RE_2   17
#define MIb_2  18
#define MI_2   19
#define FA_2   20
#define SOLb_2 21
#define SOL_2  22
#define LAb_2  23

#define LA_3   24
#define SIb_3  25
#define SI_3   26
#define DO_3   27
#define REb_3  28
#define RE_3   29
#define MIb_3  30
#define MI_3   31
#define FA_3   32
#define SOLb_3 33
#define SOL_3  34
#define LAb_3  35

#define LA_4   36
#define SIb_4  37
#define SI_4   38
#define DO_4   39
#define REb_4  40
#define RE_4   41
#define MIb_4  42
#define MI_4   43
#define FA_4   44
#define SOLb_4 45
#define SOL_4  46
#define LAb_4  47

#define LA_5   48
#define SIb_5  49
#define SI_5   50
#define DO_5   51
#define REb_5  52
#define RE_5   53
#define MIb_5  54
#define MI_5   55
#define FA_5   56
#define SOLb_5 57
#define SOL_5  58
#define LAb_5  59

#define LA_6   60
#define SIb_6  61
#define SI_6   62
#define DO_6   63
#define REb_6  64
#define RE_6   65
#define MIb_6  66
#define MI_6   67
#define FA_6   68
#define SOLb_6 69
#define SOL_6  70
#define LAb_6  71

#define LA_7   72
#define SIb_7  73
#define SI_7   74
#define DO_7   75
#define REb_7  76
#define RE_7   77
#define MIb_7  78
#define MI_7   79
#define FA_7   80
#define SOLb_7 81
#define SOL_7  82
#define LAb_7  83

#define LA_8   84
#define SIb_8  85
#define SI_8   86
#define DO_8   87

#define NOT_A_NOTE 255

#endif
