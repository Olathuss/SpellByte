#ifndef _DEFINE_H_

#include "Application.h"

// Some basic definitions to simplify things

#define APP Application::getSingletonPtr()

#ifdef _DEBUG
    #define LOG APP->Log->logMessage
#else
    #define LOG //
#endif // _DEBUG

// Used to define values as invalid values
#define INVALID std::numeric_limits<int>::min()
#define INVALID_ACTOR_ID INVALID

#define EASY "easy"
#define MEDIUM "medium"
#define HARD "hard"
#define INSANE "insane"

#define LEVEL_RESOURCE "Cube27Resource"
#define UNDEFINED_PUZZLE_STAGE INVALID
#define UNDEFINED_PUZZLE_LEVEL INVALID
#define DEFAULT_PUZZLE_NAME "undef"

#endif // _DEFINE_H_
