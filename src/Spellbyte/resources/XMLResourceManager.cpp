/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * SpellByte XMLResourceManager Class
 *
 * This class manages the interface
 * between Ogre::Resource and XML Files
 *
 */

#include "XMLResourceManager.h"

template<> XMLResourceManager *Ogre::Singleton<XMLResourceManager>::msSingleton = 0;

XMLResourceManager *XMLResourceManager::getSingletonPtr()
{
    return msSingleton;
}

XMLResourceManager &XMLResourceManager::getSingleton()
{
    assert(msSingleton);
    return(*msSingleton);
}

XMLResourceManager::XMLResourceManager()
{
    mResourceType = "XMLResource";
    mLoadOrder = 100.0f;

    Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

XMLResourceManager::~XMLResourceManager()
{
    Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

XMLResourcePtr XMLResourceManager::load(const Ogre::String &name, const Ogre::String &group)
{
    std::cout << "Searching for: " << name << std::endl;
    XMLResourcePtr xmlfile = getByName(name);

    if(xmlfile.isNull())
    {
        xmlfile = create(name, group);
        std::cout << "Creating resource!" << std::endl;
    }

    xmlfile->load();
    return xmlfile;
}

Ogre::Resource *XMLResourceManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                                const Ogre::String &group, bool isManual,
                                                Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *createParams)
{
    return new XMLResource(this, name, handle, group, isManual, loader);
}
