#include <cassert>

#include "Actor.h"

namespace SpellByte {
    int Actor::nextValidID = 0;

    Actor::Actor(int id) {
        setID(id);
    }

    Actor::Actor() {
        setID(nextValidID);
    }

    void Actor::setID(int val) {
        // Ensure ID is valid
        assert((val >= nextValidID) && "<Actor::setID>: invalid ID");

        ID = val;
        nextValidID = ID + 1;
    }

    void Actor::resetNextValidID() {
        nextValidID = 0;
    }

    int Actor::getNextValidID() {
        return nextValidID;
    }

    void Actor::update(const Ogre::FrameEvent &evt) {
        // empty for now
    }

    void Actor::render() {
        // empty for now
    }

    void Actor::handleMessage(const Telegram &msg) {
        // Handle message code for actor here

        // Pass message to each component
        std::map<componentType, ComponentPtr>::iterator it;
        for (it = components.begin(); it != components.end(); ++it) {
            it->second->handleMessage(msg);
        }
    }
}
