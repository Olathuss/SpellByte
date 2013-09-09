/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * MenuState Class
 *
 * This is the menu state class
 *
 */

#ifndef _MENUSTATE_H_
#define _MENUSTATE_H_

#include "../stdafx.h"
#include "./BaseMenuState.h"

namespace SpellByte
{
    class MenuState : public BaseMenuState
    {
    public:
        // Buttons
        enum {
            PLAY_GAME = 0,
            OPTIONS,
            SCORES,
            ACHIEVEMENTS,
            CREDITS,
            EXIT,
            BTN_END
        };
        MenuState();
        DECLARE_BASESTATE_CLASS(MenuState)

        void enter();
        void createScene();
        void exit();
        void resume();

        virtual void update(const Ogre::FrameEvent &evt);
        virtual void handleEvents();

        void buildGUI();

        bool buttonClicked(const CEGUI::EventArgs &evt);

    private:
        // helper function
        virtual const std::string getButtonID(int id) const;
        // helper function
        virtual const std::string getButtonText(int id) const;

        const float BUTTON_POSITION_START_X;
        const float BUTTON_X_OFFSET;
        const float BUTTON_POSITION_START_Y;
        const float BUTTON_Y_OFFSET;
    };
}

#endif // _MENUSTATE_H_
