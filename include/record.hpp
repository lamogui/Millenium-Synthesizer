#ifndef RECORD_H
#define RECORD_H

#include <fstream>
#include <iostream>

class Record {
  public:
    bool init ();
    bool close ();
    bool writeNote(int note, unsigned int time);
    
  private:
    std::ofstream *recordFile;

};

#endif
