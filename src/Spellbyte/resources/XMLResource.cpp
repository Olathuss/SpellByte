/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Cube27 XMLResource Class
 *
 * This class manages the interface
 * between Ogre::Resource and XML Files
 *
 */

#include "XMLResource.h"
#include "ManualLoaders.h"
#include "../Application.h"
#include "../SpellByte.h"

XMLResource::XMLResource(Ogre::ResourceManager *creator, const Ogre::String &name,
                  Ogre::ResourceHandle handle, const Ogre::String &group,
                  bool isManual, Ogre::ManualResourceLoader *loader) :
    Ogre::Resource(creator, name, handle, group, isManual, loader)
    {
        createParamDictionary("XMLResource");
    }

XMLResource::~XMLResource()
{
    unload();
}

void XMLResource::loadImpl()
{
    // First get datastream from Ogre3d
    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
    // Create vector
    std::vector<unsigned char> fileVector(stream->size());
    // Copy stream into vector

    stream->read(&fileVector[0], stream->size());
    stream->close();

    dataSize = fileVector.size();

    char* xml = new char[dataSize];
    memcpy(xml, &fileVector[0], dataSize);

    xmlFile.Parse( xml, dataSize );

    delete[] xml;
}

void XMLResource::unloadImpl()
{
    // TODO: Need to update this so it clears ALL RESOURCE DATA
}

size_t XMLResource::calculateSize() const
{
    return dataSize;
}
