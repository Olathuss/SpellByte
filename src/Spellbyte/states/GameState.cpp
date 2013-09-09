#include "GameState.h"
#include "../SpellByte.h"

namespace SpellByte
{
    GameState::GameState()
    {
        Quit = false;
    }

    void GameState::enter()
    {
        LOG("GameState: enter");

        SceneMgr = APP->OgreRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
        SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

        Camera = SceneMgr->createCamera("GameCamera");
        Camera->setPosition(Ogre::Vector3(-1800, -300, -1100));
        Camera->lookAt(Ogre::Vector3(-300, 300, 0));
        Camera->setNearClipDistance(1);

        Camera->setAspectRatio(Ogre::Real(APP->Viewport->getActualWidth())
                                    / Ogre::Real(APP->Viewport->getActualHeight()));

        APP->Viewport->setCamera(Camera);

        buildGUI();
        createScene();
    }

    bool GameState::pause()
    {
        APP->Log->logMessage("GameState: pause");

        return true;
    }

    void GameState::resume()
    {
        APP->Log->logMessage("GameState: resume");

        buildGUI();

        APP->Viewport->setCamera(Camera);

        Quit = false;
    }

    void GameState::exit()
    {
        APP->Log->logMessage("GameState: exit");

        SceneMgr->destroyCamera(Camera);

        if(SceneMgr)
            APP->OgreRoot->destroySceneManager(SceneMgr);
    }

    void GameState::createScene()
    {

    }

    void GameState::update(const Ogre::FrameEvent &evt)
    {

    }

    void GameState::handleEvents()
    {
        while(CONTROL->hasEvent())
        {
            const UserEvent *tmp = CONTROL->getEvent();
            if(tmp->getType() == UserEvent::ESCAPE)
            {
                pushState(findByName("PauseState"));
            }
        }
    }
}
