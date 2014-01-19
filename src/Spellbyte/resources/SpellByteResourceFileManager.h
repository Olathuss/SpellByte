/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * SpellByte ResourceFileManager Class
 *
 * This class manages the interface
 * between Ogre::Resource and SpellByte Resource Files
 *
 */

#ifndef _CRESFILEMANAGER_H_
#define _CRESFILEMANAGER_H_

#include <OgreResourceManager.h>
#include "SpellByteResourceFile.h"

namespace SpellByte
{
    class SBResFileManager : public Ogre::ResourceManager, public
    Ogre::Singleton<SBResFileManager>
    {
    protected:
        // must implement from ResourceManager's interface
        Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                   const Ogre::String &group, bool isManual,
                                   Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams);

    public:
        SBResFileManager();
        virtual ~SBResFileManager();

        virtual SBResFilePtr load(const Ogre::String &name, const Ogre::String &group);

        static SBResFileManager &getSingleton();
        static SBResFileManager *getSingletonPtr();
    };
}

#endif // _CRESFILEMANAGER_H_
