#include <OgreSubEntity.h>

#include "ActorFactory.h"
#include "define.h"
#include "World.h"
#include "ActorManager.h"

namespace SpellByte {
    ActorFactory::ActorFactory(void) {
        // Nothing here!
    }

    ActorFactory::~ActorFactory() {
        // Nothing here!
    }

    Actor *ActorFactory::createActor(Ogre::SceneManager *SceneMgr,
                                     World *worldPtr) {

        // Create actor and set unique id
        Actor *newActor = ActorMgr->getFreeActor();

        newActor->WorldPtr = worldPtr;

        std::vector<Ogre::String> entParts = ActorMgr->getRandomSkin();
        newActor->setVisibleParts(entParts);

        Ogre::Vector3 pos = Ogre::Vector3(0, 0, 0);
        int negativex = 1 ? rand() % 1 == 0: -1;
        int negativez = 1 ? rand() % 1 == 0: -1;
        pos.x = -117 + rand() % 50 * (negativex);
        pos.z = 272 + rand() % 50 * (negativez);
        worldPtr->setVector3Height(pos);
        newActor->setPositionXYZ(pos.x, pos.y, pos.z);
        //newActor->SceneNode->setPosition(Position);
        newActor->activate();
        newActor->setAnimation("NPCTalking", true, false);

        return newActor;
    }
}
