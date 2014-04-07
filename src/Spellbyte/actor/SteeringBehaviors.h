#ifndef STEERING_BEHAVIORS_H
#define STEERING_BEHAVIORS_H

#include "../stdafx.h"
#include "../utilities/utils.h"
#include "../define.h"

namespace SpellByte {
    class Actor;
    class SteeringBehaviors {
    public:
        SteeringBehaviors(Actor *owner);
        ~SteeringBehaviors();

        Ogre::Vector3 seek(Ogre::Vector3 &target);

        Ogre::Vector3 calculate();

        void setTarget(Ogre::Vector3 &target);

    private:
        Ogre::Real MaxSteerForce;
        Actor *Owner;
        Ogre::Vector3 Target;
    };
}

#endif // STEERING_BEHAVIORS_H
