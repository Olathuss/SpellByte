#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "../BaseState.h"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

namespace SpellByte
{
    enum QueryFlags
    {
        OGRE_HEAD_MASK      = 1<<0,
        CUBE_MASK           = 1<<1
    };

    class GameState : public BaseState
    {
    public:
        GameState();

        DECLARE_BASESTATE_CLASS(GameState)

        void enter();
        void createScene();
        void exit();
        bool pause();
        void resume();

        void moveCamera();
        void buildGUI(){};

        void update(const Ogre::FrameEvent &evt);
        void handleEvents();

    private:
        bool                    LMouseDown;
        bool                    RMouseDown;
        bool                    SettingsMode;
    };
}

#endif // _GAMESTATE_H_
