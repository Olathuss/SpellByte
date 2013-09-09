/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * SplashState Class
 *
 * This is splash state class
 *
 */

#ifndef _SPLASHSTATE_H_
#define _SPLASHSTATE_H_

#include <vector>
#include <string>
#include "../BaseState.h"

namespace SpellByte
{
    class SplashState : public BaseState
    {
    public:
        SplashState();
        ~SplashState();
        DECLARE_BASESTATE_CLASS(SplashState)

        void enter();
        void createScene();
        void exit();
        void resume();

        void update(const Ogre::FrameEvent &evt);
        void handleEvents();

    private:
        std::vector<std::string> SplashScreenList;
        double SplashStart;
        Ogre::Rectangle2D * SplashScreen;
        unsigned int CurrentSplash;
    };
}

#endif // _SPLASHSTATE_H_

