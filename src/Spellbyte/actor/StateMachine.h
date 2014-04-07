#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "State.h"

namespace SpellByte {
    template<class actor_type>
    class StateMachine {
    private:
        // Object who owns this class
        actor_type *Parent;

        // Current state of actor
        State<actor_type> *CurrentState;
        // Previous state of actor
        State<actor_type> *PreviousState;
        // Global state, called everytime
        State<actor_type> *GlobalState;

    public:
        StateMachine(actor_type *owner):Parent(owner),
                                    CurrentState(NULL),
                                    PreviousState(NULL),
                                    GlobalState(NULL) {
        }

        // Methods used to initialize the StateMachine
        void setCurrentState(State<actor_type> *s) { CurrentState = s; }
        void setGlobalState(State<actor_type> *s) { GlobalState = s; }
        void setPreviousState(State<actor_type> *s) { PreviousState = s; }

        // Update the StateMachine
        void update(const Ogre::FrameEvent &evt) {
            // Execute global state, if it exists
            if (GlobalState) GlobalState->Execute(Parent, evt);

            // Execute current state, if it exists
            if (CurrentState) CurrentState->Execute(Parent, evt);
        }

        void changeState(State<actor_type> *newState) {
            assert(newState && "<StateMachine::changeState>: trying to change to null state");

            // Set current state as previous state
            PreviousState = CurrentState;

            // Exit from the current state
            CurrentState->Exit(Parent);

            // Set current state to newState
            CurrentState = newState;

            // Enter into current state
            CurrentState->Enter(Parent);
        }

        // change state back to the previous one
        void revertToPreviousState() {
            ChangeSate(PreviousState);
        }

        // Accessors
        State<actor_type> *getCurrentState() const { return CurrentState ;}
        State<actor_type> *getGlobalState() const { return GlobalState; }
        State<actor_type> *getPreviousState() const { return PreviousState; }

        //bool isInState(const State<actor_type> &st) const { return CurrentState == st; }
    };
}

#endif // STATE_MACHINE_H
