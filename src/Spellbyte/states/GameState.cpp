#include "GameState.h"
#include "../SpellByte.h"

namespace SpellByte
{
    GameState::GameState()
    {
        mCollisionTools = 0;
        Quit = false;
    }

    GameState::~GameState()
    {
        SceneMgr = 0;
    }

    void GameState::enter()
    {
        LOG("GameState: enter");

        SceneMgr = APP->SceneMgr;
        Camera = SceneMgr->createCamera("GameCamera");
        Camera->setAspectRatio(Ogre::Real(APP->Viewport->getActualWidth())
                                / Ogre::Real(APP->Viewport->getActualHeight()));

        Camera->setNearClipDistance(0.1);
        Camera->setFarClipDistance(100);
        APP->Viewport->setCamera(Camera);
        APP->ceguiContext->getMouseCursor().hide();

        player.init(SceneMgr, Camera, &gameWorld);

        //CameraMan = new OgreBites::SdkCameraMan(Camera);

        gameWorld.init(Camera, &player);
        gameWorld.loadWorld(APP->getConfigString("world"));
        buildGUI();
        createScene();
        mCollisionTools = new MOC::CollisionTools(SceneMgr, gameWorld.terrainGroup);
        player.setCollisionHanlder(mCollisionTools);
        gameWorld.setCollisionTool(mCollisionTools);
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

        APP->ceguiContext->getMouseCursor().hide();

        Quit = false;
    }

    void GameState::exit()
    {
        APP->Log->logMessage("GameState: exit");

        destroyScene();

        if(mCollisionTools)
        {
            delete mCollisionTools;
        }

        SceneMgr->destroyCamera(Camera);

        //SceneMgr->destroyQuery(RSQ);
        SceneMgr = 0;
    }

    void GameState::createScene()
    {
        gameWorld.createScene();

        CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
        rootWindow = wmgr.createWindow("DefaultWindow", "root");
        APP->ceguiContext->setRootWindow( rootWindow );
        debugBox = wmgr.createWindow("TaharezLook/StaticText", "textbox");
        rootWindow->addChild(debugBox);
        GCW->Init(rootWindow);
    }

    void GameState::destroyScene()
    {
        gameWorld.destroyScene();

        rootWindow->destroyChild(debugBox);
        GCW->Disable();
    }

    void GameState::update(const Ogre::FrameEvent &evt)
    {
        player.update(evt);
        gameWorld.update(evt);

        debugBox->setText(player.getDebugString().c_str());
    }

    void GameState::handleEvents()
    {
        while(CONTROL->hasEvent())
        {
            const UserEvent *tmp = CONTROL->getEvent();
            if(tmp->getType() == UserEvent::TERMINAL)
            {
                if(!GCW->isVisible())
                {
                    LOG("Console enabled");
                    APP->ceguiContext->getMouseCursor().show();
                    GCW->setVisible(true);
                }
                else
                {
                    LOG("Console disabled");
                    APP->ceguiContext->getMouseCursor().hide();
                    GCW->setVisible(false);
                }
                continue;
            }
            else
            {
                if(GCW->isVisible())
                {
                    continue;
                }
            }
            if(tmp->getType() > UserEvent::PLAYER_ACTION_START && tmp->getType() < UserEvent::PLAYER_ACTION_END)
            {
                player.handleEvent(tmp->getType());
                continue;
            }
            else if(tmp->getType() > UserEvent::WORLD_ACTION_START && tmp->getType() < UserEvent::WORLD_ACTION_END)
            {
                gameWorld.handleEvent(tmp->getType());
                continue;
            }
            switch(tmp->getType())
            {
            case UserEvent::ESCAPE:
                pushState(findByName("PauseState"));
                break;
            case UserEvent::MOUSE_MOVE:
                int x, y;
                tmp->getRel(x, y);
                player.setRotation(x, y);
                break;
            default:
                break;
            }
        }
    }
}
