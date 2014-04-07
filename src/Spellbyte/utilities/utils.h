#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstring>
#include <vector>
#include "../stdafx.h"
#include "tinyxml2.h"

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

inline int unsignedCharArrayToInt(unsigned char* ucArray)
{
    int dest;
    dest = ( (ucArray[0] << 24)
            + (ucArray[1] << 16)
            + (ucArray[2] << 8)
            + (ucArray[3]));
    return dest;
}

inline short unsignedCharArrayToShort(unsigned char* ucArray)
{
    short dest;
    dest = ( (ucArray[0] << 8)
            + (ucArray[1]));
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

inline bool loadPosition(tinyxml2::XMLElement *elt, float &x, float &y, float &z)
{
    tinyxml2::XMLElement *pos = elt->FirstChildElement("position");
    if(!pos)
    {
        return false;
    }
    x = pos->FloatAttribute("x");
    y = pos->FloatAttribute("y");
    z = pos->FloatAttribute("z");
    return true;
}

inline bool loadScale(tinyxml2::XMLElement *elt, float &x, float &y, float &z)
{
    tinyxml2::XMLElement *pos = elt->FirstChildElement("scale");
    if(!pos)
    {
        return false;
    }
    x = pos->FloatAttribute("x");
    y = pos->FloatAttribute("y");
    z = pos->FloatAttribute("z");
    return true;
}

// Return 0 of rotate does not exist
// Return 1 if rotation uses quaternion (PRIORITY)
// Return 2 if rotation has roll, pitch, and yaw
inline int loadRotate(tinyxml2::XMLElement *elt, float &roll, float &pitch, float &yaw, float &qw, float &qx, float &qy, float &qz)
{
    tinyxml2::XMLElement *pos = elt->FirstChildElement("rotate");
    if(!pos)
    {
        return 0;
    }
    if(pos->QueryFloatAttribute("qw", &qw) != tinyxml2::XML_NO_ATTRIBUTE &&
       pos->QueryFloatAttribute("qx", &qx) != tinyxml2::XML_NO_ATTRIBUTE &&
       pos->QueryFloatAttribute("qy", &qy) != tinyxml2::XML_NO_ATTRIBUTE &&
       pos->QueryFloatAttribute("qz", &qz) != tinyxml2::XML_NO_ATTRIBUTE)
    {
        return 1;
    }
    roll = pos->FloatAttribute("roll");
    pitch = pos->FloatAttribute("pitch");
    yaw = pos->FloatAttribute("yaw");
    return 2;
}

template <class container>
inline void DeleteSTLContainer(container& c)
{
  for(typename container::iterator it = c.begin(); it != c.end(); ++it)
  {
    delete *it;
    *it = NULL;
  }
}

inline void truncate(Ogre::Vector3 &v, Ogre::Real maxLength) {
  if(v.squaredLength() > (maxLength * maxLength)) {
    v = (v.normalisedCopy() * maxLength);
  }
}
#endif // _UTILS_H_
