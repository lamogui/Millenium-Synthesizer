#ifndef ___NOTES
#define ___NOTES


//a music note
class InstrumentVoice;

class Note
{
  public:
    Note(unsigned int st, float f, float v=1.0);
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
    }
    
    inline unsigned float getFrequency()
    {
      return _frequency;
    }
    inline unsigned float getVelocity()
    {
      return _velocity;
    }*/
    
    //how it's working : 
    // - the InstrumentVoice tell to the note that she will be played by it
    void receivePlayedSignal(InstrumentVoice* v);
    // - when the note is finished she should tell it the voice actually used;
    void sendStopSignal();
    
    
  
  private:
    InstrumentVoice* _voiceUsed;
    
  public:
    unsigned int start; //time where the note is appeared
    unsigned int lenght; //note duration
    
    float frequency;
    float velocity;
    
};


class InstrumentParameter
{
  public:
    InstrumentParameter(short value, short min, short max);
    InstrumentParameter(const InstrumentParameter& p);
    InstrumentParameter& operator=(const InstrumentParameter& p);
  
    inline bool active() { return _value ? true : false; }
    inline void toggle() { _value = _value ? 0 : _max; }
    inline void on() { _value = _max; }
    inline void off() { _value = 0; }
    
    //Set the value with an other value between 0 and max (from the GUI)
    inline void setValueFromUnsigned(unsigned v, unsigned max) { 
      _value = _min + (v*(_max - _min))/max;
      //_value = (v/(float)max)*(float)(_max - _min) + _min;
    }
    
    //get value in range from 0 to max (for the GUI)
    inline unsigned getValueToUnsigned(unsigned max) {
      return (_value-_min)*max/(_max-_min) ;
    }
    
    //get value (for the instrument)
    inline short getValue() {
      return _value;
    }
    
    //direct set (do checks)
    bool setValue(short v);
    
    //getters
    inline short getMin() { return _min; }
    inline short getMax() { return _max; }
    inline unsigned getRange() { return (_max-_min); }
    
    short operator++(int);
    short operator--(int);
    
    
    
  private:
    short _value;
    short _min;
    short _max;
    
    
};

#endif