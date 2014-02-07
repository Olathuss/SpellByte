#include "WAVFileManager.h"

#ifdef AUDIO
namespace SpellByte
{
    template<> WAVFileManager *Ogre::Singleton<WAVFileManager>::msSingleton = 0;

    WAVFileManager *WAVFileManager::getSingletonPtr()
    {
        return msSingleton;
    }

    WAVFileManager &WAVFileManager::getSingleton()
    {
        assert(msSingleton);
        return(*msSingleton);
    }

    WAVFileManager::WAVFileManager()
    {
        // tell Ogre what kind of ogre we use
        mResourceType = "WAVFile";

        // set priority level, low so it can reference other resources
        mLoadOrder = 50.0f;

        // Register this ResourceManager with OGRE
        Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }

    WAVFileManager::~WAVFileManager()
    {
        // Unregister with ogre
        Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
    }

    WAVFilePtr WAVFileManager::load(const Ogre::String &name, const Ogre::String &group)
    {
        WAVFilePtr resf = getByName(name);

        if(resf.isNull())
            resf = create(name, group);

        resf->load();
        return resf;
    }

    Ogre::Resource *WAVFileManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                                     const Ogre::String &group, bool isManual,
                                                     Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams)
    {
        return new WAVFile(this, name, handle, group, isManual, loader);
    }
}
#endif
