/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * OptionsState Class
 *
 * This is Options state class
 *
 */

 #include "OptionsState.h"
 #include "../SpellByte.h"

 namespace SpellByte
 {
    OptionsState::OptionsState():BUTTON_POSITION_START_X(0.4),
                        BUTTON_X_OFFSET(0.0),
                        BUTTON_POSITION_START_Y(0.65),
                        BUTTON_Y_OFFSET(0.08)
    {
        // Create OptionsState, note this does not execute the Options state
        Quit = false;
        FrameEvent = Ogre::FrameEvent();
    }

    void OptionsState::enter()
    {
        // Enter into Options state, output logmessage to inform us of this
        APP->Log->logMessage("OptionsState: enter");

        // Create scene manager derived from ogre root, this is a ST_GENERIC type with name "OptionsSceneMgr"
        SceneMgr = APP->OgreRoot->createSceneManager(Ogre::ST_GENERIC, "OptionsSceneMgr");

        // Set light for Options scene, not sure if this is necessary or if the values are correct
        // but here more as placement
        SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

        // Create camera and set its position and look at
        Camera = SceneMgr->createCamera("OptionsCam");
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

    void OptionsState::createScene()
    {
        buildGUI();
    }

    void OptionsState::buildGUI()
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
           button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OptionsState::buttonClicked, this));
           wRoot->addChild(button);

           btn_current_y += BUTTON_Y_OFFSET;
           btn_current_x += BUTTON_X_OFFSET;
        }

        /*CEGUI::DefaultWindow *resLabel = static_cast<CEGUI::DefaultWindow*>(wmgr.createWindow("TaharezLook/StaticText", "resLabel"));
        resLabel->Enabled
        resLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(btn_current_x, 0), CEGUI::UDim(.1, 0)))
        resLabel->setText("Video Resolution:");
        wRoot->addChild(resLabel);*/
        videoBox = static_cast<CEGUI::Combobox*>(wmgr.createWindow("TaharezLook/Combobox", "resolution"));
        videoBox->setPosition(CEGUI::UVector2(CEGUI::UDim(btn_current_x, 0), CEGUI::UDim(.2, 0)));
        const Ogre::StringVector &videoModes = APP->OgreRoot->getRenderSystem()->getConfigOptions()["Video Mode"].possibleValues;
        //comboBox->setMultiselectEnabled();
        Ogre::String &currentResolution = APP->OgreRoot->getRenderSystem()->getConfigOptions()["Video Mode"].currentValue;

        for(unsigned int i = 0; i < videoModes.size(); ++i)
        {
            LOG(videoModes[i]);
            CEGUI::ListboxTextItem *videoItem = new CEGUI::ListboxTextItem(videoModes[i].c_str(), i);

            videoBox->addItem(videoItem);
            if(videoModes[i] == currentResolution)
            {
                videoBox->setItemSelectState(videoBox->getItemIndex(videoItem), true);
                videoBox->getEditbox()->setText(videoModes[i]);
            }
        }

        wRoot->addChild(videoBox);
    }

    void OptionsState::exit()
    {
        // Exit Options state
        APP->Log->logMessage("OptionsState: exit");

        // Destroy state's CEGUI widgets
        CEGUI::WindowManager::getSingleton().destroyAllWindows();

        // Destroy the camera so next state can create its own camera
        SceneMgr->destroyCamera(Camera);

        // Destroy scene manager
        if(SceneMgr)
            APP->OgreRoot->destroySceneManager(SceneMgr);
    }

    void OptionsState::handleEvents()
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

    void OptionsState::update(const Ogre::FrameEvent &evt)
    {
        if(Quit == true)
        {
            Parent->shutDown();
            return;
        }
    }

    bool OptionsState::buttonClicked(const CEGUI::EventArgs &evt)
    {
        const CEGUI::MouseEventArgs &btnEvents = static_cast<const CEGUI::MouseEventArgs&>(evt);
        CEGUI::String buttonName = btnEvents.window->getName();
        if(buttonName == getButtonID(RETURN))
        {
            popState();
        }
        if(buttonName == getButtonID(APPLY))
        {
            LOG("Video mode set to: " + Ogre::String(videoBox->getText().c_str()));
            APP->OgreRoot->getRenderSystem()->setConfigOption("Video Mode", videoBox->getText().c_str());
            APP->OgreRoot->saveConfig();
        }
        return false;
    }

    const std::string OptionsState::getButtonID(int id) const
    {
        // Different button IDs
        std::string buttonID[BTN_END] = { "ApplyBtn", "ReturnBtn" };
        return buttonID[id];
    }

    const std::string OptionsState::getButtonText(int id) const
    {
        // Button text
        return APP->getString(getButtonID(id));
    }

    void OptionsState::resume()
    {

    }
 }

