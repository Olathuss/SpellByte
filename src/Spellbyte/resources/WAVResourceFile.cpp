/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * WAV ResourceFile Class
 *
 * This class manages the WAV Resource Files
 *
 */

#include "WAVResourceFile.h"
#ifdef AUDIO
#include <AL/alc.h>
#include "../utilities/easyzlib.h"
#include "../utilities/utils.h"
#include "file_defines.h"
#include "ManualLoaders.h"

namespace SpellByte {
    WAVFile::WAVFile(Ogre::ResourceManager *creator, const Ogre::String &name,
                  Ogre::ResourceHandle handle, const Ogre::String &group,
                  bool isManual, Ogre::ManualResourceLoader *loader) :
    Ogre::Resource(creator, name, handle, group, isManual, loader) {
        createParamDictionary("WAVFile");
        validWave = false;
    }

    WAVFile::~WAVFile() {
        unload();
    }

    /*void WAVFile::loadImpl() {
        ALCdevice* device = alcOpenDevice(NULL);
        ALenum err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }

        ALCcontext* context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }

        alListener3f(AL_POSITION, 0, 0, 0);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }
        alListener3f(AL_VELOCITY, 0, 0, 0);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }
        ALfloat	listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};
        alListenerfv(AL_ORIENTATION,listenerOri);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }


        ALuint source;
        ALuint buffer;
        alGenSources(1, &source);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }

        alSourcef(source, AL_PITCH, 1);
        alSourcef(source, AL_GAIN, 1);
        alSource3f(source, AL_POSITION, 0, 0, 0);
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSourcei(source, AL_LOOPING, AL_FALSE);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }

        //std::vector<unsigned char> wavBuffer;
        // First get datastream from Ogre3d
        Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
        // Create vector
        std::vector<unsigned char> wavBuffer = std::vector<unsigned char>(stream->size());
        // Copy stream into vector
        stream->read(&wavBuffer[0], stream->size());
        stream->close();

        std::cout << wavBuffer.size() << std::endl;
        unsigned char riff[4] = { 'R', 'I', 'F', 'F' };
        unsigned int offset = 0;
        for(unsigned int i = offset; i < 4; i++) {
            if(wavBuffer[i] != riff[i]) {
                std::cout << "Invalid wave";
                return;
            } else {
                std::cout << "OK: " << wavBuffer[i + offset] << std::endl;
            }
        }
        offset += sizeof(long) + 4;
        unsigned char riffFormat[4] = { 'W', 'A', 'V', 'E' };
        for(unsigned int i = 0; i < 4; i++) {
            if(wavBuffer[i + offset] != riffFormat[i]) {
                std::cout << "Invalid wave.  Expected: " << riffFormat[i] << " Got: " << wavBuffer[i + offset] << std::endl;
                return;
            } else {
                std::cout << "OK: " << wavBuffer[i + offset] << std::endl;
            }
        }
        offset += 4;
        unsigned char waveFormat[4] = { 'f', 'm', 't', ' ' };
        for(unsigned int i = 0; i < 4; i++) {
            if(wavBuffer[i + offset] != waveFormat[i]) {
                std::cout << "Invalid wave.  Expected: " << waveFormat[i] << " Got: " << wavBuffer[i + offset] << std::endl;
                return;
            } else {
                std::cout << "OK: " << wavBuffer[i + offset] << std::endl;
            }
        }
        offset += sizeof(long) * 2;
        unsigned char audForm[2];
        audForm[1] = wavBuffer[offset++];
        audForm[0] = wavBuffer[offset++];
        audioFormat = unsignedCharArrayToShort(audForm);
        std::cout << "Audio Format (1 is PCM): " << audioFormat << std::endl;
        unsigned char numChan[2];
        numChan[1] = wavBuffer[offset++];
        numChan[0] = wavBuffer[offset++];
        numChannels = unsignedCharArrayToShort(numChan);
        std::cout << "Number of Channels: " << numChannels << std::endl;
        unsigned char samRate[4];
        for(int i = 3; i >= 0; i--) {
            samRate[i] = wavBuffer[offset++];
        }
        sampleRate = unsignedCharArrayToInt(samRate);
        std::cout << "Sample Rate: " << sampleRate << std::endl;
        unsigned char byRate[4];
        for(int i = 3; i >= 0; i--) {
            byRate[i] = wavBuffer[offset++];
        }
        byteRate = unsignedCharArrayToLong(byRate);
        std::cout << "Byte Rate: " << byteRate << std::endl;
        unsigned char byteAlignA[2];
        for(int i = 1; i >= 0; i--) {
            byteAlignA[i] = wavBuffer[offset++];
        }
        byteAlign = unsignedCharArrayToShort(byteAlignA);
        std::cout << "Byte Align: " << byteAlign << std::endl;
        unsigned char bitsPerSampleA[2];
        for(int i = 1; i >= 0; i--) {
            bitsPerSampleA[i] = wavBuffer[offset++];
        }
        bitsPerSample = unsignedCharArrayToShort(bitsPerSampleA);
        std::cout << "Bits Per Sample: " << bitsPerSample << std::endl;
        unsigned char dataStart[4] = { 'd', 'a', 't', 'a' };
        for(unsigned int i = 0; i < 4; i++) {
            if(wavBuffer[i + offset] != dataStart[i]) {
                std::cout << "Invalid wave.  Expected: " << dataStart[i] << " Got: " << wavBuffer[i + offset] << std::endl;
                return 1;
            } else {
                std::cout << "OK: " << wavBuffer[i + offset] << std::endl;
            }
        }
        offset += sizeof(long);
        unsigned char dataSizeA[4];
        for(int i = 3; i >= 0; i--) {
            dataSizeA[i] = wavBuffer[offset++];
        }
        dataSize = unsignedCharArrayToInt(dataSizeA);
        std::cout << "Data Size: " << dataSize << std::endl;
        std::cout << "Loading data..." << std::endl;
        //offset += sizeof(long);
        std::cout << "Position: " << offset << std::endl;
        std::cout << "Remaining size: " << (wavBuffer.size() - offset) << std::endl;
        data = new unsigned char[dataSize];
        for(int i = 0; i < dataSize; i++) {
            //std::cout << "i: " << i << std::endl;
            data[i] = wavBuffer[offset++];
        }
        std::cout << "Data loaded.  Final offset: " << offset << std::endl;

        ALsizei size = dataSize;
        std::cout << "Buffer size: " << size << std::endl << std::flush;
        ALsizei frequency = sampleRate;
        ALenum format = getFormat();

        alGenBuffers(1, &buffer);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }

        alBufferData(buffer, format, data, size, frequency);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "#1 Error: " << alGetString(err) << std::endl;
            delete[] data;
            return;
        }

        alSourcei(source, AL_BUFFER, buffer);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "#1 Error: " << alGetString(err) << std::endl;
            return;
        }

        alSourcePlay(source);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            std::cout << "Error: " << alGetString(err) << std::endl;
            return;
        }
        delete[] data;

        ALint val;
        do {
            alGetSourcei(source, AL_SOURCE_STATE, &val);
        } while(val == AL_PLAYING);

        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }*/

    void WAVFile::loadImpl() {
        // First get datastream from Ogre3d
        Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
        // Create vector
        std::vector<unsigned char> wavBuffer = std::vector<unsigned char>(stream->size());
        // Copy stream into vector
        stream->read(&wavBuffer[0], stream->size());
        stream->close();

        unsigned char riff[4] = { 'R', 'I', 'F', 'F' };
        unsigned int offset = 0;
        for(unsigned int i = offset; i < 4; i++) {
            if(wavBuffer[i] != riff[i]) {
                LOG(mName + " - Invalid wave: " + mName);
                return;
            }
        }
        offset += sizeof(long) + 4;
        unsigned char riffFormat[4] = { 'W', 'A', 'V', 'E' };
        for(unsigned int i = 0; i < 4; i++) {
            if(wavBuffer[i + offset] != riffFormat[i]) {
                LOG(mName + " - Invalid wave" + mName);
                return;
            }
        }
        offset += 4;
        unsigned char waveFormat[4] = { 'f', 'm', 't', ' ' };
        for(unsigned int i = 0; i < 4; i++) {
            if(wavBuffer[i + offset] != waveFormat[i]) {
                LOG(mName + " - Invalid wave" + mName);
                return;
            }
        }
        offset += sizeof(long) * 2;
        unsigned char audForm[2];
        audForm[1] = wavBuffer[offset++];
        audForm[0] = wavBuffer[offset++];
        audioFormat = unsignedCharArrayToShort(audForm);
        LOG(mName + " - Audio Format (1 is PCM): " + Ogre::StringConverter::toString(audioFormat));
        unsigned char numChan[2];
        numChan[1] = wavBuffer[offset++];
        numChan[0] = wavBuffer[offset++];
        numChannels = unsignedCharArrayToShort(numChan);
        LOG(mName + " - Number of Channels: " + Ogre::StringConverter::toString(numChannels));
        unsigned char samRate[4];
        for(int i = 3; i >= 0; i--) {
            samRate[i] = wavBuffer[offset++];
        }
        sampleRate = unsignedCharArrayToInt(samRate);
        LOG(mName + " - Sample Rate: " + Ogre::StringConverter::toString(sampleRate));
        unsigned char byRate[4];
        for(int i = 3; i >= 0; i--) {
            byRate[i] = wavBuffer[offset++];
        }
        byteRate = unsignedCharArrayToInt(byRate);
        LOG(mName + " - Byte Rate: " + Ogre::StringConverter::toString(byteRate));
        unsigned char byteAlignA[2];
        for(int i = 1; i >= 0; i--) {
            byteAlignA[i] = wavBuffer[offset++];
        }
        byteAlign = unsignedCharArrayToShort(byteAlignA);
        LOG(mName + " - Byte Align: " + Ogre::StringConverter::toString(byteAlign));
        unsigned char bitsPerSampleA[2];
        for(int i = 1; i >= 0; i--) {
            bitsPerSampleA[i] = wavBuffer[offset++];
        }
        bitsPerSample = unsignedCharArrayToShort(bitsPerSampleA);
        LOG(mName + " - Bits Per Sample: " + Ogre::StringConverter::toString(bitsPerSample));
        unsigned char dataStart[4] = { 'd', 'a', 't', 'a' };
        for(unsigned int i = 0; i < 4; i++) {
            if(wavBuffer[i + offset] != dataStart[i]) {
                LOG(mName + " - Invalid wave");
                validWave = false;
                return;
            }
        }
        offset += sizeof(long);
        unsigned char dataSizeA[4];
        for(int i = 3; i >= 0; i--) {
            dataSizeA[i] = wavBuffer[offset++];
        }
        dataSize = unsignedCharArrayToInt(dataSizeA);
        LOG(mName + " - Data Size: " + Ogre::StringConverter::toString(dataSize));

        data = new unsigned char[dataSize];
        for(int i = 0; i < dataSize; i++) {
            //std::cout << "i: " << i << std::endl;
            data[i] = wavBuffer[offset++];
        }
        LOG(mName + " - Data loaded");
        validWave = true;
    }

    bool WAVFile::playWAV() {
        if(!validWAV() || isPlaying())
            return false;

        device = alcOpenDevice(NULL);
        ALenum err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }

        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }

        alListener3f(AL_POSITION, 0, 0, 0);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }
        alListener3f(AL_VELOCITY, 0, 0, 0);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }
        ALfloat	listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};
        alListenerfv(AL_ORIENTATION,listenerOri);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }
        //ALuint source;
        //ALuint buffer;
        alGenSources(1, &source);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }

        alSourcef(source, AL_PITCH, 1);
        alSourcef(source, AL_GAIN, 1);
        alSource3f(source, AL_POSITION, 0, 0, 0);
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSourcei(source, AL_LOOPING, AL_FALSE);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }

        ALsizei size = dataSize;
        ALsizei frequency = sampleRate;
        ALenum format = getFormat();

        alGenBuffers(1, &buffer);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }

        alBufferData(buffer, format, data, size, frequency);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }

        alSourcei(source, AL_BUFFER, buffer);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }

        alSourcePlay(source);
        err = alGetError();
        if(err != AL_NO_ERROR) {
            return false;
        }
        return true;
    }

    bool WAVFile::isPlaying() {
        ALint val;
        alGetSourcei(source, AL_SOURCE_STATE, &val);
        return val == AL_PLAYING;
    }

    void WAVFile::close() {
        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    ALenum WAVFile::getFormat() {
        if (numChannels == 1) {
                LOG("Channel: 1");
            if (bitsPerSample == 8) {
                LOG("Format: 8");
                return AL_FORMAT_MONO8;
            } else if (bitsPerSample == 16) {
                LOG("Format: 16");
                return AL_FORMAT_MONO16;
            }
        } else if (numChannels == 2) {
            LOG("Channel: 2");
            if (bitsPerSample == 8) {
                LOG("Format: 8");
                return AL_FORMAT_STEREO8;
            } else if (bitsPerSample == 16) {
                LOG("Format: 16");
                return AL_FORMAT_STEREO16;
            }
        }
        return AL_INVALID_VALUE;
    }

    /*void WAVFile::getWAV(ALuint *buffer, ALsizei *size, ALsizei *frequency, ALenum *format) {
        // Now we set the variables that we passed in with the
        // data from the structs
        *size = wave_data.dataSize;
        LOG(Ogre::StringConverter::toString(wave_data.dataSize));
        *frequency = wave_format.sampleRate;

        // The format is worked out by looking at the number of
        // channels and the bits per sample.
        if (wave_format.numChannels == 1) {
                LOG("Channel: 1");
            if (wave_format.bitsPerSample == 8) {
                *format = AL_FORMAT_MONO8;
                LOG("Format: 8");
            } else if (wave_format.bitsPerSample == 16) {
                *format = AL_FORMAT_MONO16;
                LOG("Format: 16");
            }
        } else if (wave_format.numChannels == 2) {
            LOG("Channel: 2");
            if (wave_format.bitsPerSample == 8) {
                *format = AL_FORMAT_STEREO8;
                LOG("Format: 8");
            } else if (wave_format.bitsPerSample == 16) {
                *format = AL_FORMAT_STEREO16;
                LOG("Format: 16");
            }
        }

        //LOG(Ogre::StringConverter::toString(WAVData.size()));
        //LOG(Ogre::StringConverter::toString(wave_data.subChunk2Size));

        try {
            // Create our OpenAL buffer and check for success
            alGenBuffers(1, buffer);
            ALenum err = alGetError();
            if (err != AL_NO_ERROR)
                throw("Error getting buffer");

            // Now we put our data into the openAL buffer and
            // check for success
            alBufferData(*buffer, *format, (void*)WAVData,
                         *size, *frequency);
            err = alGetError();
            if (err != AL_NO_ERROR)
                throw("Error loading buffer");
            LOG(mName + " - WAV Data Buffered");
        } catch (std::string error) {
            LOG(error);
        }
    }*/

    void WAVFile::unloadImpl() {
        // TODO: Need to update this so it clears ALL RESOURCE DATA
        delete[] data;
        //delete[] WAVData;
    }

    size_t WAVFile::calculateSize() const {
        // Calculate the total memory used by this class
        // And send it to resource manager
        return dataSize;
    }
}
#endif
