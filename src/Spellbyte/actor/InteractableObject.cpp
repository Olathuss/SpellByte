#include "InteractableObject.h"
#include <define.h>
#include <World.h>
#include <utilities/Telegram.h>
#include <utilities/Messages.h>
#include <utilities/MessageDispatcher.h>

namespace SpellByte {
    Coffin::Coffin():Interactable() {
        ActorNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode("COFFIN");
        ActorAny = new UserAny(UserAny::ACTOR, -2);
        ActorNode->setUserAny(Ogre::Any(ActorAny));
    }

    void Coffin::reset() {
        if (ActorNode) {
            ActorNode->removeAndDestroyAllChildren();
        }
    }

    void Coffin::init(World *worldptr) {
        if(ActorNode) {
            WorldPtr = worldptr;
            ActorEntity = APP->SceneMgr->createEntity("COFFIN", "coffin.mesh", "Objects");
            ActorEntity->setQueryFlags(World::COLLISION_MASK::ACTOR);
            ActorNode->attachObject(ActorEntity);
            ActorNode->setPosition(-41, WorldPtr->getHeight(-41, 0, 339.9), 339.9);
            ActorNode->yaw(Ogre::Degree(-90));

        }
    }

    Coffin::~Coffin() {
        reset();
    }

    bool Coffin::handleMessage(const Telegram &msg) {
        if (msg.Msg == MessageType::TARGETED) {
            LOG("COFFIN TARGETED!");
            enableTarget();
            return true;
        } else if (msg.Msg == MessageType::NOT_TARGETED) {
            disableTarget();
            return true;
        } else if (msg.Msg == MessageType::PLAYER_INTERACT) {
            Courier->DispatchMsg(SEND_MSG_IMMEDIATELY, SENDER_ID_IRRELEVANT, -1, MessageType::SLEEP);
            return true;
        }
        return false;
    }

    void Coffin::update(const Ogre::FrameEvent &evt) {

    }

    void Coffin::enableTarget() {
        //ActorNode->showBoundingBox(true);

        /*
            rim lighting
            see: http://www.ogre3d.org/tikiwiki/Create+outline+around+a+character

        */

        unsigned short count = ActorEntity->getNumSubEntities();

        const Ogre::String file_name = "rim.dds";
        const Ogre::String rim_material_name = "_rim";

        for (unsigned short i = 0; i < count; ++i) {
            Ogre::SubEntity *subentity = ActorEntity->getSubEntity(i);

            const Ogre::String &old_material_name = subentity->getMaterialName();
            Ogre::String new_material_name = old_material_name + rim_material_name;

            Ogre::MaterialPtr new_material = Ogre::MaterialManager::getSingleton().getByName(new_material_name);

            if (new_material.isNull()) {
                MaterialPtr old_material = Ogre::MaterialManager::getSingleton().getByName(old_material_name);
                new_material = old_material->clone(new_material_name);

                Ogre::Pass *pass = new_material->getTechnique(0)->getPass(0);
                Ogre::TextureUnitState *texture = pass->createTextureUnitState();
                texture->setCubicTextureName(&file_name, true);
                texture->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                texture->setColourOperationEx(Ogre::LBX_ADD, Ogre::LBS_TEXTURE, Ogre::LBS_CURRENT);
                texture->setColourOpMultipassFallback(Ogre::SBF_ONE, Ogre::SBF_ONE);
                texture->setEnvironmentMap(true, Ogre::TextureUnitState::ENV_NORMAL);
            }

            subentity->setMaterial(new_material);
        }
    }

    void Coffin::disableTarget() {
        //ActorNode->showBoundingBox(false);

        /*
            rim lighting
            see: http://www.ogre3d.org/tikiwiki/Create+outline+around+a+character

        */

        unsigned short count = ActorEntity->getNumSubEntities();

        for (unsigned short i = 0; i < count; ++i) {
            Ogre::SubEntity *subentity = ActorEntity->getSubEntity(i);
            Ogre::SubMesh *submesh = subentity->getSubMesh();

            const Ogre::String &old_material_name = submesh->getMaterialName();
            const Ogre::String &new_material_name = subentity->getMaterialName();

            // If the entity is already using original material then done
            if (0 == strcmp(old_material_name.c_str(), new_material_name.c_str()))
                continue;

            subentity->setMaterialName(old_material_name);
        }
    }
}
