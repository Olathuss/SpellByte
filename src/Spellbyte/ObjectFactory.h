
#ifndef _OBJECT_FObjectY_H_
#define _OBJECT_FObjectY_H_

#include <Terrain/OgreTerrainGroup.h>

#include "utilities/tinyxml2.h"
#include "Object.h"

namespace SpellByte {
    typedef int ObjectID;

    class ObjectFactory {
        ObjectID lastObjectID;

    protected:

    public:
        // Used to snap Object to terrain, relative to terrain,
        // or absolute y position
        enum Y_SNAP {
            Y_TERRAIN,
            Y_RELATIVE,
            Y_ABSOLUTE,
            Y_INVALID
        };
        ObjectFactory(void);
        ~ObjectFactory();

        Object* createObject(tinyxml2::XMLElement* xmlRoot, Ogre::SceneNode *parentNode = NULL);

    protected:
        //virtual ComponentPtr createComponent(tinyxml2::XMLNode *xmlElement);

    private:
        ObjectID getNextObjectID(void) { ++lastObjectID; return lastObjectID; };
    };
}

#endif // _OBJECT_FACTORY_H_
