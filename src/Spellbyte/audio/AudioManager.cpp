#include <cstdio>
#include "AudioManager.h"

#ifdef AUDIO
#include "../console/LuaManager.h"
#include "SLB.hpp"
#include "../console/console.h"
#include "../console/console_communicator.h"
#include "../resources/WAVFileManager.h"
#include "../resources/WAVResourceFile.h"
#include "../utilities/utils.h"

namespace SpellByte {
    AudioManager *AudioManager::Instance = NULL;

    AudioManager::AudioManager() {
        /*device = alcOpenDevice(NULL);//alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));
        err = alGetError();
        if(err != AL_NO_ERROR) {
            LOG("AudioManager Error: " + Ogre::String(alGetString(err)));
        }
        LOG("Sound Device: " + Ogre::String(alcGetString(device, ALC_DEVICE_SPECIFIER)));
        context = alcCreateContext(device, NULL);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            LOG("AudioManager Error: " + Ogre::String(alGetString(err)));
        }
        alListener3f(AL_POSITION, 0, 0, 0);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            LOG("AudioManager Error: " + Ogre::String(alGetString(err)));
        }
        alListener3f(AL_VELOCITY, 0, 0, 0);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            LOG("AudioManager Error: " + Ogre::String(alGetString(err)));
        }
        //alListener3f(AL_ORIENTATION, 0, 0, -1);
        ALfloat	listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};
        alListenerfv(AL_ORIENTATION,listenerOri);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            LOG("AudioManager Error: " + Ogre::String(alGetString(err)));
        }*/
    }

    AudioManager::~AudioManager() {
        //for(unsigned int i = 0; i < SoundList.size(); ++i) {
        //    Mix_FreeChunk(SoundList[i]);
        //}
        WAVPlaying.clear();
    }

    AudioManager *AudioManager::getInstance() {
        if (!Instance) {
            Instance = new AudioManager();
        }

        return Instance;
    }

    bool AudioManager::init() {
        COMM->registerSubscriber("audioman", this);
        bindToLUA();
        return true;
    }

    void AudioManager::listDevices() {
        const ALCchar *devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;

        LOG("Devices list:");
        LOG("----------");
        while (device && *device != '\0' && next && *next != '\0') {
                LOG(device);
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
        }
        LOG("----------");
    }

    std::string AudioManager::handleConsoleCmd(std::queue<std::string> cmdQueue) {
        std::string returnString;
        std::string nextCmd = cmdQueue.front();
        cmdQueue.pop();
        returnString = "Not yet implemented.";
        return returnString;
    }

    // Load wave file function.
    int AudioManager::loadWavFile(const std::string filename) {
        LOG("Loading WAV file: " + filename);
        WAVFilePtr wavFile = APP->wavManager->load(filename, "Audio");
        if (!wavFile->validWAV())
            return -1;
        return 0;
    }

    void AudioManager::playWAV(const std::string filename) {
        LOG("Loading WAV file: " + filename);
        WAVFilePtr wavFile;
        try {
            wavFile = APP->wavManager->load(filename, "Audio");
        } catch (Ogre::Exception &e) {
            LOG(e.getFullDescription());
            return;
        }
        if (!wavFile->validWAV()) {
            LOG("Error: could not load wave or invalid file");
            return;
        }

        if(wavFile->playWAV()) {
            LOG("Played " + filename);
            WAVPlaying.push_back(wavFile);
        } else {
            LOG("Could not play " + filename);
        }
    }

    bool AudioManager::update(const Ogre::FrameEvent &evt) {
        std::vector<WAVFilePtr>::size_type i = 0;
        while ( i < WAVPlaying.size() ) {
            if ( !WAVPlaying[i]->isPlaying() ) {
                LOG("Closing sound");
                WAVPlaying[i]->close();
                WAVPlaying.erase(WAVPlaying.begin() + i);
            } else {
                ++i;
            }
        }

        return true;
    }

    // Bind AudioManager to LUA, update as necessary
    void AudioManager::bindToLUA() {
        SLB::Class<AudioManager, SLB::Instance::NoCopyNoDestroy >("AudioManager::getInstance()")
            .set("loadWavFile", &AudioManager::loadWavFile)
            .set("playWAV", &AudioManager::playWAV)
            .set("listDevices", &AudioManager::listDevices);

        // Set global access to LUA
        SLB::setGlobal<AudioManager*>(&(*LUAMANAGER->LUA), AudioManager::getInstance(), "AudioManager");
    }
}
#endif
