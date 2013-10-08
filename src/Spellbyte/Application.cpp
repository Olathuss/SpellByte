/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Application Class
 *
 * This class is the core class
 * and handles all file input/output, OS operations
 * and input.
 *
 */

#include "Application.h"
#include "states/SplashState.h"
#include "states/LoadState.h"
#include "states/MenuState.h"
#include "states/GameState.h"
#include "states/PauseState.h"
#include "states/OptionsState.h"
#include "states/PlayMenuState.h"
#include "ControlManager.h"

namespace SpellByte
{
    template<> Application* Ogre::Singleton<Application>::msSingleton = 0;

    Application::Application(void)
        : OgreRoot(0),
        RenderWindow(0),
        Viewport(0),
        Log(0),
        Timer(0),
        InputMgr(0),
        Keyboard(0),
        Mouse(0)
    {
        language = "english";
        currentDifficulty = "easy";
    }

    Application::~Application(void)
    {
        /*
         * Objects must be deleted in the reverse order
         * from which they are created
         */

         Log->logMessage("Application: Shutting Down...");

        ceguiContext = NULL;
        ceguiRenderer = NULL;
        enabledCollision = true;

        if(GameStateManager)
        {
            delete GameStateManager;
            GameStateManager = 0;
        }

        if(SceneMgr)
        {
            OgreRoot->destroySceneManager(SceneMgr);
            SceneMgr = 0;
        }

        if(Timer)
        {
            delete Timer;
            Timer = 0;
        }

        if(InputMgr)
            OIS::InputManager::destroyInputSystem(InputMgr);

        if(xmlManager)
        {
            delete Ogre::ResourceGroupManager::getSingleton()._getResourceManager("XMLResource");
            xmlManager = 0;
        }

        if(c27Manager)
        {
            delete Ogre::ResourceGroupManager::getSingleton()._getResourceManager("Cube27ResourceFile");
            c27Manager = 0;
        }

        if(OgreRoot)
        {
            delete OgreRoot;
            OgreRoot = 0;
        }

        if(Log)
        {
            delete Log;
            Log = 0;
        }
    }

    bool Application::frameRenderingQueued(const Ogre::FrameEvent &evt)
    {
            Ogre::WindowEventUtilities::messagePump();

            Keyboard->capture();
            Mouse->capture();

            // Handle events
            GameStateManager->handleEvents();

            GameStateManager->update(evt);

            if(RenderWindow->isClosed())
            {
                return false;
            }

            if(GameStateManager->hasShutdown())
            {
                return false;
            }

            return true;
    }

    void Application::execute()
    {
        // Initialize Ogre3D
        if(!initOgre("SpellByte", 0, 0))
            return;

        SceneMgr = OgreRoot->createSceneManager(Ogre::ST_GENERIC, "SpellByteSceneMgr");

        loadConfig();
        language = getConfigString("language");
        // Initialize our strings
        loadStrings(language);

        // Initialize CEGUI
        ceguiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

        CEGUI::Font::setDefaultResourceGroup("Fonts");
        CEGUI::Scheme::setDefaultResourceGroup("Schemes");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
        CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
        CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");

        CEGUI::SchemeManager::getSingleton().createFromFile( "GameMenu.scheme" );
        CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
        ceguiContext = &CEGUI::System::getSingleton().getDefaultGUIContext();
        ceguiContext->getMouseCursor().setDefaultImage( "GameMenuImages/MouseCursor" );
        ceguiContext->injectMousePosition(0.0, 0.0);
        ceguiContext->getMouseCursor().show();

        Log->logMessage("Ogre initialized...");

        GameStateManager = new StateManager();

        SplashState::create(GameStateManager, "SplashState");
        LoadState::create(GameStateManager, "LoadState");
        MenuState::create(GameStateManager, "MenuState");
        OptionsState::create(GameStateManager, "OptionsState");
        PauseState::create(GameStateManager, "PauseState");
        GameState::create(GameStateManager, "GameState");
        PlayMenuState::create(GameStateManager, "PlayMenuState");

        GameStateManager->changeState(GameStateManager->findByName("GameState"));

        OgreRoot->addFrameListener(this);
        OgreRoot->startRendering();

        // Want to make a shutdown function and place anything that needs to be shutdown here.
        CONTROL->shutDown();

        Log->logMessage("Shutting down");
    }

    bool Application::loadStrings(std::string language)
    {
        std::string languageFile = language + ".xml";
        XMLResourcePtr xmlFile = xmlManager->load(languageFile,"Language");

        std::cout << "getXML" << std::endl;
        tinyxml2::XMLDocument *langDoc = xmlFile->getXML();
        if(!langDoc)
        {
            Ogre::String desc = "Error, strings missing for language: " + language;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        for(tinyxml2::XMLElement *element = langDoc->FirstChildElement()->FirstChildElement(); element; element = element->NextSiblingElement())
        {
            const char *key=element->Attribute("id");
            const char *text=element->Attribute("value");
            if(key && text)
            {
                textResource[key] = text;
            }
        }

        return true;
    }

    bool Application::loadConfig()
    {
        std::string configFile = "spellbyte.xml";
        XMLResourcePtr xmlFile = xmlManager->load(configFile,"General");
        tinyxml2::XMLDocument *configDoc = xmlFile->getXML();
        if(!configDoc)
        {
            Ogre::String desc = "Error, config file missing";
            throw(Ogre::Exception(20, desc, "Application"));
        }

        for(tinyxml2::XMLElement *element = configDoc->FirstChildElement("config")->FirstChildElement("strings")->FirstChildElement(); element; element = element->NextSiblingElement())
        {
            const char *key=element->Attribute("id");
            const char *text=element->Attribute("value");
            if(key && text)
            {
                configStrings[key] = text;
            }
        }

        for(tinyxml2::XMLElement *element = configDoc->FirstChildElement("config")->FirstChildElement("floats")->FirstChildElement(); element; element = element->NextSiblingElement())
        {
            const char *key=element->Attribute("id");
            float value=element->FloatAttribute("value");
            if(key)
            {
                configFloats[key] = value;
            }
        }

        return true;
    }

    const std::string Application::getString(std::string ID) const
    {
        std::map<std::string, std::string>::const_iterator it;
        it = textResource.find(ID);
        if(it == textResource.end())
        {
            Ogre::String desc = "Error finding string " + ID;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        return it->second;
    }

    const std::string Application::getConfigString(std::string ID) const
    {
        std::map<std::string, std::string>::const_iterator it;
        it = configStrings.find(ID);
        if(it == configStrings.end())
        {
            Ogre::String desc = "Error finding config " + ID;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        return it->second;
    }

    const float Application::getConfigFloat(std::string ID) const
    {
        std::map<std::string, float>::const_iterator it;
        it = configFloats.find(ID);
        if(it == configFloats.end())
        {
            Ogre::String desc = "Error finding config " + ID;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        return it->second;
    }

    void updateGame(double timeSinceLastFrame)
    {
        // Empty for now.
    }

    bool Application::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
    {
        Ogre::LogManager* logMgr = new Ogre::LogManager();

        Log = logMgr->getSingleton().createLog("OgreLogfile.log", true, true, false);
        #ifdef _DEBUG
            Log->setDebugOutputEnabled(true);
        #else
            Log->setDebugOutputEnabled(false);
        #endif

        #ifdef _DEBUG
            OgreRoot = new Ogre::Root("plugins_d.cfg");
        #else
            OgreRoot = new Ogre::Root("plugins.cfg");
        #endif

        //OgreRoot->restoreConfig();
        if(!OgreRoot->showConfigDialog())
        {
            return false;
        }

        c27Manager = new Cube27ResFileManager();
        xmlManager = new XMLResourceManager();

        Ogre::ResourceGroupManager::getSingleton().declareResource("cube.c27", "Cube27ResourceFile");

        RenderWindow = OgreRoot->initialise(true, wndTitle);

        Viewport = RenderWindow->addViewport(0);
        Viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));

        Viewport->setCamera(0);

        size_t hWnd = 0;
        OIS::ParamList paramList;
        RenderWindow->getCustomAttribute("WINDOW", &hWnd);

        paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

        /*paramList.insert(std::make_pair(std::string("w32_mouse"),
            std::string("DISCL_BACKGROUND" )));
        paramList.insert(std::make_pair(std::string("w32_mouse"),
            std::string("DISCL_NONEXCLUSIVE")));*/

        InputMgr = OIS::InputManager::createInputSystem(paramList);

        Keyboard = static_cast<OIS::Keyboard*>(InputMgr->createInputObject(OIS::OISKeyboard, true));
        Mouse = static_cast<OIS::Mouse*>(InputMgr->createInputObject(OIS::OISMouse, true));

        Mouse->getMouseState().height = RenderWindow->getHeight();
        Mouse->getMouseState().width = RenderWindow->getWidth();

        if(pKeyListener == 0)
            Keyboard->setEventCallback(this);
        else
            Keyboard->setEventCallback(pKeyListener);

        if(pMouseListener == 0)
            Mouse->setEventCallback(this);
        else
            Mouse->setEventCallback(pMouseListener);

        Ogre::String secName, typeName, archName;
        Ogre::ConfigFile cf;
        cf.load("resources.cfg");

        Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
        while(seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
            Ogre::ConfigFile::SettingsMultiMap::iterator it;
            for(it = settings->begin(); it != settings->end(); ++it)
            {
                typeName = it->first;
                archName = it->second;
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
            }
        }
        Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

        // Get resource file loaded first
        Cube27ResFilePtr c27File = c27Manager->load("cube.c27", "Cube27Resource");
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        c27File->load();

        Timer = new Ogre::Timer();
        Timer->reset();

        RenderWindow->setActive(true);

        return true;
    }

    bool Application::keyPressed(const OIS::KeyEvent &keyEventRef)
    {
        // Following comment is left for reference:
        //Log->logMessage(Keyboard->getAsString(keyEventRef.key));
        //CEGUI::InjectedInputReceiver::injectKeyDown(keyEventRef.key);
        //CEGUI::InjectedInputReceiver::injectChar(keyEventRef.text);

        UserEvent *tmp = NULL;
        if(Keyboard->isKeyDown(OIS::KC_SYSRQ))
        {
            RenderWindow->writeContentsToTimestampedFile("SpellByte_Screenshot_", ".jpg");
            return true;
        }
        else if(keyEventRef.key == OIS::KC_MINUS)
        {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_DOWN_ON);
        }
        else if(keyEventRef.key == OIS::KC_EQUALS)
        {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_UP_ON);
        }
        else if(keyEventRef.key == OIS::KC_PGUP)
        {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_UP_ON);
        }
        else if(keyEventRef.key == OIS::KC_PGDOWN)
        {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_DOWN_ON);
        }
        else if(keyEventRef.key == OIS::KC_C)
        {
            enabledCollision = (enabledCollision == true ? false : true);
        }
        else if(keyEventRef.key == OIS::KC_ESCAPE)
        {
            tmp = new UserEvent(UserEvent::ESCAPE);
        }
        else if(keyEventRef.key == OIS::KC_W)
        {
            tmp = new UserEvent(UserEvent::PLAYER_FORWARD_ON);
        }
        else if(keyEventRef.key == OIS::KC_S)
        {
            tmp = new UserEvent(UserEvent::PLAYER_BACKWARD_ON);
        }
        else if(keyEventRef.key == OIS::KC_A)
        {
            tmp = new UserEvent(UserEvent::PLAYER_LEFT_ON);
        }
        else if(keyEventRef.key == OIS::KC_D)
        {
            tmp = new UserEvent(UserEvent::PLAYER_RIGHT_ON);
        }
        else if(keyEventRef.key == OIS::KC_Q)
        {
            tmp = new UserEvent(UserEvent::PLAYER_UP_ON);
        }
        else if(keyEventRef.key == OIS::KC_E)
        {
            tmp = new UserEvent(UserEvent::PLAYER_DOWN_ON);
        }
        if(tmp != NULL)
        {
            CONTROL->addEvent(tmp);
        }

        return true;
    }

    bool Application::keyReleased(const OIS::KeyEvent &keyEventRef)
    {
        //CEGUI::InjectedInputReceiver::injectKeyUp(keyEventRef.key);
        UserEvent *tmp = NULL;
        if(keyEventRef.key == OIS::KC_MINUS)
        {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_DOWN_OFF);
        }
        else if(keyEventRef.key == OIS::KC_EQUALS)
        {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_UP_OFF);
        }
        else if(keyEventRef.key == OIS::KC_PGUP)
        {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_UP_OFF);
        }
        else if(keyEventRef.key == OIS::KC_PGDOWN)
        {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_DOWN_OFF);
        }
        else if(keyEventRef.key == OIS::KC_W)
        {
            tmp = new UserEvent(UserEvent::PLAYER_FORWARD_OFF);
        }
        else if(keyEventRef.key == OIS::KC_S)
        {
            tmp = new UserEvent(UserEvent::PLAYER_BACKWARD_OFF);
        }
        else if(keyEventRef.key == OIS::KC_A)
        {
            tmp = new UserEvent(UserEvent::PLAYER_LEFT_OFF);
        }
        else if(keyEventRef.key == OIS::KC_D)
        {
            tmp = new UserEvent(UserEvent::PLAYER_RIGHT_OFF);
        }
        else if(keyEventRef.key == OIS::KC_Q)
        {
            tmp = new UserEvent(UserEvent::PLAYER_UP_OFF);
        }
        else if(keyEventRef.key == OIS::KC_E)
        {
            tmp = new UserEvent(UserEvent::PLAYER_DOWN_OFF);
        }
        if(tmp != NULL)
        {
            CONTROL->addEvent(tmp);
        }
        return true;
    }

    bool Application::mouseMoved(const OIS::MouseEvent &evt)
    {
        // Following comment is left as reference
        //Log->logMessage(Ogre::StringConverter::toString(evt.state.X.rel));
        ceguiContext->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
        if(evt.state.Z.rel)
        {
            ceguiContext->injectMouseWheelChange(evt.state.Z.rel / 120.0f);
        }

        UserEvent* tmp = new UserEvent(UserEvent::MOUSE_MOVE);
        tmp->setAbs(evt.state.X.abs, evt.state.Y.abs);
        tmp->setRel(evt.state.X.rel, evt.state.Y.rel);
        CONTROL->addEvent(tmp);

        return true;
    }

    CEGUI::MouseButton Application::convertButton(OIS::MouseButtonID buttonID)
    {
        switch(buttonID)
        {
        case OIS::MB_Left:
            return CEGUI::LeftButton;
        case OIS::MB_Right:
            return CEGUI::RightButton;
        case OIS::MB_Middle:
            return CEGUI::MiddleButton;
        default:
            return CEGUI::LeftButton;
        }
    }

    bool Application::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
    {
        ceguiContext->injectMouseButtonDown(convertButton(id));
        UserEvent *tmp = NULL;
        switch(id)
        {
        case OIS::MB_Left:
            {
                tmp = new UserEvent(UserEvent::MOUSE_LEFT_PRESS);
                break;
            }
        case OIS::MB_Right:
            {
                tmp = new UserEvent(UserEvent::MOUSE_RIGHT_PRESS);
                break;
            }
        default:
            {
                tmp = new UserEvent(UserEvent::MOUSE_OTHER_PRESS);
                break;
            }
        }
        tmp->setAbs(evt.state.X.abs, evt.state.Y.abs);
        tmp->setRel(evt.state.X.rel, evt.state.Y.rel);
        CONTROL->addEvent(tmp);

        return true;
    }

    bool Application::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
    {
        ceguiContext->injectMouseButtonUp(convertButton(id));
        UserEvent *tmp = NULL;
        switch(id)
        {
        case OIS::MB_Left:
            {
                tmp = new UserEvent(UserEvent::MOUSE_LEFT_RELEASE);
                break;
            }
        case OIS::MB_Right:
            {
                tmp = new UserEvent(UserEvent::MOUSE_RIGHT_RELEASE);
                break;
            }
        default:
            {
                tmp = new UserEvent(UserEvent::MOUSE_OTHER_RELEASE);
                break;
            }
        }
        tmp->setAbs(evt.state.X.abs, evt.state.Y.abs);
        tmp->setRel(evt.state.X.rel, evt.state.Y.rel);
        CONTROL->addEvent(tmp);

        return true;
    }
}
