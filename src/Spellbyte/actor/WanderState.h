#ifndef WANDER_STATE_H
#define WANDER_STATE_H

#include "State.h"
#include "../define.h"

namespace SpellByte {
    class Wander : public State<Actor> {
    public:
        Wander(){};
        virtual ~Wander(){};

        virtual void Enter(Actor* actor) {
            LOG("Entering Wander State");
        }

        virtual void Execute(Actor* actor, const Ogre::FrameEvent &evt) {

        }

        virtual void Exit(Actor* actor) {
            LOG("Exit Wander State");
        }
    };
}

#endif // WANDER_STATE_H
