#ifndef _ACTOR_FACTORY_H_
#define _ACTOR_FACTORY_H_

#include <Terrain/OgreTerrainGroup.h>

#include "Actor.h"
#include "utilities/tinyxml2.h"
#include "DotSceneLoader.h"

namespace SpellByte
{
    class ActorFactory
    {
        ActorID lastActorID;

    protected:

    public:
        ActorFactory(void);
        ~ActorFactory();

        Actor *createActor(Ogre::SceneManager *SceneMgr, Ogre::SceneNode *parentNode, Ogre::TerrainGroup *tGrp, tinyxml2::XMLElement* xmlRoot);

    protected:
        //virtual ComponentPtr createComponent(tinyxml2::XMLNode *xmlElement);
        bool loadPosition(tinyxml2::XMLElement *elt, float &x, float &y, float &z);
        bool loadScale(tinyxml2::XMLElement *elt, float &x, float &y, float &z);
        bool loadRotate(tinyxml2::XMLElement *elt, float &roll, float &pitch, float &yaw);

        Ogre::DotSceneLoader *dsl;

    private:
        ActorID getNextActorID(void) { ++lastActorID; return lastActorID; };
    };
}

#endif // _ACTOR_FACTORY_H_
