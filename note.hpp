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

#endif