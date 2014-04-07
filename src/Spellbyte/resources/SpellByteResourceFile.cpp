/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * SpellByte ResourceFile Class
 *
 * This class manages the interface
 * between Ogre::Resource and SpellByte Resource Files
 *
 */

#include "SpellByteResourceFile.h"
#include "../utilities/easyzlib.h"
#include "../utilities/utils.h"
#include "file_defines.h"
#include "ManualLoaders.h"
#include "../SpellByte.h"

namespace SpellByte {
    SBResFile::SBResFile(Ogre::ResourceManager *creator, const Ogre::String &name,
                  Ogre::ResourceHandle handle, const Ogre::String &group,
                  bool isManual, Ogre::ManualResourceLoader *loader) :
    Ogre::Resource(creator, name, handle, group, isManual, loader) {
        createParamDictionary("SBResourceFile");
    }

    SBResFile::~SBResFile() {
        unload();
    }

    void SBResFile::loadImpl() {
        // First get datastream from Ogre3d
        Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
        /*
            Create vector to contain file data for easy array like access
            Set size of vector so it doesn't have to resize during population
            as this will slow down the loading process due to reallocation
        */
        std::vector<unsigned char> fileVector(stream->size());
        // Copy stream into vector
        stream->read(&fileVector[0], stream->size());
        stream->close();

        // Offset used to keep track of position in vector
        unsigned long offset = 0;
        // Copy signature from file into resource header
        ReadFromVector(fHeader.sig, 0, fileVector);
        offset += sizeof (fHeader.sig);

        // Compare and verify header, if not throw exception
        if (fHeader.sig != fHeader.SIGNATURE) {
            throw(Ogre::Exception(15, "Error, invalid resource file.", "ResourceFile"));
        }

        // Get resource file version
        ReadFromVector(fHeader.version, offset, fileVector);
        // For now just ignore the version.
        offset += sizeof (fHeader.version);

        // Find out how many files are packed into the resource file
        ReadFromVector(dataCount, offset, fileVector);
        offset += sizeof (dataCount);
        // Loop through all files and unpack them into resource
        for (unsigned long i = 0; i < dataCount; ++i) {
            ResFileData tmp;
            // Get length of resource's file name
            ReadFromVector(tmp.fileNameLen, offset, fileVector);
            offset += sizeof (tmp.fileNameLen);
            // Get the filename
            char* buffer = new char[tmp.fileNameLen + 1];
            // copy it into the buffer
            memcpy(buffer, &fileVector[offset], tmp.fileNameLen);
            // Append character to signify end of c-string, otherwise
            // there will be a crash later on
            buffer[tmp.fileNameLen] = '\0';
            // Make a string and add it to file data
            tmp.fileName = Ogre::String(buffer);
            offset += tmp.fileNameLen;
            // Get file format
            ReadFromVector(tmp.dataFormat, offset, fileVector);
            offset += sizeof (tmp.dataFormat);
            // Get data to determine if file is compressed or not
            ReadFromVector(tmp.compressed, offset, fileVector);
            offset += sizeof (tmp.compressed);
            // Get length of file
            ReadFromVector(tmp.dataLength, offset, fileVector);
            offset += sizeof (tmp.dataLength);

            // Copy file data into vector
            std::vector<unsigned char> tmpData(fileVector.begin() + offset, fileVector.begin() + offset + tmp.dataLength);
            tmp.data = tmpData;
            offset += tmpData.size();
            if(tmp.compressed == COMPRESSED) {
                ReadFromVector(tmp.decompressedLength, offset, fileVector);
                offset += sizeof(tmp.decompressedLength);
            }

            /*
                Inform Ogre3D's Resource Manager that a new resource is available for loading,
                when the resource is then loaded during runtime, resource manager will use the
                appropriate manual resource loader for the resource.
            */
            LOG("SBResource declaring resource: " + Ogre::String(tmp.fileName));
            if (tmp.dataFormat == MESHFile) {
                // File is MESH file, so declare manual mesh resource.
                manualMeshLoader *mml = new manualMeshLoader(this->getName(), (unsigned int)i);
                Ogre::MeshManager::getSingleton().createManual(tmp.fileName, "SpellByteResource", mml);
            }
            if(tmp.dataFormat == XMLFile) {
                // Resource is XML file, so declare manual xml loader.
                manualXMLLoader *xml = new manualXMLLoader(this->getName(), (unsigned int)i);
                Ogre::ResourceGroupManager::getSingleton().declareResource(tmp.fileName, "XMLResource", "General", xml);
            }

            // Push file struct into vector
            Data.push_back(tmp);
        }
    }

    void SBResFile::unloadImpl() {
        // TODO: Need to update this so it clears ALL RESOURCE DATA
    }

    size_t SBResFile::calculateSize() const {
        // Calculate the total memory used by this class
        // And send it to resource manager
        size_t totalSize = sizeof (ResFileHeader) + sizeof (ResFileData) * Data.size();
        for (unsigned int i = 0; i < Data.size(); ++i) {
            totalSize += Data[i].fileName.size();
            totalSize += Data[i].dataLength;
        }
        return totalSize;
    }

    /*
        Get the data and its allocated size
        Position is optional and used to optimized so the resource
        doesn't have to search for it.
    */
    char *SBResFile::getData(Ogre::String name, size_t &dataSize, int position) const {
        unsigned int filePosition = 0;
        if (position < 0) {
            for (filePosition = 0; filePosition < Data.size(); ++filePosition)
                if (Data[filePosition].fileName == name)
                    break;
        } else {
            filePosition = (unsigned int)position;
            if (Data[filePosition].fileName != name) {
                throw(Ogre::Exception(15, "Invalid resource filename", "ResourceFile"));
            }
        }

        // Check whether data is compressed
        if (Data[filePosition].compressed == COMPRESSED) {
            // Set datasize equal to file's decompressed size/length
            dataSize = Data[filePosition].decompressedLength;
            // Decompress data and return it
            return reinterpret_cast<char*>(decompress(filePosition));
        } else {
            dataSize = Data[filePosition].dataLength;
            // Receiver is responsible for deleting char*
            char* tmp = new char[Data[filePosition].dataLength];
            // Copy data from vector into char*
            memcpy(tmp, &Data[filePosition].data[0], Data[filePosition].dataLength);
            return tmp;
        }
        // Safeguard, return NULL if an unknown problem occurs
        return NULL;
    }

    // Helper method to decompress resource data
    unsigned char* SBResFile::decompress(unsigned int data_index) const {
        // Get data length
        ezbuffer bufSrc(Data[data_index].dataLength);
        // Copy data from vector into buffer
        memcpy(bufSrc.pBuf, &Data[data_index].data[0], bufSrc.nLen);
        ezbuffer bufDest;

        // Decompress data
        int ezErr = ezuncompress( bufDest, bufSrc );

        // If error, throw ogre exception
        if (ezErr < 0)
            throw(Ogre::Exception(15, "Error during decompression", "ResourceFile"));

        // Return
        return bufDest.pBuf;
    }
}
