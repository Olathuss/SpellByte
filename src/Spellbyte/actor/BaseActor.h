#ifndef BASE_ACTOR_H
#define BASE_ACTOR_H

#include "../UserAny.h"

/*
    This is an abstract BaseActor class
    which should not be created
*/

namespace SpellByte {
    // Define unique ID for component type
    typedef int ActorID;

    // Telegram for actor communication
    struct Telegram;

    class BaseActor {
    public:
        BaseActor() {
            setID();
        }
        virtual ~BaseActor() {
            WorldPtr = nullptr;
        };

        setWorldPtr(World *world) {
            WorldPtr = world;
        }

        // entities handle messages, pass telegram to each component
        virtual bool handleMessage(const Telegram &msg) = 0;

        // Some basic stuff
        void setPosition(const Ogre::Vector3 newPos) {
            ActorNode->setPosition(newPos);
        }

        void setPositionXYZ(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z) {
            ActorNode->setPosition(x, y, z);
        }

        const Ogre::Vector3 getPosition() const {
            return ActorNode->getPosition();
        }

        virtual void update(const Ogre::FrameEvent &evt) = 0;

    protected:
        Ogre::SceneNode     *ActorNode;
        Ogre::Entity        *ActorEntity;
        World               *WorldPtr;
        UserAny             *ActorAny;

        // Sets ID of the Actor
        void setID() {
            INTERNAL_ID = nextValidINTERNAL_ID++;
        }

        // next valid ID, each type an Actor is created
        // this is updated
        static int nextValidINTERNAL_ID;

        int getInternalID() {
            return INTERNAL_ID;
        }

    private:
        // Each Actor uses INTERNAL_ID for its Scene/Entity
        ActorID INTERNAL_ID;

        // grab next valid id
        static int getNextValidID() {
            return nextValidINTERNAL_ID;
        }

        // reset entities next id
        static void resetNextValidID() {
            nextValidINTERNAL_ID = 0;
        }
    };
}

#endif // BASE_ACTOR_H
