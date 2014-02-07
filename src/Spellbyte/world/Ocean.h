#ifndef SB_OCEAN_H
#define SB_OCEAN_H

#include "../stdafx.h"
#include "../utilities/tinyxml2.h"

namespace SpellByte {
    class Ocean {
    public:
        Ocean();
        void init(tinyxml2::XMLElement *oceanElt);

        ~Ocean();

    private:
        static int Next_ID;
        int ID;

        Ogre::Real plane_d;
        Ogre::Real width;
        Ogre::Real height;
        int xsegments;
        int ysegments;
        bool normals;
        unsigned short numTexCoordSets;
        Ogre::Real uTile;
        Ogre::Real vTile;
    };
}

#endif // SB_OCEAN_H
