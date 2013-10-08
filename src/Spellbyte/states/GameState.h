#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "../BaseState.h"

#include <OgreSubEntity.h>
#include "../Player.h"
#include "../CollisionTools.h"
#include "../World.h"

namespace SpellByte
{
    class GameState : public BaseState
    {
    public:
        GameState();
        ~GameState();

        DECLARE_BASESTATE_CLASS(GameState)

        void enter();
        void createScene();
        void destroyScene();
        void exit();
        bool pause();
        void resume();

        void buildGUI(){};

        void update(const Ogre::FrameEvent &evt);
        void handleEvents();

        MOC::CollisionTools* mCollisionTools;

    private:
        bool useArtifex;

        // GUI
        CEGUI::Window *debugBox;

        // Player
        Player player;

        // World
        World gameWorld;
    };
}

#endif // _GAMESTATE_H_
