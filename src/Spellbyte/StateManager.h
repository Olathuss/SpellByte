/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * StateManager Class
 *
 * This class handles the different game states
 * and the transition from each.
 *
 */

#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

#include <vector>
#include "BaseState.h"

namespace SpellByte
{
    class StateManager : public StateListener
    {
    public:
        typedef struct
        {
            Ogre::String name;
            BaseState* state;
        } state_info;

        StateManager();
        // StateManager constructor
        ~StateManager();
        // StateManager destructor

        void manageState(Ogre::String stateName, BaseState* state);
        // Function to later add a new state to the manager

        virtual BaseState* findByName(Ogre::String stateName);

        virtual void update(const Ogre::FrameEvent &evt);
        virtual void handleEvents();

        void changeState(BaseState *state);
        // Exits the current app state and starts the one specified in the parameter
        bool pushState(BaseState *state);
        // Puts a new app state on the active state stack that will then be excecuted
        void popState();
        // Removes the top active state from the stack, which results in returning to the one below
        void pauseState();
        // Pauses current state and resuming the one below
        void shutDown();
        void popAllAndPushAppState(BaseState *state);
        // Removes all current app states from the stack and moves to the given new state

        bool hasShutdown()
        {
            return Shutdown;
        }


    protected:
        void init(BaseState *state);

        std::vector<BaseState*> ActiveStateStack;
        std::vector<state_info> States;
        bool    Shutdown;
    };
}

#endif // _STATEMANAGER_H_
