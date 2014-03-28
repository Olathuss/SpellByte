/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * Application Class
 *
 * This class is the core class
 * and handles all file input/output, OS operations
 * and input.
 *
 */

#include "Application.h"
#include "SLB.hpp"
#include "console/LuaManager.h"
#include "states/SplashState.h"
#include "states/LoadState.h"
#include "states/MenuState.h"
#include "states/GameState.h"
#include "states/PauseState.h"
#include "states/OptionsState.h"
#include "states/PlayMenuState.h"
#include "ControlManager.h"

#include "audio/AudioManager.h"

namespace SpellByte {
    template<> Application* Ogre::Singleton<Application>::msSingleton = 0;

    Application::Application(void)
        : OgreRoot(0),
        RenderWindow(0),
        Viewport(0),
        Log(0),
        Timer(0),
        InputMgr(0),
        Keyboard(0),
        Mouse(0),
        SceneMgr(0),
        SBResourceManager(0),
        xmlManager(0),
        wavManager(0),
        ceguiRenderer(0),
        ceguiContext(0),
        GameStateManager(0) {
        language = "english";
        currentDifficulty = "easy";
    }

    Application::~Application(void) {
        /*
         * Objects must be deleted in the reverse order
         * from which they are created
         */

        Log->logMessage("Application: Shutting Down...");

        ceguiContext = NULL;
        ceguiRenderer = NULL;
        enabledCollision = true;

        if (GameStateManager) {
            delete GameStateManager;
            GameStateManager = 0;
        }

        if (SceneMgr) {
            OgreRoot->destroySceneManager(SceneMgr);
            SceneMgr = 0;
        }

        if (Timer) {
            delete Timer;
            Timer = 0;
        }

        if (InputMgr)
            OIS::InputManager::destroyInputSystem(InputMgr);
        if (wavManager) {
            delete Ogre::ResourceGroupManager::getSingleton()._getResourceManager("WAVFile");
            wavManager = 0;
        }
        if (xmlManager) {
            delete Ogre::ResourceGroupManager::getSingleton()._getResourceManager("XMLResource");
            xmlManager = 0;
        }

        if (SBResourceManager) {
            delete Ogre::ResourceGroupManager::getSingleton()._getResourceManager("SBResourceFile");
            SBResourceManager = 0;
        }

        if (OgreRoot) {
            delete OgreRoot;
            OgreRoot = 0;
        }

        if (Log) {
            delete Log;
            Log = 0;
        }
    }

    bool Application::frameRenderingQueued(const Ogre::FrameEvent &evt) {
            //static int loopcount = 1;
            //LOG("Start of loop #" + Ogre::StringConverter::toString(loopcount));
            //LOG("WindowEvent pump");
            // Pop Window's event messages off
            Ogre::WindowEventUtilities::messagePump();

            // Capture events
            //LOG("Capture events");
            Keyboard->capture();
            Mouse->capture();

            // Handle events
            //LOG("Handle events");
            GameStateManager->handleEvents();

            // Update game logic
            //LOG("Update GameStateManager");
            GameStateManager->update(evt);

            // Update AudioManager to remove sounds which aren't playing
            AUDIOMAN->update(evt);

            // If Window is closed, then quit
            //LOG("Check RenderWindow closed");
            if (RenderWindow->isClosed()) {
                return false;
            }

            // If game has signal shut down, then quit
            //LOG("Check for shutdown");
            if (GameStateManager->hasShutdown()) {
                return false;
            }

            // Continue
            //LOG("return true");
            //loopcount++;
            return true;
    }

    void Application::execute() {
        // Initialize Ogre3D
        if (!initOgre("SpellByte", 0, 0))
            return;
        LOG("Ogre Initialization Complete");

        // Initialize LUA and bind APP to LUA
        LOG("Initializing LUA Manager");
        LuaManager::getInstance()->Init();
        LOG("Binding Application to LUA");
        bindToLUA();
        LOG("LUA Initialization Complete");

        LOG("Initializing AudioManager");
        AUDIOMAN->init();
        LOG("AudioManager Initialized");

        // Create Ogre scene manager
        SceneMgr = OgreRoot->createSceneManager(Ogre::ST_GENERIC, "SpellByteSceneMgr");

        // Load SpellByte configurations
        LOG("Loading SpellByte configuration files");
        loadConfig();
        // Set language and load it
        language = getConfigString("language");
        // Initialize our strings
        loadStrings(language);

        LOG("Initializing CEGUI");
        // Initialize CEGUI
        ceguiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

        // Tell CEGUI in which group to find its resources from Ogre
        CEGUI::Font::setDefaultResourceGroup("Fonts");
        CEGUI::Scheme::setDefaultResourceGroup("Schemes");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
        CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
        CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");

        // Load CEGUI configurations and skin/interface
        CEGUI::SchemeManager::getSingleton().createFromFile( "GameMenu.scheme" );
        CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
        CEGUI::SchemeManager::getSingleton().createFromFile( "VanillaSkin.scheme" );
        ceguiContext = &CEGUI::System::getSingleton().getDefaultGUIContext();
        ceguiContext->getMouseCursor().setDefaultImage( "GameMenuImages/MouseCursor" );
        ceguiContext->injectMousePosition(0.0, 0.0);
        ceguiContext->getMouseCursor().show();
        LOG("CEGUI Initialized");

        // Create GameStateManager (GSM)
        LOG("Initializing GameStateManager");
        GameStateManager = new StateManager();

        // Create and load all necessary states for GSM
        LOG("Initializing States");
        SplashState::create(GameStateManager, "SplashState");
        LoadState::create(GameStateManager, "LoadState");
        MenuState::create(GameStateManager, "MenuState");
        OptionsState::create(GameStateManager, "OptionsState");
        PauseState::create(GameStateManager, "PauseState");
        GameState::create(GameStateManager, "GameState");
        PlayMenuState::create(GameStateManager, "PlayMenuState");

        // Set entry state
        LOG("Setting initial state");
        GameStateManager->changeState(GameStateManager->findByName("GameState"));

        // Tell ogre to make application the FrameListener, then start rendering
        LOG("Setting to FrameListener");
        OgreRoot->addFrameListener(this);
        LOG("SpellByte Initialization Complete, starting...");
        OgreRoot->startRendering();

        // Want to make a shutdown function and place anything that needs to be shutdown here??
        LOG("Shutting down");

        // Shutdown control to ensure all events are deleted
        LOG("Shuttin down control");
        CONTROL->shutDown();
    }

    // Load strings for particular language
    bool Application::loadStrings(std::string language) {
        // Language file must be in format "language name" + ".xml"
        std::string languageFile = language + ".xml";
        // Language files should be in resource group "Language"
        XMLResourcePtr xmlFile = xmlManager->load(languageFile,"Language");

        // Load XML file for language and ensure it exists
        std::cout << "getXML" << std::endl;
        tinyxml2::XMLDocument *langDoc = xmlFile->getXML();
        if (!langDoc) {
            Ogre::String desc = "Error, strings missing for language: " + language;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        // Loop through strings and add to string map
        for (tinyxml2::XMLElement *element = langDoc->FirstChildElement()->FirstChildElement();
            element; element = element->NextSiblingElement()) {
            const char *key=element->Attribute("id");
            const char *text=element->Attribute("value");
            if (key && text) {
                textResource[key] = text;
            }
        }

        // Unload language XML file
        xmlFile->unload();
        return true;
    }

    // Load SpellByte.xml file
    bool Application::loadConfig() {
        std::string configFile = "spellbyte.xml";
        // Load XML file and ensure it exists
        XMLResourcePtr xmlFile = xmlManager->load(configFile,"General");
        tinyxml2::XMLDocument *configDoc = xmlFile->getXML();
        if (!configDoc) {
            Ogre::String desc = "Error, config file missing";
            throw(Ogre::Exception(20, desc, "Application"));
        }

        // Loop through strings and load into string map
        for (tinyxml2::XMLElement *element = configDoc
             ->FirstChildElement("config")
             ->FirstChildElement("data")
             ->FirstChildElement("string");
            element; element = element->NextSiblingElement()) {
            const char *key=element->Attribute("id");
            const char *text=element->Attribute("value");
            if (key && text) {
                configStrings[key] = text;
            }
        }

        // Loop through floats and add to float map
        for (tinyxml2::XMLElement *element = configDoc
             ->FirstChildElement("config")
             ->FirstChildElement("data")
             ->FirstChildElement("float");
            element; element = element->NextSiblingElement()) {
            const char *key=element->Attribute("id");
            float value=element->FloatAttribute("value");
            if (key) {
                configFloats[key] = value;
            }
        }

        // Unload config XML file
        xmlFile->unload();
        return true;
    }

    // Return language related string for printing to gui
    const std::string Application::getString(std::string ID) const {
        // Loop through map and look for string with ID
        std::map<std::string, std::string>::const_iterator it;
        it = textResource.find(ID);
        // If string not found, throw ogre exception...
        // should ignore instead and return default error string?
        if (it == textResource.end()) {
            Ogre::String desc = "Error finding string " + ID;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        // Return found string
        return it->second;
    }

    // Return string related to config
    const std::string Application::getConfigString(std::string ID) const {
        std::map<std::string, std::string>::const_iterator it;
        it = configStrings.find(ID);
        if (it == configStrings.end()) {
            Ogre::String desc = "Error finding config " + ID;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        return it->second;
    }

    // Return float related to config
    const float Application::getConfigFloat(std::string ID) const {
        std::map<std::string, float>::const_iterator it;
        it = configFloats.find(ID);
        if (it == configFloats.end()) {
            Ogre::String desc = "Error finding config " + ID;
            throw(Ogre::Exception(20, desc, "Application"));
        }

        return it->second;
    }

    // Initialize Ogre3d
    bool Application::initOgre(Ogre::String wndTitle,
                               OIS::KeyListener *pKeyListener,
                               OIS::MouseListener *pMouseListener) {

        // Load LogManager for logging
        Ogre::LogManager* logMgr = new Ogre::LogManager();

        // Enable logging for debug build
        Log = logMgr->getSingleton().createLog("SpellByteLogfile.log", true, true, false);
        #ifdef _DEBUG
            Log->setDebugOutputEnabled(true);
        #else
            // May want to enable debug output for release testing
            Log->setDebugOutputEnabled(false);
        #endif

        // Create OgreRoot and pass in plugins config
        #ifdef _DEBUG
            OgreRoot = new Ogre::Root("plugins_d.cfg");
        #else
            OgreRoot = new Ogre::Root("plugins.cfg", "spellbyte.cfg", "spellbyte.log");
        #endif

        // Either restoreConfig or showConfigDialog, make argument dependent?
        //OgreRoot->restoreConfig();
        if (!OgreRoot->showConfigDialog()) {
            return false;
        }

        // Create SpellByte Resource and XML Managers
        SBResourceManager = new SBResFileManager();
        xmlManager = new XMLResourceManager();
        wavManager = new WAVFileManager();

        // Declare default resource file, may want to specificy in SpellByte XML config?
        Ogre::ResourceGroupManager::getSingleton().declareResource("resource.spb", "SBResourceFile");

        // Initialize RenderWindow
        RenderWindow = OgreRoot->initialise(true, wndTitle);

        // Set ViewPort and BackgroundColour
        Viewport = RenderWindow->addViewport(0);
        Viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));

        // Set Camera to, well, no camera
        Viewport->setCamera(0);

        // Make parameter list for OIS
        size_t hWnd = 0;
        OIS::ParamList paramList;
        RenderWindow->getCustomAttribute("WINDOW", &hWnd);

        paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

        // This is left here in case developer doesn't want app to capture mouse,
        // make argument dependent??
        /*paramList.insert(std::make_pair(std::string("w32_mouse"),
            std::string("DISCL_BACKGROUND" )));
        paramList.insert(std::make_pair(std::string("w32_mouse"),
            std::string("DISCL_NONEXCLUSIVE")));*/

        // Setup OIS InputManager
        InputMgr = OIS::InputManager::createInputSystem(paramList);

        // Create Mouse and Keyboard handlers
        Keyboard = static_cast<OIS::Keyboard*>(InputMgr->createInputObject(OIS::OISKeyboard, true));
        Mouse = static_cast<OIS::Mouse*>(InputMgr->createInputObject(OIS::OISMouse, true));

        // Set mouse height and width
        Mouse->getMouseState().height = RenderWindow->getHeight() / 2;
        Mouse->getMouseState().width = RenderWindow->getWidth() / 2;

        // If our keyboard and mouse were created, tell it this is the callback app
        if (pKeyListener == 0)
            Keyboard->setEventCallback(this);
        else
            Keyboard->setEventCallback(pKeyListener);

        if (pMouseListener == 0)
            Mouse->setEventCallback(this);
        else
            Mouse->setEventCallback(pMouseListener);


        // Load resources from resources.cfg
        Ogre::String secName, typeName, archName;
        Ogre::ConfigFile cf;
        cf.load("resources.cfg");

        // Loop through resources.cfg in windows .ini format
        Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
            Ogre::ConfigFile::SettingsMultiMap::iterator it;
            for (it = settings->begin(); it != settings->end(); ++it)
            {
                typeName = it->first;
                archName = it->second;
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
            }
        }

        // Default mipmaps for Ogre3d
        Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

        // Get resource file loaded first
        SBResFilePtr SBFile = SBResourceManager->load("resource.spb", "SpellByteResource");
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        SBFile->load();

        // Start timer and reset it
        Timer = new Ogre::Timer();
        Timer->reset();

        // Set RenderWindow to active
        RenderWindow->setActive(true);

        return true;
    }

    bool Application::keyPressed(const OIS::KeyEvent &keyEventRef) {
        // Following comment is left for reference:
        //Log->logMessage(Ogre::StringConverter::toString(keyEventRef.key));

        // Pass keypress events into CEGUI
        ceguiContext->injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
        ceguiContext->injectChar(keyEventRef.text);

        // Convert key input to SpellByte events
        UserEvent *tmp = NULL;
        if (Keyboard->isKeyDown(OIS::KC_SYSRQ)) {
            RenderWindow->writeContentsToTimestampedFile("SpellByte_Screenshot_", ".jpg");
            return true;
        } else if (keyEventRef.key == OIS::KC_GRAVE) {
            tmp = new UserEvent(UserEvent::TERMINAL);
        } else if(keyEventRef.key == OIS::KC_F12) {
            tmp = new UserEvent(UserEvent::RELOAD);
        } else if (keyEventRef.key == OIS::KC_MINUS) {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_DOWN_ON);
        } else if (keyEventRef.key == OIS::KC_EQUALS) {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_UP_ON);
        } else if (keyEventRef.key == OIS::KC_PGUP) {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_UP_ON);
        } else if (keyEventRef.key == OIS::KC_PGDOWN) {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_DOWN_ON);
        } else if (keyEventRef.key == OIS::KC_C) {
            tmp = new UserEvent(UserEvent::PLAYER_SET_CLIPPING);
        } else if (keyEventRef.key == OIS::KC_ESCAPE) {
            tmp = new UserEvent(UserEvent::ESCAPE);
        } else if (keyEventRef.key == OIS::KC_W) {
            tmp = new UserEvent(UserEvent::PLAYER_FORWARD_ON);
        } else if (keyEventRef.key == OIS::KC_S) {
            tmp = new UserEvent(UserEvent::PLAYER_BACKWARD_ON);
        } else if (keyEventRef.key == OIS::KC_A) {
            tmp = new UserEvent(UserEvent::PLAYER_LEFT_ON);
        } else if (keyEventRef.key == OIS::KC_D) {
            tmp = new UserEvent(UserEvent::PLAYER_RIGHT_ON);
        } else if (keyEventRef.key == OIS::KC_Q) {
            tmp = new UserEvent(UserEvent::PLAYER_UP_ON);
        } else if (keyEventRef.key == OIS::KC_E) {
            tmp = new UserEvent(UserEvent::PLAYER_DOWN_ON);
        } else if (keyEventRef.key == OIS::KC_LSHIFT) {
            tmp = new UserEvent(UserEvent::PLAYER_RUN_ON);
        }

        // It it was a valid action, then add the event
        if (tmp != NULL) {
            CONTROL->addEvent(tmp);
        }

        return true;
    }

    bool Application::keyReleased(const OIS::KeyEvent &keyEventRef) {
        // Pass keyup event to CEGUI
        ceguiContext->injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);

        // Convert keyrelease event into proper SpellByte events
        UserEvent *tmp = NULL;
        if (keyEventRef.key == OIS::KC_SLASH) {
            tmp = new UserEvent(UserEvent::CYCLE_POLYGON_MODE);
        } else if (keyEventRef.key == OIS::KC_PERIOD) {
            tmp = new UserEvent(UserEvent::CYCLE_AFILTER_MODE);
        } else if (keyEventRef.key == OIS::KC_MINUS) {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_DOWN_OFF);
        } else if (keyEventRef.key == OIS::KC_EQUALS) {
            tmp = new UserEvent(UserEvent::AMBIENT_LIGHT_UP_OFF);
        } else if (keyEventRef.key == OIS::KC_PGUP) {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_UP_OFF);
        } else if (keyEventRef.key == OIS::KC_PGDOWN) {
            tmp = new UserEvent(UserEvent::DIR_LIGHT_DOWN_OFF);
        } else if (keyEventRef.key == OIS::KC_W) {
            tmp = new UserEvent(UserEvent::PLAYER_FORWARD_OFF);
        } else if (keyEventRef.key == OIS::KC_S) {
            tmp = new UserEvent(UserEvent::PLAYER_BACKWARD_OFF);
        } else if (keyEventRef.key == OIS::KC_A) {
            tmp = new UserEvent(UserEvent::PLAYER_LEFT_OFF);
        } else if (keyEventRef.key == OIS::KC_D) {
            tmp = new UserEvent(UserEvent::PLAYER_RIGHT_OFF);
        } else if (keyEventRef.key == OIS::KC_Q) {
            tmp = new UserEvent(UserEvent::PLAYER_UP_OFF);
        } else if (keyEventRef.key == OIS::KC_E) {
            tmp = new UserEvent(UserEvent::PLAYER_DOWN_OFF);
        } else if (keyEventRef.key == OIS::KC_LSHIFT) {
            tmp = new UserEvent(UserEvent::PLAYER_RUN_OFF);
        }

        // If have an event, add it to CONTROL
        if (tmp != NULL) {
            CONTROL->addEvent(tmp);
        }
        return true;
    }

    bool Application::mouseMoved(const OIS::MouseEvent &evt) {
        // Following comment is left as reference
        //Log->logMessage(Ogre::StringConverter::toString(evt.state.X.rel));

        // Pass MouseMove event to CEGUI
        ceguiContext->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
        // If mouse wheel moves, let cegui know so it can properly handle event
        if (evt.state.Z.rel) {
            ceguiContext->injectMouseWheelChange(evt.state.Z.rel / 120.0f);
        }

        // Pass mouse move event to CONTROL, should always be done, applications
        // can ignore if desired
        UserEvent* tmp = new UserEvent(UserEvent::MOUSE_MOVE);
        tmp->setWidth(evt.state.width);
        tmp->setHeight(evt.state.height);
        tmp->setAbs(evt.state.X.abs, evt.state.Y.abs);
        tmp->setRel(evt.state.X.rel, evt.state.Y.rel);
        CONTROL->addEvent(tmp);

        return true;
    }

    // Convert OIS MouseButton event to CEGUI MouseButton event
    CEGUI::MouseButton Application::convertButton(OIS::MouseButtonID buttonID) {
        switch (buttonID) {
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

    bool Application::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id) {
        // Pass MousePress event to cegui after conversion
        ceguiContext->injectMouseButtonDown(convertButton(id));

        UserEvent *tmp = NULL;
        switch (id) {
        case OIS::MB_Left: {
                tmp = new UserEvent(UserEvent::MOUSE_LEFT_PRESS);
                break;
            }
        case OIS::MB_Right: {
                tmp = new UserEvent(UserEvent::MOUSE_RIGHT_PRESS);
                break;
            }
        default: {
                tmp = new UserEvent(UserEvent::MOUSE_OTHER_PRESS);
                break;
            }
        }

        // Pass MouseEvents to CONTROL, always done
        tmp->setAbs(evt.state.X.abs, evt.state.Y.abs);
        tmp->setRel(evt.state.X.rel, evt.state.Y.rel);
        CONTROL->addEvent(tmp);

        return true;
    }

    bool Application::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id) {
        // Convert MouseRelease EVENT to CEGUI and pass
        ceguiContext->injectMouseButtonUp(convertButton(id));

        UserEvent *tmp = NULL;
        switch (id) {
        case OIS::MB_Left: {
                tmp = new UserEvent(UserEvent::MOUSE_LEFT_RELEASE);
                break;
            }
        case OIS::MB_Right: {
                tmp = new UserEvent(UserEvent::MOUSE_RIGHT_RELEASE);
                break;
            }
        default: {
                tmp = new UserEvent(UserEvent::MOUSE_OTHER_RELEASE);
                break;
            }
        }

        // Pass MouseRelease Event to CONTROL, always done
        tmp->setAbs(evt.state.X.abs, evt.state.Y.abs);
        tmp->setRel(evt.state.X.rel, evt.state.Y.rel);
        CONTROL->addEvent(tmp);

        return true;
    }

    // Bind APP to LUA, update as necessary
    void Application::bindToLUA() {
        SLB::Class<Application, SLB::Instance::NoCopyNoDestroy >("APP")
            .set("getConfigFloat", &Application::getConfigFloat)
            .set("getConfigString", &Application::getConfigString)
            .set("getString", &Application::getString);

        // Set global access to LUA
        SLB::setGlobal<Application*>(&(*LUAMANAGER->LUA), APP, "app");
    }

    // Change Ogre3d's Anisotropic Filter
    void Application::cycleAFilter() {
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (currentAFilter) {
        case AF_BILINEAR:
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;
        case AF_TRILINEAR:
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;
        case AF_ANISOTROPIC:
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;
        default:
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        Ogre::TextureManager::getSingleton().reloadAll();
    }
}
