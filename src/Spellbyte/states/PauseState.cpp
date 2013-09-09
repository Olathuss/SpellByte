/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * PauseState Class
 *
 * This is Pause state class
 *
 */

 #include "PauseState.h"
 #include "../SpellByte.h"

 namespace SpellByte
 {
     PauseState::PauseState():BUTTON_POSITION_START_X(0.4),
                            BUTTON_X_OFFSET(0.0),
                            BUTTON_POSITION_START_Y(0.45),
                            BUTTON_Y_OFFSET(0.08)
     {
         // Create PauseState, note this does not execute the Pause state
         Quit = false;
         FrameEvent = Ogre::FrameEvent();
     }

     void PauseState::enter()
     {
        // Enter into Pause state, output logmessage to inform us of this
        APP->Log->logMessage("PauseState: enter");

        // Create scene manager derived from ogre root, this is a ST_GENERIC type with name "PauseSceneMgr"
        SceneMgr = APP->OgreRoot->createSceneManager(Ogre::ST_GENERIC, "PauseSceneMgr");

        // Set light for Pause scene, not sure if this is necessary or if the values are correct
        // but here more as placement
        SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

        // Create camera and set its position and look at
        Camera = SceneMgr->createCamera("PauseCam");
        Camera->setPosition(Ogre::Vector3(-300, 300, -1100));
        Camera->lookAt(Ogre::Vector3(-300, 300, 0));
        Camera->setNearClipDistance(1);

        // Set aspect ratio of camera
        Camera->setAspectRatio(Ogre::Real(APP->Viewport->getActualWidth()) /
                               Ogre::Real(APP->Viewport->getActualHeight()));

        // Tell the application this camera is going to be the one for the viewport
        APP->Viewport->setCamera(Camera);

        // Create the scene as is necessary
        createScene();
     }

     void PauseState::createScene()
     {
        buildGUI();
     }

     void PauseState::buildGUI()
     {
        // Create root window from window manager
        CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
        CEGUI::Window *wRoot = wmgr.createWindow("DefaultWindow", "root");
        APP->ceguiContext->setRootWindow( wRoot );

        // Create each button
        float btn_current_x = BUTTON_POSITION_START_X;
        float btn_current_y = BUTTON_POSITION_START_Y;

        // For button positioning, UDim has two values: the first is for scale value (0 to 1.0),
        // and the second is for offset in pixel value.  In other words, UDim(relative, absolute)
        // So this needs to be changed depending on which one we want to use
        for(int i = 0; i < BTN_END; ++i)
        {
           CEGUI::Window *button = wmgr.createWindow("TaharezLook/Button", getButtonID(i));
           button->setText(getButtonText(i));
           button->setSize(CEGUI::USize(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.05f, 0)));
           button->setPosition(CEGUI::UVector2(CEGUI::UDim(btn_current_x, 0), CEGUI::UDim(btn_current_y, 0)));
           button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseState::buttonClicked, this));
           wRoot->addChild(button);

           btn_current_y += BUTTON_Y_OFFSET;
           btn_current_x += BUTTON_X_OFFSET;
        }
     }

    void PauseState::exit()
    {
        // Exit Pause state
        APP->Log->logMessage("PauseState: exit");

        // Destroy state's CEGUI widgets
        CEGUI::WindowManager::getSingleton().destroyAllWindows();

        // Destroy the camera so next state can create its own camera
        SceneMgr->destroyCamera(Camera);

        // Destroy scene manager
        if(SceneMgr)
            APP->OgreRoot->destroySceneManager(SceneMgr);
    }

    void PauseState::handleEvents()
    {
        while(CONTROL->hasEvent())
        {
            const UserEvent *tmp = CONTROL->getEvent();
            if(tmp->getType() == UserEvent::ESCAPE)
            {
                popState();
            }
        }
    }

    void PauseState::update(const Ogre::FrameEvent &evt)
    {
        if(Quit == true)
        {
            Parent->shutDown();
            return;
        }
    }

    bool PauseState::buttonClicked(const CEGUI::EventArgs &evt)
    {
        const CEGUI::MouseEventArgs &btnEvents = static_cast<const CEGUI::MouseEventArgs&>(evt);
        CEGUI::String buttonName = btnEvents.window->getName();
        if(buttonName == getButtonID(RESUME))
        {
            popState();
        }
        if(buttonName == getButtonID(OPTIONS))
        {
            pushState(findByName("OptionsState"));
        }
        if(buttonName == getButtonID(MAIN_MENU))
        {
            popAllAndPushState(findByName("MenuState"));
        }
        return false;
    }

    const std::string PauseState::getButtonID(int id) const
    {
        // Different button IDs
        std::string buttonID[BTN_END] = { "ResumeBtn", "OptionsBtn", "ExitToMainBtn" };
        return buttonID[id];
    }

    const std::string PauseState::getButtonText(int id) const
    {
        // Button text
        return APP->getString(getButtonID(id));
    }

    void PauseState::resume()
    {
        buildGUI();

        APP->Viewport->setCamera(Camera);
    }
 }

