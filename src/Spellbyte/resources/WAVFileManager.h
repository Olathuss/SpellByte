/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * WAVFileManager Class
 *
 * This class manages the WAV Files
 *
 */

#ifndef _WAVFILEMANAGER_H_
#define _WAVFILEMANAGER_H_

#ifdef AUDIO
#include <OgreResourceManager.h>
#include "WAVResourceFile.h"

namespace SpellByte
{
    class WAVFileManager : public Ogre::ResourceManager, public
    Ogre::Singleton<WAVFileManager>
    {
    protected:
        // must implement from ResourceManager's interface
        Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                   const Ogre::String &group, bool isManual,
                                   Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams);

    public:
        WAVFileManager();
        virtual ~WAVFileManager();

        virtual WAVFilePtr load(const Ogre::String &name, const Ogre::String &group);

        static WAVFileManager &getSingleton();
        static WAVFileManager *getSingletonPtr();
    };
}
#endif

#endif // _CRESFILEMANAGER_H_

