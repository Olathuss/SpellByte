/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * PlayMenuState Class
 *
 * This is the PlayMenu state class
 *
 */

#ifndef _PLAY_MENUSTATE_H_
#define _PLAY_MENU_STATE_H_

#include "../stdafx.h"
#include "./BaseMenuState.h"

namespace SpellByte
{
    class PlayMenuState : public BaseMenuState
    {
    public:
        // Buttons
        enum {
            STORY_MODE = 0,
            CHALLENGE_MODE,
            CUSTOM_MODE,
            RETURN_MAIN,
            BTN_END
        };
        PlayMenuState();
        DECLARE_BASESTATE_CLASS(PlayMenuState)

        void enter();
        void createScene();
        void exit();
        void resume();

        void update(const Ogre::FrameEvent &evt);
        void handleEvents();

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

#endif // _PLAY_MENU_STATE_H_


