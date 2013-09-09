/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Cube27 XMLResourceManager Class
 *
 * This class manages the interface
 * between Ogre::Resource and XML Files
 *
 */

#ifndef _XML_RESOURCE_MANAGER_H_
#define _XML_RESOURCE_MANAGER_H_

#include <OgreResourceManager.h>
#include "XMLResource.h"

class XMLResourceManager : public Ogre::ResourceManager, public Ogre::Singleton<XMLResourceManager>
{
protected:
    // must implement this from ResourceManager's interface
     Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
         const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
         const Ogre::NameValuePairList *createParams);

public:
    XMLResourceManager();
    virtual ~XMLResourceManager();

    virtual XMLResourcePtr load(const Ogre::String &name, const Ogre::String &group);

    static XMLResourceManager &getSingleton();
    static XMLResourceManager *getSingletonPtr();
};

#endif // _XML_RESOURCE_MANAGER_H_
