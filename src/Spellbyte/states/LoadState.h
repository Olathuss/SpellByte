/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * LoadState Class
 *
 * This is load state class
 *
 */

#ifndef _LOADSTATE_H_
#define _LOADSTATE_H_

#include "../BaseState.h"

namespace SpellByte
{
    class LoadState : public BaseState
    {
    public:
        LoadState();
        DECLARE_BASESTATE_CLASS(LoadState)

        void enter();
        void createScene();
        void exit();
        void resume();

        virtual void update(const Ogre::FrameEvent &evt);
        virtual void handleEvents();

    private:
        bool Quit;
    };
}

#endif // _LOADSTATE_H_


