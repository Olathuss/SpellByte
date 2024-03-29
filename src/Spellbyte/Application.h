#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <map>

//#define AUDIO

#ifdef AUDIO
#include <AL/al.h>
#endif

#include "stdafx.h"
#include "StateManager.h"
#include "./resources/SpellByteResourceFileManager.h"
#include "./resources/XMLResourceManager.h"
#include "./resources/LUAResourceManager.h"

#ifdef AUDIO
#include "./resources/WAVFileManager.h"
#endif

//! Application Singleton Class
/*!
    This is the primary class used as an entry point
    into the SpellByte engine.  It is a singleton and
    controls all of the Ogre and OIS controllers:
    Winow, SceneManager, Keyboard, Mouse, etc.

    It loads configuration variables and maintains them.
    It's purpose is to be the line between application and OS,
    although external libraries Ogre/CEGUI/etc. do some of this
    as well.
*/

namespace SpellByte {
    class World;
    class Application : public Ogre::Singleton<Application>, Ogre::FrameListener, OIS::KeyListener, OIS::MouseListener {
    public:
        // Determines Anisotropic Filtering for changing this setting
        //! Anisotropic filtering enum
        enum AFILTER {
            AF_TRILINEAR = 0,       /*!< Trilinear      */
            AF_ANISOTROPIC,         /*!< Anisotropic    */
            AF_NONE,                /*!< None           */
            AF_BILINEAR             /*!< Bilinear       */
        };

        //! Constructor, should NEVER be called except by Ogre
        Application();
        //! Destructor
        virtual ~Application();

        // Execute application, this is the entry point to the game
        // And begins main loop
        //! Method used as entry point, begins initializations and main loop
        virtual void execute(void);

        // OIS::KeyListener ( Keyboard Input )
        //! Handles key presses from OIS
        /*!
            Handles key presses from OIS and converts into UserEvent for State polling.
            Also passes key press off to CEGUI.  The purpose is to prevent game objects
            from handling key presses themselves and instead handling the event,
            this allows key input to be centralized and easier to handle.

            keyPressed means the key has been pressed, and can be assumed to be held
            down until keyRelease is called with the same key.
        */
        virtual bool keyPressed( const OIS::KeyEvent &keyEventRef );
        //! Handles key releases from OIS
        /*!
            Handles key releases from OIS and converts into UserEvent for State polling.
            Also passes key release off to CEGUI.  The purpose is to prevent game objects
            from handling key releases themselves and instead handling the event,
            this allows key input to be centralized and easier to handle.

            keyReleased means the key has been releases and no longer held
            down by the user.
        */
        virtual bool keyReleased( const OIS::KeyEvent &keyEventRef );
        // OIS::MouseListener ( Mouse Input )
        virtual bool mouseMoved( const OIS::MouseEvent &evt );
        virtual bool mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id );
        virtual bool mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id );

        // Set world pointer for objects to gain reference
        void attachWorld(World* nWorld) {
            worldPtr = nWorld;
        }
        // Remove world pointer
        void detachWorld() {
            worldPtr = NULL;
        }
        // Return world pointer
        World* getWorldPtr() {
            return worldPtr;
        }

        // Get text resource for on screen text, allows use of external text for language translations
        const std::string getString(std::string ID) const;

        // Configurations
        // Get config string setting
        const std::string getConfigString(std::string ID) const;
        // Get config float setting
        const float getConfigFloat(std::string ID) const;

        // Get current resolution in string form, used for options menu
        const Ogre::String getResolution() const {
            Ogre::String curRes;
            curRes = Ogre::StringConverter::toString(RenderWindow->getWidth()) + " x " + Ogre::StringConverter::toString(RenderWindow->getHeight());
            return curRes;
        }

        // All of these are public pointers so that other parts of game may access them freely
        // BE CAREFUL ON ACCESSING THEM

        // Ogre Objects
        Ogre::Root                     *OgreRoot;
        Ogre::RenderWindow             *RenderWindow;
        Ogre::Viewport                 *Viewport;

        Ogre::Log                      *Log;

        Ogre::Timer                    *Timer;

        // OIS Input Devices
        OIS::InputManager              *InputMgr;
        OIS::Keyboard                  *Keyboard;
        OIS::Mouse                     *Mouse;

        // Single Ogre::SceneManager
        Ogre::SceneManager *SceneMgr;

        // Resource related
        SBResFileManager *SBResourceManager;
        XMLResourceManager *xmlManager;
        LUAResourceManager *luaManager;

#ifdef AUDIO
        WAVFileManager *wavManager;
#endif

        // CEGUI related
        CEGUI::OgreRenderer *ceguiRenderer;
        CEGUI::GUIContext *ceguiContext;

        // Manage GameStates - Why is this public?
        StateManager *GameStateManager;

        // Used to get current frame, that is amount of time that has passed since program started
        double getCurrentFrame() { return Timer->getMillisecondsCPU(); }

        // Convert Mouse Button ID from Ogre to CEGUI
        CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

        // Cycle Anisotropic Filter
        void cycleAFilter();

    protected:
        // Essential main loop, called by Ogre3d
        virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

    private:
        Application(const Application&);
        Application& operator=(const Application&);

        // Initialize Ogre
        virtual bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);

        // Used to load strings for On Screen Text
        virtual bool loadStrings(Ogre::String language);
        virtual bool loadConfig();

        // To bind Application Class to LUA, also sets global App accessor within LUA
        void bindToLUA();

        // Check storage space
        //virtual bool checkStorage(long diskSpaceNeeded);

        // Check available memory
        //virtual bool checkMemory(long physicalRAM, long virtualRAM=0);

        // Calculate CPU speed
        //unsigned int readCPUSpeed();

        // Current language being used throughout application
        std::string language;
        std::map<std::string, std::string> configStrings;
        std::map<std::string, float> configFloats;
        std::string currentDifficulty;
        std::map<std::string, std::string> textResource;
        World* worldPtr;
        AFILTER currentAFilter;
        int loopCount;
    };
}

#endif // _APPLICATION_H_

