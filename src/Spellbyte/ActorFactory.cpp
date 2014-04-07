#include <OgreSubEntity.h>

#include "ActorFactory.h"
#include "define.h"
#include "World.h"
#include "ActorManager.h"

namespace SpellByte {
    ActorFactory::ActorFactory(void) {
        lastActorID = Actor::getNextValidID();
    }

    ActorFactory::~ActorFactory() {
    }

    Actor *ActorFactory::createActor(Ogre::SceneManager *SceneMgr,
                                     World *worldPtr) {

        // Create actor and set unique id
        Actor *newActor = ActorMgr->getFreeActor();

        newActor->WorldPtr = worldPtr;

        std::vector<Ogre::String> entParts = ActorMgr->getRandomSkin();
        /*entParts.push_back("BaseArms");
        entParts.push_back("BaseHands");
        entParts.push_back("BaseHairC");
        entParts.push_back("Head");
        entParts.push_back("Teeth");
        entParts.push_back("Lowerteeth");
        entParts.push_back("ThiefHood");
        entParts.push_back("ThiefTorso");
        entParts.push_back("ThiefTrousers_MinerMeshExchange_");
        entParts.push_back("MinerBoots");*/
        //entParts.push_back("FarmerTorso");das

        for (unsigned int i = 0; i < entParts.size(); ++i) {
            Ogre::SubEntity *sEnt;
            sEnt = newActor->ActorEntity->getSubEntity(entParts[i]);
            sEnt->setVisible(true);
        }
        Ogre::Vector3 pos = Ogre::Vector3(0, 0, 0);
        int negativex = 1 ? rand() % 1 == 0: -1;
        int negativez = 1 ? rand() % 1 == 0: -1;
        pos.x = -117 + rand() % 50 * (negativex);
        pos.z = 272 + rand() % 50 * (negativez);
        worldPtr->setVector3Height(pos);
        newActor->setPositionXYZ(pos.x, pos.y, pos.z);
        //newActor->SceneNode->setPosition(Position);
        newActor->setAnimation("NPCTalking", true, true);
        newActor->activate();

        return newActor;
    }
}
