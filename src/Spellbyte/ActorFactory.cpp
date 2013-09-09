#include "ActorFactory.h"
#include "SpellByte.h"

namespace SpellByte
{
    ActorFactory::ActorFactory(void)
    {
        lastActorID = INVALID_ACTOR_ID;
    }

    ActorPtr ActorFactory::createActor(long slice, long row, long col, tinyxml2::XMLNode* xmlRoot)
    {
        Actor *newActor = new Actor(getNextActorID());
        ActorPtr newActorPtr(newActor);

        for(tinyxml2::XMLNode* element = xmlRoot->FirstChild(); element; element = element->NextSibling())
        {

        }
        return newActorPtr;
    }
}
