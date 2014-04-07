/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * SpellByte XMLResource Class
 *
 * This class manages the interface
 * between Ogre::Resource and LUA Files
 *
 * This uses smart pointer, any time this resource is needed
 * use LUAPtr to get smart pointer: then do not
 * delete!
 *
 */

#ifndef _LUARESOURCE_H_
#define _LUARESOURCE_H_

#include <OgreResourceManager.h>

class LUAResource : public Ogre::Resource
 {
protected:
    // must be implemented from Ogre::Resource interface
    // Loads resource file
    void loadImpl();
    // Unloads resource
    void unloadImpl();
    // Calculates size of resource
    size_t calculateSize() const;

public:
    LUAResource(Ogre::ResourceManager *creator, const Ogre::String &name,
                  Ogre::ResourceHandle handle, const Ogre::String &group,
                  bool isManual = false, Ogre::ManualResourceLoader *loader = 0);
    virtual ~LUAResource();

    // Return pointer to LUA script
    Ogre::String getLUA() { return luaScript; };


private:
    Ogre::String luaScript;
 };

// Needed for Ogre3d, offers smart pointer to resource
// Which allows for automatic garbage collection
class LUAResourcePtr : public Ogre::SharedPtr<LUAResource>
{
public:
    LUAResourcePtr() : Ogre::SharedPtr<LUAResource>() {}
    explicit LUAResourcePtr(LUAResource *rep) : Ogre::SharedPtr<LUAResource>(rep) {}
    LUAResourcePtr(const LUAResourcePtr &r) : Ogre::SharedPtr<LUAResource>(r) {}
    LUAResourcePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<LUAResource>() {
        if(r.isNull())
            return;
        // lock and copy other mutex pointer
        OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
        OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
        pRep = static_cast<LUAResource*>(r.getPointer());
        pUseCount = r.useCountPointer();
        useFreeMethod = r.freeMethod();
        if(pUseCount) {
            ++(*pUseCount);
        }
    }

    // Operator used to convert a ResourcePtr to SBResFilePtr
    LUAResourcePtr& operator=(const Ogre::ResourcePtr &r)
    {
        if(pRep == static_cast<LUAResource*>(r.getPointer()))
            return *this;
        release();
        if (r.isNull())
            return *this;
        // resource ptr is null, so the call to release above has done all we need to do.
        // lock & copy other mutex pointer
        OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
        OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
        pRep = static_cast<LUAResource*>(r.getPointer());
        pUseCount = r.useCountPointer();
        useFreeMethod = r.freeMethod();
        if (pUseCount) {
            ++(*pUseCount);
        }
        return *this;
    }
};

#endif // _LUARESOURCE_H_


