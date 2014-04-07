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

#include "LUAResourceManager.h"

template<> LUAResourceManager *Ogre::Singleton<LUAResourceManager>::msSingleton = 0;

LUAResourceManager *LUAResourceManager::getSingletonPtr()
{
    return msSingleton;
}

LUAResourceManager &LUAResourceManager::getSingleton()
{
    assert(msSingleton);
    return(*msSingleton);
}

LUAResourceManager::LUAResourceManager()
{
    mResourceType = "LUAResource";
    mLoadOrder = 100.0f;

    Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

LUAResourceManager::~LUAResourceManager()
{
    Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

LUAResourcePtr LUAResourceManager::load(const Ogre::String &name, const Ogre::String &group)
{
    std::cout << "Searching for: " << name << std::endl;
    LUAResourcePtr luafile = getByName(name);

    if(luafile.isNull())
    {
        luafile = create(name, group);
        std::cout << "Creating resource!" << std::endl;
    }

    luafile->load();
    return luafile;
}

Ogre::Resource *LUAResourceManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                                const Ogre::String &group, bool isManual,
                                                Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams)
{
    return new LUAResource(this, name, handle, group, isManual, loader);
}

