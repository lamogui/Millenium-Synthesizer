#ifndef __CONFIG__SYNTH
#define __CONFIG__SYNTH

#include <SFML/Config.hpp>

#if defined(SFML_SYSTEM_WINDOWS)
#define COMPILE_WINDOWS
#endif 

#if defined HAVE_QT4
#define HAVE_QT
#endif

#if defined HAVE_QT3
#define HAVE_QT
#endif

#endif
