#include "CubeResourceFileManager.h"

namespace SpellByte
{
    template<> Cube27ResFileManager *Ogre::Singleton<Cube27ResFileManager>::msSingleton = 0;

    Cube27ResFileManager *Cube27ResFileManager::getSingletonPtr()
    {
        return msSingleton;
    }

    Cube27ResFileManager &Cube27ResFileManager::getSingleton()
    {
        assert(msSingleton);
        return(*msSingleton);
    }

    Cube27ResFileManager::Cube27ResFileManager()
    {
        // tell Ogre what kind of ogre we use
        mResourceType = "Cube27ResourceFile";

        // set priority level, low so it can reference other resources
        mLoadOrder = 30.0f;

        // Register this ResourceManager with OGRE
        Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }

    Cube27ResFileManager::~Cube27ResFileManager()
    {
        // Unregister with ogre
        Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
    }

    Cube27ResFilePtr Cube27ResFileManager::load(const Ogre::String &name, const Ogre::String &group)
    {
        Cube27ResFilePtr resf = getByName(name);

        if(resf.isNull())
            resf = create(name, group);

        resf->load();
        return resf;
    }

    Ogre::Resource *Cube27ResFileManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                                     const Ogre::String &group, bool isManual,
                                                     Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams)
    {
        return new Cube27ResFile(this, name, handle, group, isManual, loader);
    }
}
