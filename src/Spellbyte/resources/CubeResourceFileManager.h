/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Cube27 ResourceFileManager Class
 *
 * This class manages the interface
 * between Ogre::Resource and Cube27 Resource Files
 *
 */

#ifndef _CRESFILEMANAGER_H_
#define _CRESFILEMANAGER_H_

#include <OgreResourceManager.h>
#include "CubeResourceFile.h"

namespace SpellByte
{
    class Cube27ResFileManager : public Ogre::ResourceManager, public
    Ogre::Singleton<Cube27ResFileManager>
    {
    protected:
        // must implement from ResourceManager's interface
        Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                   const Ogre::String &group, bool isManual,
                                   Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams);

    public:
        Cube27ResFileManager();
        virtual ~Cube27ResFileManager();

        virtual Cube27ResFilePtr load(const Ogre::String &name, const Ogre::String &group);

        static Cube27ResFileManager &getSingleton();
        static Cube27ResFileManager *getSingletonPtr();
    };
}

#endif // _CRESFILEMANAGER_H_
