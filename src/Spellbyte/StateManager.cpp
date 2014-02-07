#include <OgreWindowEventUtilities.h>

#include "StateManager.h"

namespace SpellByte
{
    StateManager::StateManager()
    {
        Shutdown = false;
    }

    StateManager::~StateManager()
    {
        state_info si;

        while(!ActiveStateStack.empty())
        {
            ActiveStateStack.back()->exit();
            ActiveStateStack.pop_back();
        }

        while(!States.empty())
        {
            si = States.back();
            si.state->destroy();
            States.pop_back();
        }
    }

    bool StateManager::update(const Ogre::FrameEvent &evt)
    {
        if(!ActiveStateStack.empty())
            return ActiveStateStack.back()->update(evt);
    }

    void StateManager::handleEvents()
    {
        if(!ActiveStateStack.empty())
            ActiveStateStack.back()->handleEvents();
    }

    void StateManager::manageState(Ogre::String stateName, BaseState* state)
    {
        try
        {
            state_info new_state_info;
            new_state_info.name = stateName;
            new_state_info.state = state;
            States.push_back(new_state_info);
        }
        catch(std::exception &e)
        {
            delete state;
            throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR,
                                  "Error while trying to manage a new BaseState\n" + Ogre::String(e.what()), "StateManager.cpp (43)");
        }
    }

    BaseState* StateManager::findByName(Ogre::String stateName)
    {
        std::vector<state_info>::iterator it;

        for(it = States.begin(); it != States.end(); it++)
        {
            if(it->name == stateName)
                return it->state;
        }

        return 0;
    }

    void StateManager::changeState(BaseState *state)
    {
        if(!ActiveStateStack.empty())
        {
            ActiveStateStack.back()->exit();
            ActiveStateStack.pop_back();
        }

        ActiveStateStack.push_back(state);
        init(state);
        ActiveStateStack.back()->enter();
    }

    bool StateManager::pushState(BaseState *state)
    {
        if(!ActiveStateStack.empty())
        {
            if(!ActiveStateStack.back()->pause())
                return false;
        }

        ActiveStateStack.push_back(state);
        init(state);
        ActiveStateStack.back()->enter();

        return true;
    }

    void StateManager::popState()
    {
        if(!ActiveStateStack.empty())
        {
            ActiveStateStack.back()->exit();
            ActiveStateStack.pop_back();
        }

        if(!ActiveStateStack.empty())
        {
            init(ActiveStateStack.back());
            ActiveStateStack.back()->resume();
        }
        else
        {
            shutDown();
        }
    }

    void StateManager::popAllAndPushAppState(BaseState *state)
    {
        while(!ActiveStateStack.empty())
        {
            ActiveStateStack.back()->exit();
            ActiveStateStack.pop_back();
        }

        pushState(state);
    }

    void StateManager::pauseState()
    {
        if(!ActiveStateStack.empty())
        {
            ActiveStateStack.back()->pause();
        }

        if(ActiveStateStack.size() > 2)
        {
            init(ActiveStateStack.at(ActiveStateStack.size() - 2));
            ActiveStateStack.at(ActiveStateStack.size() - 2)->resume();
        }
    }

    void StateManager::shutDown()
    {
        Shutdown = true;
    }

    void StateManager::init(BaseState *state)
    {
        // Empty for now
    }
}
