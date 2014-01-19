#include "SpellByteResourceFileManager.h"

namespace SpellByte
{
    template<> SBResFileManager *Ogre::Singleton<SBResFileManager>::msSingleton = 0;

    SBResFileManager *SBResFileManager::getSingletonPtr()
    {
        return msSingleton;
    }

    SBResFileManager &SBResFileManager::getSingleton()
    {
        assert(msSingleton);
        return(*msSingleton);
    }

    SBResFileManager::SBResFileManager()
    {
        // tell Ogre what kind of ogre we use
        mResourceType = "SBResourceFile";

        // set priority level, low so it can reference other resources
        mLoadOrder = 30.0f;

        // Register this ResourceManager with OGRE
        Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }

    SBResFileManager::~SBResFileManager()
    {
        // Unregister with ogre
        Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
    }

    SBResFilePtr SBResFileManager::load(const Ogre::String &name, const Ogre::String &group)
    {
        SBResFilePtr resf = getByName(name);

        if(resf.isNull())
            resf = create(name, group);

        resf->load();
        return resf;
    }

    Ogre::Resource *SBResFileManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                                     const Ogre::String &group, bool isManual,
                                                     Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams)
    {
        return new SBResFile(this, name, handle, group, isManual, loader);
    }
}
