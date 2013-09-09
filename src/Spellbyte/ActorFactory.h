#ifndef _ACTOR_FACTORY_H_
#define _ACTOR_FACTORY_H_

#include "Actor.h"
#include "utilities/tinyxml2.h"

namespace SpellByte
{
    class ActorFactory
    {
        ActorID lastActorID;

    protected:

    public:
        ActorFactory(void);

        ActorPtr createActor(long slice, long row, long col, tinyxml2::XMLNode *xmlRoot);

    protected:
        //virtual ComponentPtr createComponent(tinyxml2::XMLNode *xmlElement);

    private:
        ActorID getNextActorID(void) { ++lastActorID; return lastActorID; };
    };
}

#endif // _ACTOR_FACTORY_H_
