#ifndef ACTORMGR_H
#define ACTORMGR_H

#include <map>
#include <cassert>
#include "stdafx.h"

#define ActorMgr ActorManager::getSingletonPtr()

namespace SpellByte
{
    class Actor;
    class ActorPtr;

    // Ogre Singleton Type
    class ActorManager : public Ogre::Singleton<ActorManager>
    {
    private:
        // Use map to relate actor ID with its pointer
        typedef std::map<int, Actor*> ActorMapType;

    private:
        // Map container with ID and ActorPtr
        ActorMapType ActorMap;
        ActorManager();
        ActorManager(const ActorManager&);
        ActorManager& operator=(const ActorManager&);

    public:
        ~ActorManager();

        // this method stores a pointer to the Actor
        // in the std::vector at the index position indicated
        void registerActor(Actor *newActor);

        // returns pointer to the Actor with the ID given as a parameter
        Actor *getActorFromID(int id) const;

        // this method removes the Actor from the list
        void removeActor(Actor *newActor);

        // clears all entities from the Actor map
        void reset()
        {
            ActorMap.clear();
        }
    };
}

#endif // ACTORMGR_H
