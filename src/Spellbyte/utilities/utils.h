#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstring>

// This functions reads either a BYTE, WORD, or DWORD from a vector of unsigned char
template <class T>
void ReadFromVector(T &t, std::size_t offset, const std::vector<unsigned char> &v)
{
    memcpy(&t, &v[offset], sizeof(T));
}

// Converts type long to unsigned char*
inline void longToUnsignedCharArray(unsigned char *ucArray, const long value)
{
    ucArray[0] = (int)((value >> 24) & 0xFF);
    ucArray[1] = (int)((value >> 16) & 0xFF);
    ucArray[2] = (int)((value >> 8) & 0xFF);
    ucArray[3] = (int)((value & 0xFF));
}

// Converts unsigned char* to long (unsigned char must be size 4)
inline long unsignedCharArrayToLong(unsigned char* ucArray)
{
    long dest;
    dest = ( (ucArray[0] << 24)
            + (ucArray[1] << 16)
            + (ucArray[2] << 8)
            + (ucArray[3]));
    return dest;
}

// Converts dword to long
inline long dwordToLong(unsigned long dWord)
{
    size_t len = sizeof(dWord);
    unsigned char *tCount = new unsigned char[len];
    memcpy(tCount, &dWord, len);
    return unsignedCharArrayToLong(tCount);
}

#endif // _UTILS_H_
