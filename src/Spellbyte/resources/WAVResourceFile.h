/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * WAV ResourceFile Class
 *
 * This class manages WAV Resource Files
 *
 * This uses smart pointer, any time this resource is needed
 * use WAVFilePtr to get smart pointer: then do not
 * delete!
 *
 */

#ifndef _WAVRESFILE_H_
#define _WAVRESFILE_H_

#ifdef AUDIO
#include <OgreResourceManager.h>
#include <AL/al.h>
#include <AL/alc.h>

// Some types to handle binary resource file data
 typedef unsigned long dword;
 typedef unsigned short word;
 typedef unsigned char byte;

namespace SpellByte {
    class WAVFile : public Ogre::Resource {
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
        WAVFile(Ogre::ResourceManager *creator, const Ogre::String &name,
                      Ogre::ResourceHandle handle, const Ogre::String &group,
                      bool isManual = false, Ogre::ManualResourceLoader *loader = 0);
        virtual ~WAVFile();

        // Check if file is valid
        bool validWAV() { return validWave; };

        bool playWAV();
        bool isPlaying();
        void close();

        ALenum getFormat();

    private:
        ALCdevice *device;
        ALCcontext *context;
        ALuint source;
        ALuint buffer;
        short audioFormat;
        short numChannels;
        int sampleRate;
        int byteRate;
        short byteAlign;
        short bitsPerSample;
        int dataSize;
        unsigned char* data;

        ALenum err;

        bool validWave;
    };

    // Needed for Ogre3d, offers smart pointer to resource
    // Which allows for automatic garbage collection
     class WAVFilePtr : public Ogre::SharedPtr<WAVFile> {
     public:
        WAVFilePtr() : Ogre::SharedPtr<WAVFile>() {}
        explicit WAVFilePtr(WAVFile *rep) : Ogre::SharedPtr<WAVFile>(rep) {}
        WAVFilePtr(const WAVFilePtr &r) : Ogre::SharedPtr<WAVFile>(r) {}
        WAVFilePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<WAVFile>() {
            if (r.isNull())
                return;
            // lock and copy other mutex pointer
            OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
            OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
            pRep = static_cast<WAVFile*>(r.getPointer());
            pUseCount = r.useCountPointer();
            useFreeMethod = r.freeMethod();
            if (pUseCount) {
                ++(*pUseCount);
            }
        }

        // Operator used to convert a ResourcePtr to WAVFilePtr
        WAVFilePtr& operator=(const Ogre::ResourcePtr &r) {
            if (pRep == static_cast<WAVFile*>(r.getPointer()))
                 return *this;
             release();
             if ( r.isNull() )
                 return *this;
                 // resource ptr is null, so the call to release above has done all we need to do.
             // lock & copy other mutex pointer
             OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
             OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
             pRep = static_cast<WAVFile*>(r.getPointer());
             pUseCount = r.useCountPointer();
             useFreeMethod = r.freeMethod();
             if (pUseCount) {
                 ++(*pUseCount);
             }
             return *this;
        }
     };
}
#endif
 #endif // _WAVRESFILE_H_

