/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Cube27 XMLResource Class
 *
 * This class manages the interface
 * between Ogre::Resource and XML Files
 *
 * This uses smart pointer, any time this resource is needed
 * use XMLPtr to get smart pointer: then do not
 * delete!
 *
 */

#ifndef _XMLRESOURCE_H_
#define _XMLRESOURCE_H_

#include <OgreResourceManager.h>
#include "../utilities/tinyxml2.h"

class manualXMLLoader;
class XMLResource : public Ogre::Resource
 {
     friend manualXMLLoader;
protected:
    // must be implemented from Ogre::Resource interface
    // Loads resource file
    void loadImpl();
    // Unloads resource
    void unloadImpl();
    // Calculates size of resource
    size_t calculateSize() const;

public:
    XMLResource(Ogre::ResourceManager *creator, const Ogre::String &name,
                  Ogre::ResourceHandle handle, const Ogre::String &group,
                  bool isManual = false, Ogre::ManualResourceLoader *loader = 0);
    virtual ~XMLResource();

    // Return pointer to XML document
    tinyxml2::XMLDocument *getXML() { return &xmlFile; };


private:
    tinyxml2::XMLDocument xmlFile;
    size_t dataSize;
 };

// Needed for Ogre3d, offers smart pointer to resource
// Which allows for automatic garbage collection
 class XMLResourcePtr : public Ogre::SharedPtr<XMLResource>
 {
 public:
    XMLResourcePtr() : Ogre::SharedPtr<XMLResource>() {}
    explicit XMLResourcePtr(XMLResource *rep) : Ogre::SharedPtr<XMLResource>(rep) {}
    XMLResourcePtr(const XMLResourcePtr &r) : Ogre::SharedPtr<XMLResource>(r) {}
    XMLResourcePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<XMLResource>()
    {
        if(r.isNull())
            return;
        // lock and copy other mutex pointer
        OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
        OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
        pRep = static_cast<XMLResource*>(r.getPointer());
        pUseCount = r.useCountPointer();
        useFreeMethod = r.freeMethod();
        if(pUseCount)
        {
            ++(*pUseCount);
        }
    }

    // Operator used to convert a ResourcePtr to Cube27ResFilePtr
    XMLResourcePtr& operator=(const Ogre::ResourcePtr &r)
    {
        if(pRep == static_cast<XMLResource*>(r.getPointer()))
             return *this;
         release();
         if( r.isNull() )
             return *this;
             // resource ptr is null, so the call to release above has done all we need to do.
         // lock & copy other mutex pointer
         OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
         OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
         pRep = static_cast<XMLResource*>(r.getPointer());
         pUseCount = r.useCountPointer();
         useFreeMethod = r.freeMethod();
         if (pUseCount)
         {
             ++(*pUseCount);
         }
         return *this;
    }
 };

 #endif // _XMLResource_H_

