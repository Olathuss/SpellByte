
#include "ObjectFactory.h"
#include "SpellByte.h"

namespace SpellByte {
    ObjectFactory::ObjectFactory(void) {
        lastObjectID = 1;
    }

    ObjectFactory::~ObjectFactory() {
    }

    Object* ObjectFactory::createObject(tinyxml2::XMLElement* xmlRoot,
                                        Ogre::SceneNode *parentNode) {
        Object *newObject = new Object();
        newObject->init(xmlRoot, parentNode);

        return newObject;
    }
}
