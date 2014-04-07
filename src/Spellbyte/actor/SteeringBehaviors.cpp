#include "SteeringBehaviors.h"
#include "../Actor.h"

namespace SpellByte {
    SteeringBehaviors::SteeringBehaviors(Actor *owner) {
        Owner = owner;
        Target = Ogre::Vector3::ZERO;
        MaxSteerForce = Owner->maxForce();
    }

    SteeringBehaviors::~SteeringBehaviors() {
        Owner = nullptr;
    }

    Ogre::Vector3 SteeringBehaviors::seek(Ogre::Vector3 &target) {
        Ogre::Vector3 DesiredVelocity = Ogre::Vector3(((target - Owner->getPosition()).normalise()));

        return (DesiredVelocity - Owner->velocity());
    }

    Ogre::Vector3 SteeringBehaviors::calculate() {
        Ogre::Vector3 SteeringForce;

        if (Target != Ogre::Vector3::ZERO)
            SteeringForce += seek(Target);

        truncate(SteeringForce, MaxSteerForce);
        return SteeringForce;
    }

    void SteeringBehaviors::setTarget(Ogre::Vector3 &target) {
        Target = target;
    }
}
