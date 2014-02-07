/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * MenuState Class
 *
 * This is menu state class
 *
 */

 #include "MenuState.h"
 #include "../define.h"

 namespace SpellByte
 {
     MenuState::MenuState():BUTTON_POSITION_START_X(0.8),
                            BUTTON_X_OFFSET(0.0),
                            BUTTON_POSITION_START_Y(0.45),
                            BUTTON_Y_OFFSET(0.05)
     {
         // Create MenuState, note this does not execute the menu state
         Quit = false;
         FrameEvent = Ogre::FrameEvent();
     }

     void MenuState::enter()
     {
        // Enter into menu state, output logmessage to inform us of this
        APP->Log->logMessage("MenuState: enter");

        // Create scene manager derived from ogre root, this is a ST_GENERIC type with name "MenuSceneMgr"
        SceneMgr = APP->SceneMgr;//APP->OgreRoot->createSceneManager(Ogre::ST_GENERIC, "MenuSceneMgr");

        // Set light for menu scene, not sure if this is necessary or if the values are correct
        // but here more as placement
        SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

        // Create camera and set its position and look at
        Camera = SceneMgr->createCamera("MenuCam");
        Camera->setPosition(Ogre::Vector3(-300, 300, -1100));
        Camera->lookAt(Ogre::Vector3(-300, 300, 0));
        Camera->setNearClipDistance(1);

        // Set aspect ratio of camera
        Camera->setAspectRatio(Ogre::Real(APP->Viewport->getActualWidth()) /
                               Ogre::Real(APP->Viewport->getActualHeight()));

        // Tell the application this camera is going to be the one for the viewport
        APP->Viewport->setCamera(Camera);

        wRoot = NULL;
        CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
        wRoot = wmgr.createWindow("DefaultWindow", "root");
        APP->ceguiContext->setRootWindow( wRoot );
        APP->ceguiContext->getMouseCursor().show();

        // Create the scene as is necessary
        createScene();
     }

     void MenuState::createScene() {
        buildGUI();
     }

     void MenuState::buildGUI() {
        LOG("Creating Menu");
        // Create root window from window manager
        CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

        // Below is just here for testing purposes
        /*CEGUI::FrameWindow *fWnd = static_cast<CEGUI::FrameWindow*>(wmgr.createWindow("TaharezLook/FrameWindow", "testWindow"));
        wRoot->addChild(fWnd);
        fWnd->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25f, 0) , CEGUI::UDim(0.25f, 0)));
        fWnd->setSize(CEGUI::USize(CEGUI::UDim(0.5f, 0), CEGUI::UDim(0.5f, 0)));
        fWnd->setText("Just a test!");*/

        // Create each button
        float btn_current_x = BUTTON_POSITION_START_X;
        float btn_current_y = BUTTON_POSITION_START_Y;

        // For button positioning, UDim has two values: the first is for scale value (0 to 1.0),
        // and the second is for offset in pixel value.  In other words, UDim(relative, absolute)
        // So this needs to be changed depending on which one we want to use
        for (int i = 0; i < BTN_END; ++i)
        {
           CEGUI::Window *button = wmgr.createWindow("TaharezLook/Button", getButtonID(i));
           button->setText(getButtonText(i));
           button->setSize(CEGUI::USize(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.05f, 0)));
           button->setPosition(CEGUI::UVector2(CEGUI::UDim(btn_current_x, 0), CEGUI::UDim(btn_current_y, 0)));
           button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::buttonClicked, this));
           wRoot->addChild(button);

           btn_current_y += BUTTON_Y_OFFSET;
           btn_current_x += BUTTON_X_OFFSET;
        }
        LOG("Menu Created");
     }

    void MenuState::exit()
    {
        APP->ceguiContext->getMouseCursor().hide();

        // Exit menu state
        APP->Log->logMessage("MenuState: exit");

        // Destroy state's CEGUI widgets
        CEGUI::WindowManager::getSingleton().destroyAllWindows();
        wRoot = NULL;

        // Destroy the camera so next state can create its own camera
        SceneMgr->destroyCamera(Camera);

        // Destroy scene manager
        SceneMgr = 0;
    }

    void MenuState::handleEvents()
    {
        while(CONTROL->hasEvent())
        {
            const UserEvent *tmp = CONTROL->getEvent();
            if(tmp->getType() == UserEvent::ESCAPE)
            {
                Quit = true;
            }
        }
    }

    bool MenuState::update(const Ogre::FrameEvent &evt)
    {
        if(Quit == true)
        {
            Parent->shutDown();
            return false;
        }

        return true;
    }

    bool MenuState::buttonClicked(const CEGUI::EventArgs &evt)
    {
        const CEGUI::MouseEventArgs &btnEvents = static_cast<const CEGUI::MouseEventArgs&>(evt);
        CEGUI::String buttonName = btnEvents.window->getName();

        if(buttonName == getButtonID(PLAY_GAME))
        {
            changeState(findByName("GameState"));
            return true;
        }
        else if(buttonName == getButtonID(OPTIONS))
        {
            pushState(findByName("OptionsState"));
            return true;
        }
        else if(buttonName == getButtonID(EXIT))
        {
            Quit = true;
            return true;
             }
        return false;
    }

    const std::string MenuState::getButtonID(int id) const
    {
        // Different button IDs
        std::string buttonID[BTN_END] = { "EnterBtn", "OptionsBtn", "CreditsBtn", "ExitBtn" };
        return buttonID[id];
    }

    const std::string MenuState::getButtonText(int id) const
    {
        return APP->getString(getButtonID(id));
    }

    void MenuState::resume()
    {
        buildGUI();

        APP->Viewport->setCamera(Camera);

        APP->ceguiContext->getMouseCursor().show();
    }
 }
