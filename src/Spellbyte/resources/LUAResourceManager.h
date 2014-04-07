/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * SpellByte LUAResourceManager Class
 *
 * This class manages the interface
 * between Ogre::Resource and LUA Files
 *
 */

#ifndef _LUA_RESOURCE_MANAGER_H_
#define _LUA_RESOURCE_MANAGER_H_

#include <OgreResourceManager.h>
#include "LUAResourceFile.h"

class LUAResourceManager : public Ogre::ResourceManager, public Ogre::Singleton<LUAResourceManager>
{
protected:
    // must implement this from ResourceManager's interface
     Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
         const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
         const Ogre::NameValuePairList *createParams);

public:
    LUAResourceManager();
    virtual ~LUAResourceManager();

    virtual LUAResourcePtr load(const Ogre::String &name, const Ogre::String &group);

    static LUAResourceManager &getSingleton();
    static LUAResourceManager *getSingletonPtr();
};

#endif // _LUA_RESOURCE_MANAGER_H_

