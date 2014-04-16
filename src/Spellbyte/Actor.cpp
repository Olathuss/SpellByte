#include <cassert>

#include <SLB/SLB.hpp>
#include "Actor.h"
#include <define.h>
#include <World.h>
#include <actor/WanderState.h>
#include <audio/AudioManager.h>
#include <utilities/Messages.h>
#include <utilities/MessageDispatcher.h>
#include <utilities/telegram.h>
#include <graph/AStarSearch.h>

namespace SpellByte {
    int BaseActor::nextValidINTERNAL_ID = 0;

    bool Actor::initAnimationList = false;
    std::vector<Ogre::String> Actor::idleAnimations = std::vector<Ogre::String>(24);

    Actor::Actor():BaseActor() {
        AnimationState = nullptr;
        MotionAnimation = nullptr;
        ActorEntity = nullptr;
        ActorFSM = nullptr;
        Steering = nullptr;
        ActorAny = nullptr;
        Direction = Ogre::Vector3::ZERO;
        Position = Ogre::Vector3::ZERO;
        positionChanged = false;
        Active = true;
        InMotion = false;
        NeedTravel = false;
        dead = false;
        Mass = 1.0f;
        MaxSpeed = APP->getConfigFloat("npc_velocity");
        MaxForce = 0.01;
        ID = 0;
        Velocity = Ogre::Vector3::ZERO;
        realVelocity = APP->getConfigFloat("npc_velocity");

        Ogre::String nodeName = "NODE_MALE" + Ogre::StringConverter::toString(getInternalID());
        if (!APP->SceneMgr->hasSceneNode(nodeName)) {
            ActorNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
            Ogre::Real scaleFactor = (rand() % 60) / 10000 + 0.01;
            ActorNode->setScale(scaleFactor, scaleFactor, scaleFactor);
            //ActorNode->setDirection(Ogre::Vector3::UNIT_X);
            ActorAny = new UserAny(UserAny::ACTOR, ID);
            ActorNode->setUserAny(Ogre::Any(ActorAny));
        }

        WalkList = std::deque<Ogre::Vector3>();
        WalkList.clear();

        ActorFSM = new StateMachine<Actor>(this);
        ActorFSM->setCurrentState(new Wander());//Wander::Instance());
        ActorFSM->setGlobalState(nullptr);

        Steering = new SteeringBehaviors(this);

        if (!initAnimationList) {
            idleAnimations.push_back("NPCThinking");
            idleAnimations.push_back("NPCLookingAround");
            idleAnimations.push_back("NPCBoring");
            idleAnimations.push_back("NPCYawn");
            idleAnimations.push_back("NPCPointingA");
            initAnimationList = true;
        }
    }

    Actor::~Actor() {
        if (ActorFSM) {
            delete ActorFSM;
            ActorFSM = nullptr;
        }

        if (Steering) {
            delete Steering;
            Steering = nullptr;
        }

        if (ActorAny) {
            delete ActorAny;
            ActorAny = nullptr;
        }
    }

    void Actor::update(const Ogre::FrameEvent &evt) {
        if (!Active)
            return;

        if (AnimationState) {
                if (dead && AnimationState->getAnimationName() == APP->getConfigString("npc_death") &&
                    !AnimationState->hasEnded())
                    AnimationState->addTime(evt.timeSinceLastFrame);
                else if(!InMotion && (!AnimationState->getEnabled() || AnimationState->hasEnded()))
                    animateIdle();
                else
                    AnimationState->addTime(evt.timeSinceLastFrame);
        }

        if (dead)
            return;

        if (positionChanged) {
            ActorNode->setPosition(Position);
            positionChanged = false;
        }
        ActorFSM->update(evt);

        if (!InMotion) {
            if (nextLocation()) {
                if (MotionAnimation) {
                    MotionAnimation->setLoop(true);
                    MotionAnimation->setEnabled(true);
                    InMotion = true;
                    rotateActor(evt);
                }
            }
        } else {
            Ogre::Real move = realVelocity * evt.timeSinceLastFrame;
            Distance -= move;
            if (Distance <= 0.0f) {
                ActorNode->setPosition(Destination);
                Position = ActorNode->getPosition();
                Direction = Ogre::Vector3::ZERO;
                if (InMotion && !nextLocation()) {
                    if (MotionAnimation)
                        MotionAnimation->setLoop(false);
                    InMotion = false;
                } else {
                    rotateActor(evt);
                }
            } else {
                ActorNode->translate(Direction * move);
                Position = ActorNode->getPosition();
            }
        }

        if (MotionAnimation && MotionAnimation->getEnabled()) {
            if (MotionAnimation->hasEnded() && !MotionAnimation->getLoop()) {
                MotionAnimation->setEnabled(false);
                //MotionAnimation->setWeight(0);
                MotionAnimation->setTimePosition(0);
                InMotion = false;
            } else {
                MotionAnimation->addTime(evt.timeSinceLastFrame);
            }
        }
    }

    void Actor::animateIdle() {
        int randomIdleAnimation = rand() % idleAnimations.size();
        setAnimation(idleAnimations[randomIdleAnimation], false, true);
    }

    void Actor::die() {
        if (dead)
            return;
        MotionAnimation->setEnabled(false);
        MotionAnimation->setTimePosition(0);
        InMotion = false;
        WalkList.clear();
        setAnimation(APP->getConfigString("npc_death"), false, true);
#ifdef AUDIO
        AUDIOMAN->playWAV("arghhh.wav");
#endif
        dead = true;
    }

    void Actor::rotateActor(const Ogre::FrameEvent &evt) {
        Ogre::Vector3 src = ActorNode->getOrientation() * Ogre::Vector3::UNIT_Z;
        src.y = 0;
        Direction.y = 0;
        src.normalise();
        Ogre::Quaternion quat = src.getRotationTo(Direction);
        ActorNode->rotate(quat);
    }

    void Actor::queueDestination(Ogre::Vector3 dest) {
        if (!Active || dead || !WorldPtr)
            return;
        WorldPtr->setVector3Height(dest);
        WalkList.push_back(dest);
    }

    bool Actor::nextLocation() {
        if(dead || WalkList.empty()) {
            NeedTravel = false;
            if (!InMotion && rand() % 100 == 1)
                Courier->DispatchMsg(rand() % 5000, SENDER_ID_IRRELEVANT, ID, MessageType::RANDOM_TRAVEL);
            return false;
        }
        Destination = WalkList.front();
        WalkList.pop_front();
        Direction = Destination - ActorNode->getPosition();
        Distance = Direction.normalise();
        NeedTravel = true;

        return true;
    }

    void Actor::activate() {
        Active = true;
        WorldPtr = APP->getWorldPtr();
        currentNode = rand() % WorldPtr->getGraph()->nodeCount();
        setPosition(WorldPtr->getGraph()->getNode(currentNode).getPos());
        ActorAny->ID = ID;
        dead = false;
    }

    int Actor::getID() {
        return ID;
    }

    void Actor::enableTarget() {
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

    void Actor::disableTarget() {
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

    void Actor::clear() {
        ActorNode->detachAllObjects();
    }

    void Actor::setAnimation(Ogre::String animName, bool loop, bool enabled) {
        if (dead)
            return;
        if (ActorEntity->hasAnimationState(animName)) {
            if (AnimationState) {
                //AnimationState->setWeight(0);
                AnimationState->setTimePosition(0);
                AnimationState->setEnabled(false);
            }
            AnimationState = ActorEntity->getAnimationState(animName);
            AnimationState->setLoop(loop);
            AnimationState->setEnabled(enabled);
        } else {
            LOG("No animation named '" + animName + "' for " + ActorEntity->getName());
        }
    }

    void Actor::reset() {
        Ogre::String entityName = "ENTITY_MALE" + Ogre::StringConverter::toString(getInternalID());
        if (!APP->SceneMgr->hasEntity(entityName)) {
            ActorEntity = APP->SceneMgr->createEntity(entityName, "MedBaseMaleApril2013.mesh");
            ActorEntity->setQueryFlags(World::COLLISION_MASK::ACTOR);
            /*LOG("Subentities: " + Ogre::StringConverter::toString(ActorEntity->getNumSubEntities()));
            Ogre::Mesh::SubMeshNameMap::iterator it;
            Ogre::Mesh::SubMeshNameMap nameMap = ActorEntity->getMesh()->getSubMeshNameMap();
            for (it = nameMap.begin(); it != nameMap.end(); ++it) {
                LOG(it->first);
            }*/
            MotionAnimation = ActorEntity->getAnimationState("GENWalk");
            MotionAnimation->setEnabled(false);
            MotionAnimation->setTimePosition(0);
            AnimationState = ActorEntity->getAnimationState("NPCThinking");
            AnimationState->setTimePosition(0);
            AnimationState->setEnabled(false);
            ActorNode->attachObject(ActorEntity);
        }
        AnimationState = nullptr;
        WalkList.clear();
        InMotion = NeedTravel = false;
        deactivate();
    }

    void Actor::deactivate() {
        Active = false;
        for (unsigned int i = 0; i < ActorEntity->getNumSubEntities(); ++i) {
            Ogre::SubEntity *sEnt;
            sEnt = ActorEntity->getSubEntity(i);
            sEnt->setVisible(false);
        }
    }

    bool Actor::handleMessage(const Telegram &msg) {
        LOG("Actor(" + Ogre::StringConverter::toString(ID) + ") Received Message: " + msgToString(msg.Msg));
        if (dead || !Active)
            return false;
        if (msg.Msg == MessageType::PLAYER_INTERACT) {
            Ogre::Vector3 attackerPos = DereferenceToType<Ogre::Vector3>(msg.ExtraInfo);
            if (attackerPos.distance(ActorNode->getPosition()) < 5) {
                LOG("Player fed succesfully!");
                die();
                Courier->DispatchMsg(SEND_MSG_IMMEDIATELY, ID, msg.Sender, MessageType::FEED_SUCCESSFUL);
            }
            return true;
        } else if(msg.Msg == MessageType::TARGETED) {
            enableTarget();
        } else if(msg.Msg == MessageType::NOT_TARGETED) {
            disableTarget();
        } else if (msg.Msg == MessageType::RANDOM_TRAVEL && !InMotion) {
            if (rand() % 100 < 25) {
                travelRandom();
            } else if (rand() % 100 < 10) {
                Courier->DispatchMsg(rand() % 10000 + 5000, SENDER_ID_IRRELEVANT, ID, MessageType::RANDOM_TRAVEL);
            }
            return true;
        }
        return false;
    }

    void Actor::travelFromTo(int fromNode, int toNode) {
        Graph::AStarSearch AStar(fromNode, toNode, WorldPtr->getGraph());
        if (AStar.search()) {
            std::vector<int> path = AStar.getPath();
            for (unsigned int i = 0; i < path.size(); ++i) {
                Ogre::Vector3 nextPos = WorldPtr->getGraph()->getNode(path[i]).getPos();
                queueDestination(nextPos);
                currentNode = path[path.size() - 1];
            }
        }
    }

    void Actor::travelRandom() {
        int nodeCount = WorldPtr->getGraph()->nodeCount();
        LOG("Traveling to random location");
        while(true) {
            int nextLocation = rand() % nodeCount;
            if (nextLocation != currentNode) {
                travelFromTo(currentNode, nextLocation);
                return;
            }
        }
    }

    void Actor::bindToLUA() {
        SLB::Class< Actor >("SpellByte::Actor")
            .constructor()
            .set< Actor, void, const Ogre::Real, const Ogre::Real, const Ogre::Real>("setPositionXYZ", &Actor::setPositionXYZ)
            .set< Actor, void, const Ogre::Vector3 >("setPosition", &Actor::setPosition)
            .set< Actor, const Ogre::Vector3 >("getPosition", &Actor::getPosition)
            .set< Actor, void, Ogre::Vector3 >("queueDestination", &Actor::queueDestination)
            .set< Actor, void, Ogre::String, bool, bool >("setAnimation", &Actor::setAnimation)
            .set< Actor, void, Ogre::Real >("setRealVelocity", &Actor::setRealVelocity)
            .set< Actor, void, int, int >("travelFromTo", &Actor::travelFromTo);
    }
}
