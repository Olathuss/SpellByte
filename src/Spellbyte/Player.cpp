#include "Player.h"

namespace SpellByte
{
    Player::Player()
    {
        translateVector = Ogre::Vector3::ZERO;
        PlayerAction = 0;
        moveScale = 0.0f;
        playerHeight = APP->getConfigFloat("height");
        moveSpeed = APP->getConfigFloat("speed");
    }

    Player::~Player()
    {

    }

    bool Player::init(Ogre::SceneManager *sceneManager, Ogre::Camera *Camera)
    {
        sceneMgr = sceneManager;

        cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        Camera->setNearClipDistance(0.1);
        Camera->setFarClipDistance(50000);

        cameraYawNode = cameraNode->createChildSceneNode();

        cameraPitchNode = cameraYawNode->createChildSceneNode();

        cameraRollNode = cameraPitchNode->createChildSceneNode();
        cameraRollNode->attachObject(Camera);
        cameraNode->setPosition(0, 500, 0);
    }

    Ogre::SceneNode *Player::getCameraNode()
    {
        return cameraNode;
    }

    const Ogre::Vector3 Player::getPosition()
    {
        return cameraNode->getPosition();
    }

    Ogre::String Player::getDebugString()
    {
        Ogre::Vector3 xyz = cameraNode->getPosition();
        Ogre::String txt = "X: " + Ogre::StringConverter::toString(xyz.x) +
                        "\nY: " + Ogre::StringConverter::toString(xyz.y) +
                        "\nZ: " + Ogre::StringConverter::toString(xyz.z);
        return txt;
    }

    void Player::setCollisionHanlder(MOC::CollisionTools *ct)
    {
        collisionHandler = ct;
        collisionHandler->setHeightAdjust(playerHeight);
        collisionHandler->calculateY(cameraNode);
    }

    void Player::update(const Ogre::FrameEvent &evt)
    {
        moveScale = moveSpeed * evt.timeSinceLastFrame;
        moveCamera();
        rotX = rotY = Ogre::Radian(0);
    }

    void Player::handleEvent(int event)
    {
        switch(event)
        {
        case UserEvent::PLAYER_FORWARD_ON:
            enableAction(PLAYER_FORWARD);
            break;
        case UserEvent::PLAYER_FORWARD_OFF:
            disableAction(PLAYER_FORWARD);
            break;
        case UserEvent::PLAYER_BACKWARD_ON:
            enableAction(PLAYER_BACKWARD);
            break;
        case UserEvent::PLAYER_BACKWARD_OFF:
            disableAction(PLAYER_BACKWARD);
            break;
        case UserEvent::PLAYER_LEFT_ON:
            enableAction(PLAYER_LEFT);
            break;
        case UserEvent::PLAYER_LEFT_OFF:
            disableAction(PLAYER_LEFT);
            break;
        case UserEvent::PLAYER_RIGHT_ON:
            enableAction(PLAYER_RIGHT);
            break;
        case UserEvent::PLAYER_RIGHT_OFF:
            disableAction(PLAYER_RIGHT);
            break;
        case UserEvent::PLAYER_UP_ON:
            enableAction(PLAYER_UP);
            break;
        case UserEvent::PLAYER_UP_OFF:
            disableAction(PLAYER_UP);
            break;
        case UserEvent::PLAYER_DOWN_ON:
            enableAction(PLAYER_DOWN);
            break;
        case UserEvent::PLAYER_DOWN_OFF:
            disableAction(PLAYER_DOWN);
            break;
        }
    }

    void Player::setRotation(int X, int Y)
    {
        rotX = Ogre::Degree(-X * 0.13);
        rotY = Ogre::Degree(-Y * 0.13);
    }

    void Player::moveCamera()
    {
        translateVector = Ogre::Vector3::ZERO;
        if(PlayerAction & PLAYER_UP)
        {
            cameraNode->setPosition(cameraNode->getPosition() + Ogre::Vector3(0, 5, 0));
        }
        if(PlayerAction & PLAYER_DOWN)
        {
            cameraNode->setPosition(cameraNode->getPosition() - Ogre::Vector3(0, 5, 0));
        }
        if(PlayerAction & PLAYER_FORWARD)
        {
            translateVector.z = -moveScale;
        }
        if(PlayerAction & PLAYER_BACKWARD)
        {
            translateVector.z = moveScale;
        }
        if(PlayerAction & PLAYER_LEFT)
        {
            translateVector.x = -moveScale;
        }
        if(PlayerAction & PLAYER_RIGHT)
        {
            translateVector.x = moveScale;
        }

        Ogre::Vector3 oldPos = cameraNode->getPosition();

        Ogre::Real pitchAngle;
        Ogre::Real pitchAngleSign;

        cameraYawNode->yaw(rotX);
        cameraPitchNode->pitch(rotY);

        cameraNode->translate(cameraYawNode->getOrientation() *
                            cameraPitchNode->getOrientation() *
                            translateVector, Ogre::SceneNode::TS_LOCAL);

        pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(cameraPitchNode->getOrientation().w)).valueDegrees());

        pitchAngleSign = cameraPitchNode->getOrientation().x;

        if(pitchAngle > 90.0f)
        {
            if(pitchAngleSign > 0)
            {
                cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                                 Ogre::Math::Sqrt(0.5f), 0, 0));
            }
            else if(pitchAngleSign < 0)
            {
                cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                                 -Ogre::Math::Sqrt(0.5f), 0, 0));
            }
        }

        if(APP->collisionEnabled())
        {
            collisionHandler->calculateY(cameraNode,true,true,2.0f,1);
            if (collisionHandler->collidesWithEntity(oldPos, cameraNode->getPosition(), 5.0f, -1.0f, 1))
            {
                cameraNode->setPosition(oldPos);
            }
        }
    }
}