#ifndef _SPELLBYTE_H_
#define _SPELLBYTE_H_

#define APP Application::getSingletonPtr()
#ifdef _DEBUG
#define LOG APP->Log->logMessage
#else
#define LOG //
#endif // _DEBUG

// Used to define values as invalid values
#define INVALID std::numeric_limits<int>::min()
#define INVALID_ACTOR_ID INVALID

#include "Application.h"

#endif // _SPELLBYTE_H_
