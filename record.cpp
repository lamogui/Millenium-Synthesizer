#include "record.hpp"

bool Record::init () {
  recordFile = new std::ofstream("recordFile", std::ios::out | std::ios::trunc);
  if (*recordFile)
    return true;
  else
    return false;
}

bool Record::close () {
  if (*recordFile) {
    recordFile->close();
    return true;
  }
  else
    return false;
}

bool Record::writeNote(int note, unsigned int time) {
  if (*recordFile) {
    *recordFile << "time " << time << " note " << note << std::endl;
    return true;
  }
  else 
    return false;
}

