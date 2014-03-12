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
    float noteFrequency [88] = {
      27.5,29.1353,30.8677,32.7032,34.6479,36.7081,38.8909,41.2035,43.6536,46.2493,48.9995,51.9130,
      55,  58.2705,61.7354,65.4064,69.2957,73.4162,77.7817,82.4069,87.3071,92.4986,97.9989,103.826,
      110, 116.541,123.471,130.813,138.591,146.832,155.563,164.814,174.614,184.997,195.998,207.652,
      220, 233.082,246.942,261.626,277.183,293.665,311.127,329.628,349.228,369.994,391.995,415.305,
      440, 446.164,493.883,523.251,554.365,587.33, 622.254,659.255,698.456,739.989,783.991,830.609,
      880, 932.328,987.767,1046.5, 1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,
      1760,1864.66,1975.53,2093,   2217.46,2349.32,2489.02,2637.02,2793.02,2959.96,3135.96,3322.44,
      3520,3729.31,3951.07,4186.01}
    
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
