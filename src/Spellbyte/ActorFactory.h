#ifndef _ACTOR_FACTORY_H_
#define _ACTOR_FACTORY_H_

#include <Terrain/OgreTerrainGroup.h>

#include "Actor.h"
#include "utilities/tinyxml2.h"

namespace SpellByte {
    class ActorFactory {
        ActorID lastActorID;

    protected:

    public:
        ActorFactory(void);
        ~ActorFactory();

        // Method used to create Actors
        Actor *createActor(Ogre::SceneManager *SceneMgr, World *worldPtr);

    protected:

    private:
        // Get UniqueID for Actor
        ActorID getNextActorID(void) { ++lastActorID; return lastActorID; };
    };
}

#endif // _ACTOR_FACTORY_H_
