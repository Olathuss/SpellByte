#include "GameState.h"
#include "../SpellByte.h"

namespace SpellByte
{
    GameState::GameState()
    {
        debugBox = NULL;
        rootWindow = NULL;
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

        //CameraMan = new OgreBites::SdkCameraMan(Camera);

        gameWorld.init(Camera, &player);
        gameWorld.loadWorld(APP->getConfigString("world"));
        player.init(SceneMgr, Camera, &gameWorld);
        LOG("Building GUI");
        buildGUI();
        LOG("GUI complete");
        LOG("Creating scene");
        createScene();
        LOG("Scene creation complete");
        LOG("Initializing collision tools");
        mCollisionTools = new MOC::CollisionTools(SceneMgr, &gameWorld);
        player.setCollisionHandler(mCollisionTools);
        gameWorld.setCollisionTool(mCollisionTools);
        LOG("Collision tools initialization complete");
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

    void GameState::buildGUI() {
        if (rootWindow == NULL) {
            CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
            rootWindow = wmgr.createWindow("DefaultWindow", "root");
            APP->ceguiContext->setRootWindow( rootWindow );
            debugBox = wmgr.createWindow("TaharezLook/StaticText", "textbox");
            rootWindow->addChild(debugBox);
            GCW->Init(rootWindow);
        }
    }

    void GameState::createScene() {
        LOG("Scene creation, world");
        gameWorld.createScene();
        LOG("World scene creation complete");
    }

    void GameState::destroyScene()
    {
        gameWorld.destroyScene();

        rootWindow->destroyChild(debugBox);
        GCW->Disable();
    }

    bool GameState::update(const Ogre::FrameEvent &evt) {
        player.update(evt);
        gameWorld.update(evt);

        if(debugBox) {
            Ogre::String debugText;

            debugText =
                Ogre::String("FPS: " +
                             Ogre::StringConverter::toString(APP->RenderWindow->getAverageFPS()) +
                             "\n" +
                             "Triangle Count: " +
                             Ogre::StringConverter::toString(APP->RenderWindow->getTriangleCount()) +
                             "\n" +
                             player.getDebugString());
            debugBox->setText(debugText.c_str());
        }

        return true;
    }

    void GameState::handleEvents() {
        while(CONTROL->hasEvent()) {
            const UserEvent *tmp = CONTROL->getEvent();
            if(tmp->getType() == UserEvent::TERMINAL) {
                if(!GCW->isVisible()) {
                    LOG("Console enabled");
                    APP->ceguiContext->getMouseCursor().show();
                    GCW->setVisible(true);
                } else {
                    LOG("Console disabled");
                    APP->ceguiContext->getMouseCursor().hide();
                    GCW->setVisible(false);
                }
                continue;
            } else {
                if(GCW->isVisible()) {
                    continue;
                }
            }
            if(tmp->getType() > UserEvent::PLAYER_ACTION_START &&
               tmp->getType() < UserEvent::PLAYER_ACTION_END) {
                player.handleEvent(tmp->getType());
                continue;
            } else if(tmp->getType() > UserEvent::WORLD_ACTION_START &&
                      tmp->getType() < UserEvent::WORLD_ACTION_END) {
                gameWorld.handleEvent(tmp->getType());
                continue;
            }
            switch(tmp->getType()) {
            case UserEvent::ESCAPE:
                pushState(findByName("PauseState"));
                break;
            case UserEvent::MOUSE_MOVE:
                int x, y;
                tmp->getRel(x, y);
                player.setRotation(x, y);
                break;
            case UserEvent::CYCLE_AFILTER_MODE:
                APP->cycleAFilter();
                break;
            case UserEvent::CYCLE_POLYGON_MODE:
                {
                    Ogre::PolygonMode pm;
                    switch(Camera->getPolygonMode()) {
                    case Ogre::PM_SOLID:
                        pm = Ogre::PM_WIREFRAME;
                        break;
                    case Ogre::PM_WIREFRAME:
                        pm = Ogre::PM_POINTS;
                        break;
                    default:
                        pm = Ogre::PM_SOLID;
                    }

                    Camera->setPolygonMode(pm);
                }
                break;
            default:
                break;
            }
        }
    }
}
