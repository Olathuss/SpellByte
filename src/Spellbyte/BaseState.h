/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * BaseState Class
 *
 * This is the parent class for different game states.
 *
 */

#ifndef _BASESTATE_H_
#define _BASESTATE_H_

#include "stdafx.h"

namespace SpellByte
{
    class BaseState;

    class StateListener
    {
    public:
        StateListener(){};
        virtual ~StateListener(){};

        virtual void manageState(Ogre::String stateName, BaseState *state) = 0;

        virtual BaseState*      findByName(Ogre::String stateName) = 0;
        virtual void            changeState(BaseState *state) = 0;
        virtual bool            pushState(BaseState *state) = 0;
        virtual void            popState() = 0;
        virtual void            pauseState() = 0;
        virtual void            shutDown() = 0;
        virtual void            popAllAndPushAppState(BaseState *state) = 0;
    };

    class BaseState
    {
    public:
        virtual ~BaseState(){};

        static void create(StateListener* parent, const Ogre::String name){};

        void destroy() { delete this; }

        // This method executes actions when game enters this state
        virtual void enter() = 0;
        // This method executes actions when game exits this state
        virtual void exit() = 0;
        // This method executes actions when this game state is paused
        virtual bool pause() {return true;};
        // This method executes when this state is resumed from pause
        virtual void resume() = 0;

        // Method executed to handle events
        virtual void handleEvents() = 0;

        // This method is run during the main loop to update various game information
        virtual void update(const Ogre::FrameEvent &evt) = 0;

    protected:
        BaseState() {};

        // Find state by Ogre::String name
        BaseState* findByName(Ogre::String stateName)
        {
            return Parent->findByName(stateName);
        };

        // Changes current game state
        void changeState(BaseState *newState)
        {
            Parent->changeState(newState);
        };

        // Pushes new game state onto the stack
        bool pushState(BaseState *newState)
        {
            return Parent->pushState(newState);
        };

        // Pops game state off the stack
        void popState()
        {
            Parent->popState();
        };

        // Pops all states and push new state onto stack
        void popAllAndPushState(BaseState *newState)
        {
            Parent->popAllAndPushAppState(newState);
        };

        StateListener*      Parent;

        Ogre::Camera*       Camera;
        Ogre::SceneManager* SceneMgr;
        Ogre::FrameEvent    FrameEvent;

        bool Quit;
    };

    #define DECLARE_BASESTATE_CLASS(T)                                  \
    static void create(StateListener* parent, const Ogre::String name)  \
    {                                                                   \
        T* myBaseState = new T();                                       \
        myBaseState->Parent = parent;                                     \
        parent->manageState(name, myBaseState);                         \
    }
}

#endif // _BASESTATE_H_

