/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * SpellByte LUAResource Class Implementation
 *
 * This class manages the interface
 * between Ogre::Resource and LUA Scripts
 *
 */

#include "LUAResourceFile.h"
#include "../Application.h"
#include "../SpellByte.h"

LUAResource::LUAResource(Ogre::ResourceManager *creator, const Ogre::String &name,
                  Ogre::ResourceHandle handle, const Ogre::String &group,
                  bool isManual, Ogre::ManualResourceLoader *loader) :
    Ogre::Resource(creator, name, handle, group, isManual, loader)
    {
        createParamDictionary("LUAResource");
    }

LUAResource::~LUAResource()
{
    unload();
}

void LUAResource::loadImpl()
{
    // First get datastream from Ogre3d
    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);

    luaScript = stream->getAsString();
    stream->close();
}

void LUAResource::unloadImpl()
{
    luaScript = Ogre::String();
}

size_t LUAResource::calculateSize() const
{
    return size_t(luaScript.length());
}

