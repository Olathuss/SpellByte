
#ifndef _OBJECT_FObjectY_H_
#define _OBJECT_FObjectY_H_

#include <Terrain/OgreTerrainGroup.h>

#include "utilities/tinyxml2.h"
#include "DotSceneLoader.h"
#include "Object.h"

namespace SpellByte
{
    typedef int ObjectID;

    class ObjectFactory
    {
        ObjectID lastObjectID;

    protected:

    public:
        enum Y_SNAP
        {
            Y_TERRAIN,
            Y_RELATIVE,
            Y_ABSOLUTE,
            Y_INVALID
        };
        ObjectFactory(void);
        ~ObjectFactory();

        Object* createObject(tinyxml2::XMLElement* xmlRoot, Ogre::SceneNode *parentNode = NULL);
        bool createObjectDefunct(Ogre::SceneManager *SceneMgr, Ogre::SceneNode *parentNode, Ogre::TerrainGroup *tGrp, tinyxml2::XMLElement* xmlRoot);
        bool loadPosition(tinyxml2::XMLElement *elt, float &x, float &y, float &z, int &ysnap);

    protected:
        //virtual ComponentPtr createComponent(tinyxml2::XMLNode *xmlElement);
        bool loadScale(tinyxml2::XMLElement *elt, float &x, float &y, float &z);
        bool loadRotate(tinyxml2::XMLElement *elt, float &roll, float &pitch, float &yaw);

        Ogre::DotSceneLoader *dsl;

    private:
        ObjectID getNextObjectID(void) { ++lastObjectID; return lastObjectID; };
    };
}

#endif // _OBJECT_FACTORY_H_
