/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * Manual Loaders Classes
 *
 * This class defines the manual loading
 * of resources such as meshes, images, etc.
 *
 */

#ifndef _MANUAL_LOADERS_H_
#define _MANUAL_LOADERS_H_

#include <iostream>
#include "../stdafx.h"
#include "SpellByteResourceFileManager.h"
#include "XMLResource.h"
#include "../define.h"

using namespace SpellByte;

// Allows a mesh to be loaded from memory (i.e. C27 Resource file)
// rather than a .mesh file.
 class manualMeshLoader : public Ogre::ManualResourceLoader
 {
 private:
    // Name of resource containing mesh
    Ogre::String resFileName;
    // Position where resource is contained within resource file
    // can be used for optimization, not used yet.
    unsigned int resPos;

 public:
    // Default constructor, probably shouldn't be used
    manualMeshLoader():resFileName(""), resPos(-1) {}
    // Primary constructor that should be used
    // so resource file name and position can be set
    manualMeshLoader(Ogre::String name, unsigned int position)
    {
        resFileName = name;
        resPos = position;
    }
    // Empty destructor
    virtual ~manualMeshLoader() {}

    // This is the primary function which will load the actual mesh
    virtual void loadResource(Ogre::Resource *meshRes)
    {
        LOG("Loading resource: " + Ogre::String(meshRes->getName()));
        // Cast our Ogre::Resource object to a Mesh resource
        Ogre::Mesh *mesh = static_cast<Ogre::Mesh *>(meshRes);
        // Ensure the proper Resource File is loaded
        SBResFilePtr resPtr = SBResFileManager::getSingleton().load(resFileName, "SBResourceFile");
        // Get the name of our file
        size_t len;// = resPtr->getDataSize(mesh->getName());
        // Allocate proper memory to contain mesh data
        char* dataStream;// = new char[len];
        // Get the actual data
        dataStream = resPtr->getData(mesh->getName(), len, resPos);

        // Although using MemoryDataStream to contain the mesh data
        // importMesh only takes a DataStreamPtr so we have to
        // create the Ptr from the MemoryDataStream
        Ogre::DataStreamPtr streamPtr(new Ogre::MemoryDataStream(dataStream, len));

        // Create a MeshSerializer to serialize the mesh data from the DataStream
        Ogre::MeshSerializer tmp;
        // Import the mesh from the actual data
        tmp.importMesh(streamPtr, mesh);
        // Make sure our allocated char* gets deleted
        delete[] dataStream;
    }
 };

 // Allows a mesh to be loaded from memory (i.e. C27 Resource file)
// rather than a .mesh file.
 class manualXMLLoader : public Ogre::ManualResourceLoader
 {
 private:
    // Name of resource containing mesh
    Ogre::String resFileName;
    // Position where resource is contained within resource file
    // can be used for optimization, not used yet.
    unsigned int resPos;

 public:
    // Default constructor, probably shouldn't be used
    manualXMLLoader():resFileName(""), resPos(-1) {}
    // Primary constructor that should be used
    // so resource file name and position can be set
    manualXMLLoader(Ogre::String name, unsigned int position)
    {
        resFileName = name;
        resPos = position;
    }
    // Empty destructor
    virtual ~manualXMLLoader() {}

    // This is the primary function which will load the actual mesh
    virtual void loadResource(Ogre::Resource *xmlRes)
    {
        // Cast our Ogre::Resource object to a Mesh resource
        XMLResource *xmlResource = static_cast<XMLResource *>(xmlRes);
        // Ensure the proper Resource File is loaded
        SBResFilePtr resPtr = SBResFileManager::getSingleton().load(resFileName, "SBResourceFile");
        // Get the name of our file
        size_t len;// = resPtr->getDataSize(xmlResource->getName());
        // Allocate proper memory to contain mesh data
        char* dataStream;// = new char[len];
        // Get the actual data
        dataStream = resPtr->getData(xmlResource->getName(), len, resPos);

        xmlResource->xmlFile.Parse( dataStream, len );

        delete[] dataStream;
    }
 };

#endif // _MANUAL_LOADERS_H_
