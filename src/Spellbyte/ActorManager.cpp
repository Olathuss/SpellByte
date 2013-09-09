#include "ActorManager.h"
#include "Actor.h"

namespace SpellByte
{
    template<> ActorManager* Ogre::Singleton<ActorManager>::msSingleton = 0;

    ActorManager::~ActorManager()
    {
        this->reset();
    }

    ActorPtr ActorManager::getActorFromID(int id) const
    {
        // find the Actor
        ActorMapType::const_iterator ent = ActorMap.find(id);

        // assert that the Actor is a member of the map
        assert((ent != ActorMap.end()) && "<ActorManager::getActorFromID>: invalid ID");

        return ent->second;
    }

    void ActorManager::removeActor(ActorPtr removeActor)
    {
        ActorMap.erase(ActorMap.find(removeActor->getID()));
    }

    void ActorManager::registerActor(ActorPtr newActor)
    {
        ActorMap.insert(std::make_pair(newActor->getID(), newActor));
    }
}
