#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <map>

#include "stdafx.h"
#include "StateManager.h"
#include "./resources/CubeResourceFileManager.h"
#include "./resources/XMLResourceManager.h"

//#include "SpellByte.h"

namespace SpellByte
{
    class Application : public Ogre::Singleton<Application>, Ogre::FrameListener, OIS::KeyListener, OIS::MouseListener
    {
    public:
        Application();
        virtual ~Application();

        // Initializes instance, loads strings, calculates CPU speed, runs initOgre,etc.
        //virtual bool initInstance();

        // Execute application, this is the entry point to the game
        virtual void execute(void);

        // OIS::KeyListener ( Keyboard Input )
        virtual bool keyPressed( const OIS::KeyEvent &keyEventRef );
        virtual bool keyReleased( const OIS::KeyEvent &keyEventRef );
        // OIS::MouseListener ( Mouse Input )
        virtual bool mouseMoved( const OIS::MouseEvent &evt );
        virtual bool mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id );
        virtual bool mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id );

        // Used to load strings for On Screen Text
        virtual bool loadStrings(Ogre::String language);
        // Get text resource for on screen text
        const std::string getString(std::string ID) const;

        // Get current resolution in string form
        const Ogre::String getResolution() const
        {
            Ogre::String curRes;
            curRes = Ogre::StringConverter::toString(RenderWindow->getWidth()) + " x " + Ogre::StringConverter::toString(RenderWindow->getHeight());
            return curRes;
        }

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

        // Pointer to current SceneManager, do not delete
        // Creator of SceneMgr should delete
        Ogre::SceneManager *SceneMgr;

        // Resource related
        Cube27ResFileManager *c27Manager;
        XMLResourceManager *xmlManager;

        // CEGUI related
        CEGUI::OgreRenderer *ceguiRenderer;
        CEGUI::GUIContext *ceguiContext;

        // Manage GameStates - Why is this public?
        StateManager *GameStateManager;

        // Used to get current frame, that is amount of time that has passed since program started
        double getCurrentFrame() { return Timer->getMillisecondsCPU(); }

        // Convert Mouse Button ID from Ogre to CEGUI
        CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

    protected:
        // Essential main loop, called by Ogre3d
        virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

    private:
        Application(const Application&);
        Application& operator=(const Application&);

        // Initialize Ogre
        virtual bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);

        // Check storage space
        //virtual bool checkStorage(long diskSpaceNeeded);

        // Check available memory
        //virtual bool checkMemory(long physicalRAM, long virtualRAM=0);

        // Calculate CPU speed
        //unsigned int readCPUSpeed();

        // Current language being used throughout application
        std::string language;
        std::string currentDifficulty;
        std::map<std::string, std::string> textResource;
    };
}

#endif // _APPLICATION_H_
