#ifndef INTERACTABLE_OBJECT_H
#define INTERACTABLE_OBJECT_H

#include "BaseActor.h"

/*
    InteractableObject - This inherits from
    BaseActor, which is a temporary hack.
    BaseActor needs to inherit from BaseObject,
    and Object also needs to inherit from BaseObject.
    InteractableObject needs to inherit from BaseObject
    or Object
*/

namespace SpellByte {
    class Interactable : public BaseActor {
    public:
        Interactable():BaseActor(){};
        virtual ~Interactable(){};

        virtual bool handleMessage(const Telegram &msg){return false;}

        virtual void update(const Ogre::FrameEvent &evt){}
    };

    class Coffin : public Interactable {
    public:
        Coffin();
        virtual ~Coffin();

        void init(World *worldptr);
        void reset();

        void enableTarget();
        void disableTarget();

        virtual bool handleMessage(const Telegram &msg);

        virtual void update(const Ogre::FrameEvent &evt);
    };
}

#endif // INTERACTABLE_OBJECT_H
