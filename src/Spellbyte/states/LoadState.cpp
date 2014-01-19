/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * LoadState Class
 *
 * This is load state class
 *
 */

 #include "LoadState.h"
 #include "../SpellByte.h"

 namespace SpellByte
 {
     LoadState::LoadState()
     {
         // Create LoadState, note this does not execute the load state
         Quit = false;
         FrameEvent = Ogre::FrameEvent();
     }

     void LoadState::enter()
     {
        // Enter into Load state, output logmessage to inform us of this
        APP->Log->logMessage("LoadState: enter");

        // Create scene manager derived from ogre root, this is a ST_GENERIC type with name "LoadSceneMgr"
        SceneMgr = APP->OgreRoot->createSceneManager(Ogre::ST_GENERIC, "LoadSceneMgr");

        // Set light for Load scene, not sure if this is necessary or if the values are correct
        // but here more as placement
        SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

        // Create camera and set its position and look at
        Camera = SceneMgr->createCamera("LoadCam");
        Camera->setPosition(Ogre::Vector3(0, 25, -50));
        Camera->lookAt(Ogre::Vector3(0, 0, 0));
        Camera->setNearClipDistance(1);

        // Set aspect ratio of camera
        Camera->setAspectRatio(Ogre::Real(APP->Viewport->getActualWidth()) /
                               Ogre::Real(APP->Viewport->getActualHeight()));

        // Tell the application this camera is going to be the one for the viewport
        APP->Viewport->setCamera(Camera);

        // Create the scene as is necessary
        createScene();
     }

     void LoadState::createScene()
     {
         // Empty
     }

    void LoadState::exit()
    {
        // Exit Load state
        APP->Log->logMessage("LoadState: exit");

        // Destroy the camera so next state can create its own camera
        SceneMgr->destroyCamera(Camera);

        // Destroy scene manager
        if(SceneMgr)
            APP->OgreRoot->destroySceneManager(SceneMgr);
    }

    void LoadState::handleEvents()
    {

    }

    void LoadState::update(const Ogre::FrameEvent &evt)
    {
        if(Quit == true)
        {
            Parent->shutDown();
            return;
        }
    }

    void LoadState::resume()
    {

    }
 }


