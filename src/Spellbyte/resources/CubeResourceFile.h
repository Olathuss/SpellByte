/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Cube27 ResourceFile Class
 *
 * This class manages the interface
 * between Ogre::Resource and Cube27 Resource Files
 *
 * This uses smart pointer, any time this resource is needed
 * use Cube27ResFilePtr to get smart pointer: then do not
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

namespace SpellByte
{
    class Cube27ResFile : public Ogre::Resource
     {
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
        Cube27ResFile(Ogre::ResourceManager *creator, const Ogre::String &name,
                      Ogre::ResourceHandle handle, const Ogre::String &group,
                      bool isManual = false, Ogre::ManualResourceLoader *loader = 0);
        virtual ~Cube27ResFile();

        // This method is here temporarily until new resource is created
        std::vector<unsigned char> decompress(int data_no) const;
        // Get memory size
        size_t getDataSize(Ogre::String name) const;
        // Get data
        char *getData(Ogre::String name) const;


    private:
        // Structs for resource file information
        #pragma pack(1)
        struct ResFileHeader
        {
             enum
             {
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
     struct Cube27ResFile::ResFileData
     {
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
     class Cube27ResFilePtr : public Ogre::SharedPtr<Cube27ResFile>
     {
     public:
        Cube27ResFilePtr() : Ogre::SharedPtr<Cube27ResFile>() {}
        explicit Cube27ResFilePtr(Cube27ResFile *rep) : Ogre::SharedPtr<Cube27ResFile>(rep) {}
        Cube27ResFilePtr(const Cube27ResFilePtr &r) : Ogre::SharedPtr<Cube27ResFile>(r) {}
        Cube27ResFilePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<Cube27ResFile>()
        {
            if(r.isNull())
                return;
            // lock and copy other mutex pointer
            OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
            OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
            pRep = static_cast<Cube27ResFile*>(r.getPointer());
            pUseCount = r.useCountPointer();
            useFreeMethod = r.freeMethod();
            if(pUseCount)
            {
                ++(*pUseCount);
            }
        }

        // Operator used to convert a ResourcePtr to Cube27ResFilePtr
        Cube27ResFilePtr& operator=(const Ogre::ResourcePtr &r)
        {
            if(pRep == static_cast<Cube27ResFile*>(r.getPointer()))
                 return *this;
             release();
             if( r.isNull() )
                 return *this;
                 // resource ptr is null, so the call to release above has done all we need to do.
             // lock & copy other mutex pointer
             OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
             OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
             pRep = static_cast<Cube27ResFile*>(r.getPointer());
             pUseCount = r.useCountPointer();
             useFreeMethod = r.freeMethod();
             if (pUseCount)
             {
                 ++(*pUseCount);
             }
             return *this;
        }
     };
}

 #endif // _CRESFILE_H_
