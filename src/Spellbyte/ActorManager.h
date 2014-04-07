#ifndef ACTORMGR_H
#define ACTORMGR_H

#include <map>
#include <cassert>
#include <vector>
#include <utility>
#include "stdafx.h"

#define ActorMgr ActorManager::getInstance()

namespace SpellByte {
    class Actor;
    // Ogre Singleton Type
    class ActorManager {
    typedef std::vector<Ogre::String> ActorSet;
    typedef std::vector<std::pair<Ogre::String, Ogre::String> > SetPair;
    private:
        // Use vector for contiguous iteration
        std::vector<Actor> ActorVector;

        // Data for Actor Sets
        ActorSet Required;
        std::map<Ogre::String, ActorSet> RequiredSubsets;
        ActorSet SetNames;
        std::map<Ogre::String, SetPair> ActorSets;

        // Singleton class, keep these private
        ActorManager();
        ActorManager(const ActorManager&);
        ActorManager& operator=(const ActorManager&);

        // Bind to LUA
        static bool boundToLUA;
        static void bindToLUA();

        static ActorManager *Instance;

        static unsigned int ActorCount;

    public:
        static ActorManager *getInstance();
        virtual ~ActorManager();

        // Load different skin sets for actors
        void loadActorSets();
        // Return random skin set
        ActorSet getRandomSkin();
        // Get particular skin set, not yet implemented.
        void getSkin(Ogre::String skinName){};

        // Update actors
        void update(const Ogre::FrameEvent &evt);

        // Method used to allocate new actor
        // and return it.
        Actor *getFreeActor();

        // Get actor count
        int getActorCount() {
            return ActorCount;
        }

        // this method stores a pointer to the Actor
        // in the std::vector at the index position indicated
        void registerActor(Actor *newActor);

        // returns pointer to the Actor with the ID given as a parameter
        Actor *getActorFromID(int id);

        // this method removes an actor, it will reuse the
        // actor when getFreeActor is called for object recycling
        void removeActor(Actor *removeActor);

        // Clears all actors in vector
        void clearActors();

        // Resets all actors in vector, i.e. reload entity
        void reset();
    };
}

#endif // ACTORMGR_H
