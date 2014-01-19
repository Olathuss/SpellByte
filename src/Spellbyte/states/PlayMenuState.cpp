/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * PlayMenuState Class
 *
 * This is PlayMenu state class
 *
 */

 #include "PlayMenuState.h"
 #include "../SpellByte.h"

 namespace SpellByte
 {
     PlayMenuState::PlayMenuState():BUTTON_POSITION_START_X(0.4),
                            BUTTON_X_OFFSET(0.0),
                            BUTTON_POSITION_START_Y(0.45),
                            BUTTON_Y_OFFSET(0.08)
     {
         // Create PlayMenuState, note this does not execute the PlayMenu state
         Quit = false;
         FrameEvent = Ogre::FrameEvent();
     }

     void PlayMenuState::enter()
     {
        // Enter into PlayMenu state, output logmessage to inform us of this
        APP->Log->logMessage("PlayMenuState: enter");

        // Create scene manager derived from ogre root, this is a ST_GENERIC type with name "PlayMenuSceneMgr"
        SceneMgr = APP->OgreRoot->createSceneManager(Ogre::ST_GENERIC, "PlayMenuSceneMgr");

        // Set light for PlayMenu scene, not sure if this is necessary or if the values are correct
        // but here more as placement
        SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

        // Create camera and set its position and look at
        Camera = SceneMgr->createCamera("PlayMenuCam");
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

     void PlayMenuState::createScene()
     {
        buildGUI();
     }

     void PlayMenuState::buildGUI()
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
           button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PlayMenuState::buttonClicked, this));
           wRoot->addChild(button);

           btn_current_y += BUTTON_Y_OFFSET;
           btn_current_x += BUTTON_X_OFFSET;
        }
     }

    void PlayMenuState::exit()
    {
        // Exit PlayMenu state
        APP->Log->logMessage("PlayMenuState: exit");

        // Destroy state's CEGUI widgets
        CEGUI::WindowManager::getSingleton().destroyAllWindows();

        // Destroy the camera so next state can create its own camera
        SceneMgr->destroyCamera(Camera);

        // Destroy scene manager
        if(SceneMgr)
            APP->OgreRoot->destroySceneManager(SceneMgr);
    }

    void PlayMenuState::handleEvents()
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

    void PlayMenuState::update(const Ogre::FrameEvent &evt)
    {
        if(Quit == true)
        {
            Parent->shutDown();
            return;
        }
    }

    bool PlayMenuState::buttonClicked(const CEGUI::EventArgs &evt)
    {
        const CEGUI::MouseEventArgs &btnEvents = static_cast<const CEGUI::MouseEventArgs&>(evt);
        CEGUI::String buttonName = btnEvents.window->getName();
        if(buttonName == getButtonID(RETURN_MAIN))
        {
            popState();
        }
        if(buttonName == getButtonID(STORY_MODE))
        {
            popAllAndPushState(findByName("GameState"));
        }
        if(buttonName == getButtonID(CUSTOM_MODE))
        {
            popAllAndPushState(findByName("GameState"));
        }
        return false;
    }

    const std::string PlayMenuState::getButtonID(int id) const
    {
        // Different button IDs
        std::string buttonID[BTN_END] = { "StoryBtn", "ChallengeBtn", "CustomGameBtn", "ReturnBtn" };
        return buttonID[id];
    }

    const std::string PlayMenuState::getButtonText(int id) const
    {
        // Button text
        return APP->getString(getButtonID(id));
    }

    void PlayMenuState::resume()
    {
        buildGUI();

        APP->Viewport->setCamera(Camera);
    }
 }


