#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#include <vector>
#include <string>
#include "../define.h"
#ifdef AUDIO
#include <AL/al.h>
#include <AL/alc.h>
#include "../Subscriber.h"

#define AUDIOMAN AudioManager::getInstance()

namespace SpellByte {
    class AudioManager : public Subscriber {
    public:
        ~AudioManager();
        static AudioManager *getInstance();

        bool init();

        int loadWavFile(const std::string filename);
        void playWAV(const std::string filename);

        std::string handleConsoleCmd(std::queue<std::string> cmdQueue);
        void listDevices();

        bool update(const Ogre::FrameEvent &evt);

    private:
        AudioManager();
        AudioManager(const AudioManager&);
        AudioManager& operator=(const AudioManager&);

        // To bind AudioManager Class to LUA, also sets global App accessor within LUA
        void bindToLUA();

        static AudioManager         *Instance;
        std::vector<WAVFilePtr> WAVPlaying;

        // OpenAL
        ALCdevice *device;
        ALCcontext *context;
        ALenum err;
    };
}
#endif

#endif // _SOUND_MANAGER_H_
