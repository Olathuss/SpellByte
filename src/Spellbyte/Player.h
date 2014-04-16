#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "define.h"
#include "stdafx.h"
#include "Subscriber.h"
#include "CollisionTools.h"
#include "coldet/coldet.h"
#include "actor/BaseActor.h"
#include "Actor.h"

namespace SpellByte
{
    class World;
    class Player : public BaseActor, public Subscriber
    {
    friend World;
    public:
        Player();
        ~Player();

        bool init(Ogre::SceneManager *sceneManager, Ogre::Camera *Camera, World *world);
        void setRotation(int X, int Y);

        virtual void update(const Ogre::FrameEvent &evt);
        virtual bool handleMessage(const Telegram &msg);
        void handleEvent(int);

        Ogre::SceneNode *getCameraNode();
        void setCollisionHandler(MOC::CollisionTools *ct);

        Ogre::String getDebugString();
        const Ogre::Vector3 getPosition();

        void attachCamera(Ogre::Camera *camera);
        void detachCamera(Ogre::Camera *camera);

        virtual std::string handleConsoleCmd(std::queue<std::string> cmdQueue);

    private:
        // For messaging
        int PlayerID;

        Ogre::SceneManager *sceneMgr;
        void bindToLUA();

        // Method to mark targets
        void getTarget();
        Actor *Target;
        int TargetedActor;
        Ogre::SceneNode *targetNode;
        void playerFeed();
        void feedSuccess();

        enum PLAYER_ACTIONS {
            PLAYER_FORWARD = 1 << 0,
            PLAYER_BACKWARD = 1 << 1,
            PLAYER_LEFT = 1 << 2,
            PLAYER_RIGHT = 1 << 3,
            PLAYER_UP = 1 << 4,
            PLAYER_DOWN = 1 << 5,
            PLAYER_FEED = 1 << 6
        };

        void enableAction(int action) {
            PlayerAction |= action;
        };

        void disableAction(int action) {
            PlayerAction &= ~(action);
        };

        int PlayerAction;

        // Collision Handling
        MOC::CollisionTools *collisionHandler;
        bool enabledCollision;
        float collisionRadius;
        int collisionMask;
        Ogre::SceneNode *collisionNode;
        CollisionModel3D *collisionModel;
        void updateCollisionModel();

        // UI
        CEGUI::Window *BloodScreen;
        bool showBlood;
        Ogre::Real bloodTime;
        Ogre::Real bloodCount;
        bool bloodUp;
        void displayBlood(const Ogre::FrameEvent &evt);

        // Sad physics
        Ogre::Real gravity;
        Ogre::Real accelY;
        Ogre::Real jumpVelocity;

        // For FPS controls
        Ogre::Vector3 translateVector;
        Ogre::Radian rotX, rotY;
        Ogre::SceneNode *playerNode;
        Ogre::SceneNode *cameraYawNode, *cameraPitchNode, *cameraRollNode;
        Ogre::RaySceneQuery *RSQ;
        float moveScale;
        Ogre::Real playerHeight;
        Ogre::Real moveSpeed;
        void moveCamera();
    };
}

#endif // _PLAYER_H_
