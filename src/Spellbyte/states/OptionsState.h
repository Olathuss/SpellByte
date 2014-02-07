/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * OptionsState Class
 *
 * This is the Options state class
 *
 */

#ifndef _OPTIONSSTATE_H_
#define _OPTIONSTATE_H_

#include "../stdafx.h"
#include "./BaseMenuState.h"

namespace SpellByte
{
    class OptionsState : public BaseMenuState
    {
    public:
        // Buttons
        enum {
            APPLY = 0,
            RETURN,
            BTN_END
        };
        OptionsState();
        DECLARE_BASESTATE_CLASS(OptionsState)

        void enter();
        void createScene();
        void exit();
        void resume();

        virtual bool update(const Ogre::FrameEvent &evt);
        void handleEvents();

        void buildGUI();

        bool buttonClicked(const CEGUI::EventArgs &evt);

    private:
        // helper function
        const std::string getButtonID(int id) const;
        // helper function
        const std::string getButtonText(int id) const;

        // Video Resolution Box
        CEGUI::Combobox *videoBox;

        const float BUTTON_POSITION_START_X;
        const float BUTTON_X_OFFSET;
        const float BUTTON_POSITION_START_Y;
        const float BUTTON_Y_OFFSET;
    };
}

#endif // _OPTIONSTATE_H_

