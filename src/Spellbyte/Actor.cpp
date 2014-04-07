#include <cassert>

#include "SLB/SLB.hpp"
#include "Actor.h"
#include "define.h"
#include "World.h"
#include "actor/WanderState.h"

namespace SpellByte {
    int Actor::nextValidINTERNAL_ID = 0;

    Actor::Actor() {
        SceneNode = nullptr;
        AnimationState = nullptr;
        MotionAnimation = nullptr;
        ActorEntity = nullptr;
        ActorFSM = nullptr;
        Steering = nullptr;
        WorldPtr = nullptr;
        setID(nextValidINTERNAL_ID);
        Direction = Ogre::Vector3::ZERO;
        Position = Ogre::Vector3::ZERO;
        positionChanged = false;
        Active = true;
        InMotion = false;
        NeedTravel = false;
        Mass = 1.0f;
        MaxSpeed = APP->getConfigFloat("npc_velocity");
        MaxForce = 0.01;
        ID = 0;
        Velocity = Ogre::Vector3::ZERO;
        realVelocity = APP->getConfigFloat("npc_velocity");

        Ogre::String nodeName = "NODE_MALE" + Ogre::StringConverter::toString(INTERNAL_ID);
        if (!APP->SceneMgr->hasSceneNode(nodeName)) {
            SceneNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
            SceneNode->setScale(.014, .014, .014);
            //SceneNode->setDirection(Ogre::Vector3::UNIT_X);
        }

        WalkList = std::deque<Ogre::Vector3>();
        WalkList.clear();

        ActorFSM = new StateMachine<Actor>(this);
        ActorFSM->setCurrentState(new Wander());//Wander::Instance());
        ActorFSM->setGlobalState(nullptr);

        Steering = new SteeringBehaviors(this);
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
    }

    void Actor::update(const Ogre::FrameEvent &evt) {
        if (!Active)
            return;
        if (positionChanged) {
            SceneNode->setPosition(Position);
            positionChanged = false;
        }
        ActorFSM->update(evt);

        if (!InMotion) {
            if (nextLocation())
                if (MotionAnimation) {
                    MotionAnimation->setWeight(1);
                    MotionAnimation->setLoop(true);
                    MotionAnimation->setEnabled(true);
                    InMotion = true;
                    rotateActor(evt);
                }
        } else {
            Ogre::Real move = realVelocity * evt.timeSinceLastFrame;
            Distance -= move;
            if (Distance <= 0.0f) {
                SceneNode->setPosition(Destination);
                Direction = Ogre::Vector3::ZERO;
                if (InMotion && !nextLocation()) {
                    if (MotionAnimation)
                        MotionAnimation->setLoop(false);
                    InMotion = false;
                } else {
                    rotateActor(evt);
                }
            } else {
                SceneNode->translate(Direction * move);
            }
        }

        if (AnimationState && AnimationState->getEnabled()) {
                if(AnimationState->hasEnded() && !AnimationState->getLoop()) {
                    AnimationState->setEnabled(false);
                }
                AnimationState->addTime(evt.timeSinceLastFrame);
        }
        if (MotionAnimation && MotionAnimation->getEnabled()) {
            if (MotionAnimation->hasEnded() && !MotionAnimation->getLoop()) {
                MotionAnimation->setEnabled(false);
                MotionAnimation->setWeight(0);
                MotionAnimation->setTimePosition(0);
            } else {
                MotionAnimation->addTime(evt.timeSinceLastFrame);
            }
        }
    }

    void Actor::rotateActor(const Ogre::FrameEvent &evt) {
        Ogre::Vector3 src = SceneNode->getOrientation() * Ogre::Vector3::UNIT_Z;
        src.y = 0;
        Direction.y = 0;
        src.normalise();
        Ogre::Quaternion quat = src.getRotationTo(Direction);
        SceneNode->rotate(quat);
    }

    void Actor::queueDestination(Ogre::Vector3 dest) {
        if (!Active || !WorldPtr)
            return;
        WorldPtr->setVector3Height(dest);
        WalkList.push_back(dest);
    }

    bool Actor::nextLocation() {
        if(WalkList.empty()) {
            return false;
            NeedTravel = false;
        }
        Destination = WalkList.front();
        WalkList.pop_front();
        Direction = Destination - SceneNode->getPosition();
        Distance = Direction.normalise();
        NeedTravel = true;

        return true;
    }

    void Actor::setID(int val) {
        // Ensure ID is valid
        assert((val >= nextValidINTERNAL_ID) && "<Actor::setID>: invalid ID");

        INTERNAL_ID = val;
        nextValidINTERNAL_ID = INTERNAL_ID + 1;
    }

    void Actor::activate() {
        Active = true;
        WorldPtr = APP->getWorldPtr();
    }

    void Actor::clear() {
        SceneNode->detachAllObjects();
    }

    void Actor::setAnimation(Ogre::String animName, bool loop, bool enabled) {
        if (ActorEntity->hasAnimationState(animName)) {
            LOG("Starting animation '" + animName + "' for " + ActorEntity->getName());
            if (AnimationState) {
                AnimationState->setWeight(0);
            }
            AnimationState = ActorEntity->getAnimationState(animName);
            AnimationState->setLoop(loop);
            AnimationState->setEnabled(enabled);
        } else {
            LOG("No animation named '" + animName + "' for " + ActorEntity->getName());
        }
    }

    void Actor::reset() {
        Ogre::String entityName = "ENTITY_MALE" + Ogre::StringConverter::toString(INTERNAL_ID);
        if (!APP->SceneMgr->hasEntity(entityName)) {
            ActorEntity = APP->SceneMgr->createEntity(entityName, "MedBaseMaleApril2013.mesh");
            /*LOG("Subentities: " + Ogre::StringConverter::toString(ActorEntity->getNumSubEntities()));
            Ogre::Mesh::SubMeshNameMap::iterator it;
            Ogre::Mesh::SubMeshNameMap nameMap = ActorEntity->getMesh()->getSubMeshNameMap();
            for (it = nameMap.begin(); it != nameMap.end(); ++it) {
                LOG(it->first);
            }*/
            MotionAnimation = ActorEntity->getAnimationState("GENWalk");
            SceneNode->attachObject(ActorEntity);
        }
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

    void Actor::setPosition(const Ogre::Vector3 newPos) {
        Position = newPos;
        positionChanged = true;
    }

    void Actor::setPositionXYZ(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z) {
        Position = Ogre::Vector3(x, y, z);
        positionChanged = true;
    }

    const Ogre::Vector3 Actor::getPosition() const {
        return Position;
    }

    void Actor::resetNextValidID() {
        nextValidINTERNAL_ID = 0;
    }

    int Actor::getNextValidID() {
        return nextValidINTERNAL_ID;
    }

    void Actor::handleMessage(const Telegram &msg) {
        // Handle message code for actor here
    }

    void Actor::bindToLUA() {
        SLB::Class< Actor >("SpellByte::Actor")
            .constructor()
            .set< Actor, void, const Ogre::Real, const Ogre::Real, const Ogre::Real>("setPositionXYZ", &Actor::setPositionXYZ)
            .set< Actor, void, const Ogre::Vector3 >("setPosition", &Actor::setPosition)
            .set< Actor, const Ogre::Vector3 >("getPosition", &Actor::getPosition)
            .set< Actor, void, Ogre::Vector3 >("queueDestination", &Actor::queueDestination)
            .set< Actor, void, Ogre::String, bool, bool >("setAnimation", &Actor::setAnimation)
            .set< Actor, void, Ogre::Real >("setRealVelocity", &Actor::setRealVelocity);
    }
}
