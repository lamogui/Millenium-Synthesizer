
#include "config.hpp"
#include <string>
#include <bass.h>

#if defined(COMPILE_WINDOWS)
#include <bassasio.h>
std::string handleBassAsioInitError();
std::string handleBassAsioStartError();
#endif

std::string handleBassInitError();
std::string handleBassStreamCreateError();
std::string handleBassChannelPlayError();
