#ifndef ACTOR_H
#define ACTOR_H

#include <map>
#include <deque>
#include "stdafx.h"
#include "Object.h"
#include "actor/BaseActor.h"
#include "actor/StateMachine.h"
#include "actor/SteeringBehaviors.h"

namespace SpellByte {
    // Telegram for Actor communication
    struct Telegram;
    class ActorPtr;
    class World;

    class Actor : public BaseActor {

    // Allow ActorFactory to access any data of Actor
    friend class ActorFactory;
    friend class ActorManager;
    friend class State<Actor>;
    friend class Wander;

    public:
        static void bindToLUA();

    private:
        // Each Actor uses an ID as an external ID for GameLogic reference
        ActorID ID;

        // Mark Actor has active or inactive
        bool active;
        bool dead;

        bool Active;

        void rotateActor(const Ogre::FrameEvent &evt);

    protected:
        StateMachine<Actor> *ActorFSM;

        Ogre::AnimationState *AnimationState;
        // Animation used while in motion (ralk/run)
        Ogre::AnimationState *MotionAnimation;
        Ogre::Vector3 Position;
        Ogre::Vector3 Velocity;
        Ogre::Real realVelocity;
        Ogre::Vector3 Direction;
        Ogre::Vector3 Heading;
        Ogre::Vector3 Side;
        Ogre::Vector3 Destination;
        Ogre::Real Distance;

        bool InMotion;
        bool NeedTravel;

        Ogre::Real Mass;
        Ogre::Real MaxSpeed;
        Ogre::Real MaxForce;
        Ogre::Real MaxTurnRate;
        bool positionChanged;

        SteeringBehaviors *Steering;

        std::deque<Ogre::Vector3> WalkList;

    public:
        Actor();

        ~Actor();

        // Update components
        virtual void update(const Ogre::FrameEvent &evt);

        // entities handle messages, pass telegram to each component
        virtual bool handleMessage(const Telegram &msg);

        void setAnimation(Ogre::String animName, bool loop, bool enabled);

        // Check if this actor is active or not
        bool isActive() { return Active; }
        void reset();
        void clear();
        void deactivate();
        void activate();

        // For targeting
        void enableTarget();
        void disableTarget();
        void die();

        void queueDestination(Ogre::Vector3 dest);
        bool nextLocation();

        Ogre::Vector3 velocity()const {
            return Velocity;
        }

        void setRealVelocity(Ogre::Real velocity) {
            realVelocity = velocity;
        }

        Ogre::Real getRealVelocity() {
            return realVelocity;
        }

        void setVelocity(const Ogre::Vector3 &newVel) {
            Velocity = newVel;
        }

        Ogre::Real mass() const {
            return Mass;
        }

        Ogre::Vector3 side() {
            return Side;
        }

        Ogre::Real maxSpeed() const {
            return MaxSpeed;
        }

        void setMaxSpeed(Ogre::Real newSpeed) {
            MaxSpeed = newSpeed;
        }

        Ogre::Real maxForce() {
            return MaxForce;
        }

        void setMaxForce(Ogre::Real newForce) {
            MaxForce = newForce;
        }

        bool isSpeedMaxedOut() const {
            return MaxSpeed * MaxSpeed >= Velocity.squaredLength();
        }

        Ogre::Real speed() const {
            return Velocity.length();
        }

        Ogre::Real speedSq() const {
            return Velocity.squaredLength();
        }

        Ogre::Vector3 direction() {
            return Direction;
        }

        void setDirection(Ogre::Vector3 newDirection) {
            Direction = newDirection;
        }

        //bool rotateHeadingToFacePosition(Ogre::Vector3 target){};

        Ogre::Real maxTurnRate() const {
            return MaxTurnRate;
        }
        void setMaxTurnRate(Ogre::Real val) {
            MaxTurnRate = val;
        }

        int getID();
    };

    // Use smart pointer for entities
    // Which allows for automatic garbage collection
    class ActorPtr : public Ogre::SharedPtr<Actor>
    {
    public:
        ActorPtr() : Ogre::SharedPtr<Actor>() {}
        explicit ActorPtr(Actor *rep) : Ogre::SharedPtr<Actor>(rep) {}
        ActorPtr(const ActorPtr &r) : Ogre::SharedPtr<Actor>(r) {}
    };
}

#endif // ACTOR_H
