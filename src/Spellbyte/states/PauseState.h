/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * PauseState Class
 *
 * This is the pause state class
 *
 */

#ifndef _PAUSESTATE_H_
#define _PAUSESTATE_H_

#include "../stdafx.h"
#include "./BaseMenuState.h"

namespace SpellByte
{
    class PauseState : public BaseMenuState
    {
    public:
        // Buttons
        enum {
            RESUME = 0,
            OPTIONS,
            MAIN_MENU,
            BTN_END
        };
        PauseState();
        DECLARE_BASESTATE_CLASS(PauseState)

        void enter();
        void createScene();
        void exit();
        void resume();

        virtual bool update(const Ogre::FrameEvent &evt);
        virtual void handleEvents();

        void buildGUI();

        bool buttonClicked(const CEGUI::EventArgs &evt);

    private:
        // helper function
        const std::string getButtonID(int id) const;
        // helper function
        const std::string getButtonText(int id) const;

        const float BUTTON_POSITION_START_X;
        const float BUTTON_X_OFFSET;
        const float BUTTON_POSITION_START_Y;
        const float BUTTON_Y_OFFSET;
    };
}

#endif // _PAUSESTATE_H_

