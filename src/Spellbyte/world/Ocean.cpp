#include "Ocean.h"

namespace SpellByte {
    static int Ocean::Next_ID = 0;

    Ocean::Ocean():plane_d(0), width(100), height(100), xsegments(1), ysegments(1), normals(true), numTexCoordSets(1),
                    uTile(1.0f), vTile(1.0f) {
        ID = Next_ID++;
    }

    void Ocean::init(tinyxml2::XMLElement *oceanElt) {
        Ogre::Plane plane(Ogre::Vector3::UNIT_Y, plane_d);

        Ogre::MeshManager::getSingleton().createPlane("ocean" + Ogre::StringConverter::toString(ID), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                plane, width, height, xsegments, ysegments, normals, numTexCoordSets, uTile, vTile, Ogre::Vector3::UNIT_Z);
    }

    Ocean::~Ocean() {
        Ogre::MeshManager::getSingleton().unload("ocean" + Ogre::StringConverter::toString(ID));

        Ogre::MeshManager::getSingleton().remove("ocean" + Ogre::StringConverter::toString(ID));
    }
}
