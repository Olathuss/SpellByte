#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "define.h"
#include "stdafx.h"
#include "Subscriber.h"
#include "CollisionTools.h"

namespace SpellByte
{
    class World;
    class Player : public Subscriber
    {
    friend World;
    public:
        Player();
        ~Player();

        bool init(Ogre::SceneManager *sceneManager, Ogre::Camera *Camera, World *world);
        void setRotation(int X, int Y);

        void update(const Ogre::FrameEvent &evt);
        void handleEvent(int);

        Ogre::SceneNode *getCameraNode();
        void setCollisionHanlder(MOC::CollisionTools *ct);

        Ogre::String getDebugString();
        const Ogre::Vector3 getPosition();

        void attachCamera(Ogre::Camera *camera);
        void detachCamera(Ogre::Camera *camera);

        virtual std::string handleConsoleCmd(std::queue<std::string> cmdQueue);

    private:
        Ogre::SceneManager *sceneMgr;
        void bindToLUA();

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
        bool enabledCollision;

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

        float collisionRadius;

        World *GameWorld;
    };
}

#endif // _PLAYER_H_
