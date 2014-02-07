#ifndef _DEFINE_H_

#include "Application.h"
#include "./console/console.h"

// Some basic definitions to simplify things

#define APP Application::getSingletonPtr()

#ifdef _DEBUG
    #define LOG APP->Log->logMessage
#else
    #define LOG APP->Log->logMessage
#endif // _DEBUG

// Used to define values as invalid values
#define INVALID std::numeric_limits<int>::min()
#define INVALID_ACTOR_ID INVALID

#define EASY "easy"
#define MEDIUM "medium"
#define HARD "hard"
#define INSANE "insane"

#endif // _DEFINE_H_
