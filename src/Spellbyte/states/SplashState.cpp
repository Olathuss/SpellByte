/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * SplashState Class
 *
 * This is splash state class
 *
 */

 #include <time.h>
 #include "SplashState.h"
 #include "../SpellByte.h"

 namespace SpellByte
 {
     SplashState::SplashState()
     {
         SplashScreen = NULL;
         // Create SplashState, note this does not execute the splash state
         CurrentSplash = 0;
         SplashScreenList.push_back("KnightForgedSplash");
         SplashScreenList.push_back("Ogre3DSplash");
         SplashScreenList.push_back("KickStarterSplash");
     }

     SplashState::~SplashState()
     {
        if(SplashScreen)
        {
            delete SplashScreen;
            SplashScreen = 0;
        }
     }

     void SplashState::enter()
     {
        // Enter into Splash state, output logmessage to inform us of this
        APP->Log->logMessage("SplashState: enter");

        // Create scene manager derived from ogre root, this is a ST_GENERIC type with name "SplashSceneMgr"
        SceneMgr = APP->OgreRoot->createSceneManager(Ogre::ST_GENERIC, "SplashSceneMgr");

        // Set light for Splash scene, not sure if this is necessary or if the values are correct
        // but here more as placement
        SceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

        // Create camera and set its position and look at
        Camera = SceneMgr->createCamera("SplashCam");
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

     void SplashState::createScene()
     {
         SplashStart = clock();

         int ScreenX = APP->Viewport->getActualHeight();
         int ScreenY = APP->Viewport->getActualWidth();
         double XRatio = ScreenX/1600.0;
         double YRatio = ScreenY/900.0;

         SplashScreen = new Ogre::Rectangle2D(true);
         SplashScreen->setCorners(-2.08*XRatio,YRatio/1.5,2.08*XRatio,-1*YRatio/1.5);
         SplashScreen->setMaterial(SplashScreenList[CurrentSplash]);
         SplashScreen->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

         Ogre::AxisAlignedBox box;
         box.setInfinite();
         SplashScreen->setBoundingBox(box);

         Ogre::SceneNode * node = SceneMgr->getRootSceneNode()->createChildSceneNode("SplashScreen");
         node->attachObject(SplashScreen);
     }

    void SplashState::exit()
    {
        // Exit Splash state
        APP->Log->logMessage("SplashState: exit");

        if(SplashScreen)
        {
            delete SplashScreen;
            SplashScreen = 0;
        }

        // Destroy the camera so next state can create its own camera
        SceneMgr->destroyCamera(Camera);

        // Destroy scene manager
        if(SceneMgr)
            APP->OgreRoot->destroySceneManager(SceneMgr);
    }

    void SplashState::handleEvents()
    {
        while(CONTROL->hasEvent())
        {
            const UserEvent *tmp = CONTROL->getEvent();
            if(tmp->getType() == UserEvent::ESCAPE)
            {
                CurrentSplash++;
                if(CurrentSplash >= SplashScreenList.size())
                    changeState(findByName("MenuState"));
                else
                {
                    SplashStart = clock();
                    SplashScreen->setMaterial(SplashScreenList[CurrentSplash]);
                }
            }
        }
    }

    void SplashState::update(const Ogre::FrameEvent &evt)
    {
        double CurrentTime = clock() - SplashStart;
        double Color = 0;
        if(CurrentTime < 3000)
            Color = CurrentTime;
        else if(CurrentTime < 4000)
            Color = 3000;
        else if(CurrentTime <10000)
            Color = 10000-CurrentTime;
        else
        {
            SplashStart = clock();
            CurrentSplash ++;
            if(CurrentSplash >= SplashScreenList.size())
            {
                changeState(findByName("MenuState"));
                return;
            }
            else
                SplashScreen->setMaterial(SplashScreenList[CurrentSplash]);
        }

        Color /= 3000.0;

        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(SplashScreenList[CurrentSplash],"General");
        mat->getTechnique(0)->getPass(0)->setAmbient(Color,Color,Color);
        mat->getTechnique(0)->getPass(0)->setSelfIllumination(Color,Color,Color);

    }

    void SplashState::resume()
    {

    }
 }

