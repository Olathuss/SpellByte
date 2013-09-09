/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Cube27 ResourceFile Class
 *
 * This class manages the interface
 * between Ogre::Resource and Cube27 Resource Files
 *
 */

#include "CubeResourceFile.h"
#include "../utilities/easyzlib.h"
#include "../utilities/utils.h"
#include "file_defines.h"
#include "ManualLoaders.h"
#include "../SpellByte.h"

namespace SpellByte
{
    Cube27ResFile::Cube27ResFile(Ogre::ResourceManager *creator, const Ogre::String &name,
                  Ogre::ResourceHandle handle, const Ogre::String &group,
                  bool isManual, Ogre::ManualResourceLoader *loader) :
    Ogre::Resource(creator, name, handle, group, isManual, loader)
    {
        createParamDictionary("Cube27ResourceFile");
    }

    Cube27ResFile::~Cube27ResFile()
    {
        unload();
    }

    void Cube27ResFile::loadImpl()
    {
        // First get datastream from Ogre3d
        Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
        // Create vector
        std::vector<unsigned char> fileVector(stream->size());
        // Copy stream into vector
        stream->read(&fileVector[0], stream->size());
        stream->close();

        // Offset used to keep track of position in vector
        unsigned long offset = 0;
        // Copy signature from file into resource header
        ReadFromVector(fHeader.sig, 0, fileVector);
        offset += sizeof(fHeader.sig);

        // Compare and verify header, if not throw exception
        if(fHeader.sig != fHeader.SIGNATURE)
        {
            throw(Ogre::Exception(15, "Error, invalid resource file.", "ResourceFile"));
        }

        // Get version
        ReadFromVector(fHeader.version, offset, fileVector);
        // For now just ignore the version.
        offset += sizeof(fHeader.version);

        // Find out how many files are packed into the resource file
        ReadFromVector(dataCount, offset, fileVector);
        offset += sizeof(dataCount);
        long fileCount = dataCount;//dwordToLong(header.dataCount);
        // Loop through all files and unpack them into resource
        for(long i = 0; i < fileCount; ++i)
        {
            ResFileData tmp;
            // Get length of resource's file name
            ReadFromVector(tmp.fileNameLen, offset, fileVector);
            offset += sizeof(tmp.fileNameLen);
            // Get the filename
            char* buffer = new char[tmp.fileNameLen + 1];
            // copy it into the buffer
            memcpy(buffer, &fileVector[offset], tmp.fileNameLen);
            // Append character to signify end of string, otherwise
            // there will be a crash later on
            buffer[tmp.fileNameLen] = 0;
            // Make a string and add it to file data
            tmp.fileName = Ogre::String(buffer);
            offset += tmp.fileNameLen;
            // Get file format
            ReadFromVector(tmp.dataFormat, offset, fileVector);
            offset += sizeof(tmp.dataFormat);
            // Get data to determine if file is compressed or not
            ReadFromVector(tmp.compressed, offset, fileVector);
            offset += sizeof(tmp.compressed);
            // Get length of file
            ReadFromVector(tmp.dataLength, offset, fileVector);
            offset += sizeof(tmp.dataLength);

            // Copy file data into vector
            std::vector<unsigned char> tmpData(fileVector.begin() + offset, fileVector.begin() + offset + tmp.dataLength);
            tmp.data = tmpData;
            offset += tmpData.size();
            if(tmp.compressed == COMPRESSED)
            {
                ReadFromVector(tmp.decompressedLength, offset, fileVector);
                offset += sizeof(tmp.decompressedLength);
            }
            // Push file struct into vector
            Data.push_back(tmp);
        }
        init();
    }

    void Cube27ResFile::init()
    {
        for(unsigned int i = 0; i < Data.size(); ++i)
        {
            LOG("Cube27Resource declaring resource: " + Ogre::String(Data[i].fileName));
            if(Data[i].dataFormat == MESHFile)
            {
                manualMeshLoader *mml = new manualMeshLoader(this->getName(), i);
                Ogre::MeshManager::getSingleton().createManual(Data[i].fileName, "Cube27Resource", mml);
            }
            if(Data[i].dataFormat == XMLFile)
            {
                manualXMLLoader *xml = new manualXMLLoader(this->getName(), i);
                Ogre::ResourceGroupManager::getSingleton().declareResource(Data[i].fileName, "XMLResource", "General", xml);
            }
        }
    }

    void Cube27ResFile::unloadImpl()
    {
        // TODO: Need to update this so it clears ALL RESOURCE DATA
    }

    size_t Cube27ResFile::calculateSize() const
    {
        // Calculate the total memory used by this class
        // And send it to resource manager
        size_t mySize = sizeof(fHeader);
        ResFileData tmpForSizeof;
        mySize += sizeof(tmpForSizeof.fileNameLen) * Data.size();
        mySize += sizeof(tmpForSizeof.dataFormat) * Data.size();
        mySize += sizeof(tmpForSizeof.compressed) * Data.size();
        mySize += sizeof(tmpForSizeof.dataLength) * Data.size();
        for(unsigned int i = 0; i < Data.size(); ++i)
        {
            mySize += Data[i].fileName.size();
            mySize += Data[i].dataLength;
        }
        return mySize;
    }

    // Pass of datasize of specific resource name
    // This can be optimized later to use the actual
    // position and not have to iterate through vector
    size_t Cube27ResFile::getDataSize(Ogre::String name) const
    {
        for(unsigned int i = 0; i < Data.size(); ++i)
        {
            if(Data[i].fileName == name)
            {
                if(Data[i].compressed == NOT_COMPRESSED)
                    return Data[i].dataLength;
                else if(Data[i].compressed == COMPRESSED)
                    return Data[i].decompressedLength;
            }
        }
        return 0;
    }

    // Similar to above except returns data
    char *Cube27ResFile::getData(Ogre::String name) const
    {
        for(unsigned int i = 0; i < Data.size(); ++i)
        {
            if(Data[i].fileName == name)
            {
                if(Data[i].compressed == COMPRESSED)
                {
                    std::vector<unsigned char> unCompressed = decompress(i);
                    char* tmp = new char[unCompressed.size()];
                    memcpy(tmp, &unCompressed[0], unCompressed.size());
                    return tmp;
                }
                else
                {
                    char* tmp = new char[Data[i].dataLength];
                    memcpy(tmp, &Data[i].data[0], Data[i].dataLength);
                    return tmp;
                }
            }
        }
        return NULL;
    }

    // This is kept here to demonstrate how to
    // uncompress data within the resource file
    std::vector<unsigned char> Cube27ResFile::decompress(int data_no) const
    {
        // Get data length
        ezbuffer bufSrc( Data[data_no].dataLength );
        // Copy data from vector into buffer
        memcpy(bufSrc.pBuf, &Data[data_no].data[0], bufSrc.nLen);
        ezbuffer bufDest;

        // Decompress data
        int ezErr = ezuncompress( bufDest, bufSrc );

        // If error, throw ogre exception
        if(ezErr < 0)
            throw(Ogre::Exception(15, "Error during decompression", "ResourceFile"));

        // Copy uncompressed data into vector and return
        std::vector<unsigned char> uctmp( bufDest.pBuf, bufDest.pBuf + bufDest.nLen );
        return uctmp;
    }
}
