/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * BaseMenuState Class
 *
 * This is base menu state class
 *
 */

#ifndef _BASE_MENUSTATE_H_
#define _BASE_MENUSTATE_H_

#include "../stdafx.h"
#include "../BaseState.h"

namespace SpellByte
{
    class BaseMenuState : public BaseState
    {
    public:
        // ENUM for Buttons within Menu
        /*enum {
            PLAY_GAME = 0,
            OPTIONS,
            SCORES,
            ACHIEVEMENTS,
            CREDITS,
            EXIT,
            BTN_END
        };*/
        BaseMenuState(){};

        virtual void enter(){};
        virtual void createScene(){};
        virtual void exit(){};
        virtual void resume(){};

        virtual void update(double timeSinceLastFrame){};
        virtual void handleEvents(){};

        virtual void buildGUI(){};

        virtual bool buttonClicked(const CEGUI::EventArgs &evt){ return false; };

    protected:
        // helper function
        virtual const std::string getButtonID(int id) const=0;
        // helper function
        virtual const std::string getButtonText(int id) const=0;

    private:
        // Variables to set up button arrangement
        // Each Menu should have its own
        //const float BUTTON_POSITION_START_X = 0.0;
        //const float BUTTON_X_OFFSET = 0.0;
        //const float BUTTON_POSITION_START_Y = 0.0;
        //const float BUTTON_Y_OFFSET = 0.0;
    };
}

#endif // _BASE_MENUSTATE_H_
