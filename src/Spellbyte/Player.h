#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "define.h"
#include "stdafx.h"
#include "CollisionTools.h"

namespace SpellByte
{
    class Player
    {
    public:
        Player();
        ~Player();

        bool init(Ogre::SceneManager *sceneManager, Ogre::Camera *Camera);
        void setRotation(int X, int Y);

        void update(const Ogre::FrameEvent &evt);
        void handleEvent(int);

        Ogre::SceneNode *getCameraNode();
        void setCollisionHanlder(MOC::CollisionTools *ct);

        Ogre::String getDebugString();
        const Ogre::Vector3 getPosition();

    private:
        Ogre::SceneManager *sceneMgr;

        enum PLAYER_ACTIONS
        {
            PLAYER_FORWARD = 1 << 0,
            PLAYER_BACKWARD = 1 << 1,
            PLAYER_LEFT = 1 << 2,
            PLAYER_RIGHT = 1 << 3,
            PLAYER_UP = 1 << 4,
            PLAYER_DOWN = 1 << 5
        };

        void enableAction(int action)
        {
            PlayerAction |= action;
        };
        void disableAction(int action)
        {
            PlayerAction &= ~(action);
        };

        int PlayerAction;

        // Collision Handling
        MOC::CollisionTools *collisionHandler;

        // For FPS controls
        Ogre::Vector3 translateVector;
        Ogre::Radian rotX, rotY;
        Ogre::SceneNode *playerNode;
        Ogre::SceneNode *cameraNode, *cameraYawNode, *cameraPitchNode, *cameraRollNode;
        Ogre::RaySceneQuery *RSQ;
        float moveScale;
        Ogre::Real playerHeight;
        Ogre::Real moveSpeed;
        void moveCamera();
    };
}

#endif // _PLAYER_H_
