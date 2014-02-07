/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * SpellByte ResourceFile Class
 *
 * This class manages the interface
 * between Ogre::Resource and SpellByte Resource Files
 *
 * This uses smart pointer, any time this resource is needed
 * use SBResFilePtr to get smart pointer: then do not
 * delete!
 *
 */

#ifndef _CRESFILE_H_
#define _CRESFILE_H_

#include <OgreResourceManager.h>

// Some types to handle binary resource file data
 typedef unsigned long dword;
 typedef unsigned short word;
 typedef unsigned char byte;

namespace SpellByte {
    class SBResFile : public Ogre::Resource {
    protected:
        // must be implemented from Ogre::Resource interface
        // Loads resource file
        void loadImpl();
        // Unloads resource
        void unloadImpl();
        // Calculates size of resource
        size_t calculateSize() const;
        // Initialize resources
        void init();

    public:
        SBResFile(Ogre::ResourceManager *creator, const Ogre::String &name,
                      Ogre::ResourceHandle handle, const Ogre::String &group,
                      bool isManual = false, Ogre::ManualResourceLoader *loader = 0);
        virtual ~SBResFile();

        // This method is here temporarily until new resource is created
        std::vector<unsigned char> decompress(int data_no) const;
        // Get memory size
        size_t getDataSize(Ogre::String name) const;
        // Get data
        char *getData(Ogre::String name) const;


    private:
        // Structs for resource file information
        #pragma pack(1)
        struct ResFileHeader {
             enum {
                 SIGNATURE = 0x52373243
             };
             dword sig;
             word version;
        };
        #pragma pack()
        struct ResFileData;
        ResFileHeader fHeader;
        dword dataCount;
        std::vector<ResFileData> Data;
     };

     #pragma pack(1) // Ensure compiler does not use extra padding
     struct SBResFile::ResFileData {
         dword fileNameLen;
         Ogre::String fileName;
         word dataFormat;
         byte compressed;
         dword dataLength;
         std::vector<unsigned char> data;
         dword decompressedLength;
     };
     #pragma pack()

    // Needed for Ogre3d, offers smart pointer to resource
    // Which allows for automatic garbage collection
     class SBResFilePtr : public Ogre::SharedPtr<SBResFile> {
     public:
        SBResFilePtr() : Ogre::SharedPtr<SBResFile>() {}
        explicit SBResFilePtr(SBResFile *rep) : Ogre::SharedPtr<SBResFile>(rep) {}
        SBResFilePtr(const SBResFilePtr &r) : Ogre::SharedPtr<SBResFile>(r) {}
        SBResFilePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<SBResFile>() {
            if (r.isNull())
                return;
            // lock and copy other mutex pointer
            OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
            OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
            pRep = static_cast<SBResFile*>(r.getPointer());
            pUseCount = r.useCountPointer();
            useFreeMethod = r.freeMethod();
            if (pUseCount) {
                ++(*pUseCount);
            }
        }

        // Operator used to convert a ResourcePtr to SBResFilePtr
        SBResFilePtr& operator=(const Ogre::ResourcePtr &r) {
            if (pRep == static_cast<SBResFile*>(r.getPointer()))
                 return *this;
             release();
             if ( r.isNull() )
                 return *this;
                 // resource ptr is null, so the call to release above has done all we need to do.
             // lock & copy other mutex pointer
             OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
             OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
             pRep = static_cast<SBResFile*>(r.getPointer());
             pUseCount = r.useCountPointer();
             useFreeMethod = r.freeMethod();
             if (pUseCount) {
                 ++(*pUseCount);
             }
             return *this;
        }
     };
}

 #endif // _CRESFILE_H_
