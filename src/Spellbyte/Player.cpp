#include <cctype>
#include "Player.h"
#include "console/console_communicator.h"
#include "SLB.hpp"
#include "console/LuaManager.h"
#include "World.h"
#include "Object.h"

namespace SpellByte
{
    Player::Player() : Subscriber()
    {
        translateVector = Ogre::Vector3::ZERO;
        PlayerAction = 0;
        moveScale = 0.0f;
        playerHeight = APP->getConfigFloat("height");
        moveSpeed = APP->getConfigFloat("speed");
        collisionRadius = APP->getConfigFloat("player_radius");
        collisionMask = 0;
        COMM->registerSubscriber("player", this);
        collisionNode = NULL;
    }

    Player::~Player()
    {

    }

    void Player::bindToLUA()
    {
        SLB::Class<Player>("SpellByte::Player")
            .constructor()
            .property("speed", &Player::moveSpeed)
            .property("height", &Player::playerHeight);

        SLB::setGlobal<Player*>(&(*LUAMANAGER->LUA), this, "player");
    }

    bool Player::init(Ogre::SceneManager *sceneManager, Ogre::Camera *Camera, World *world)
    {
        sceneMgr = sceneManager;
        GameWorld = world;
        RSQ = sceneMgr->createRayQuery(Ogre::Ray());

        cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        Camera->setNearClipDistance(0.1);
        Camera->setFarClipDistance(50000);

        cameraYawNode = cameraNode->createChildSceneNode();

        cameraPitchNode = cameraYawNode->createChildSceneNode();

        cameraRollNode = cameraPitchNode->createChildSceneNode();
        cameraRollNode->attachObject(Camera);
        cameraNode->setPosition(Ogre::Vector3::ZERO);
        cameraNode->setPosition(APP->getConfigFloat("playerstartx"), APP->getConfigFloat("playerstarty"), APP->getConfigFloat("playerstartz"));

        //enabledCollision = false;
        collisionMask |= World::COLLISION_MASK::STATIC;
        collisionNode = sceneMgr->getRootSceneNode()->createChildSceneNode("PlayCollisionNode");
        Ogre::Entity *collideEntity = sceneMgr->createEntity("PlayerEntity", "player.mesh", "General");
        collisionModel = newCollisionModel3D(false);
        addTrianglesToColdet(collideEntity, collisionModel);
        collisionModel->finalize();
        collisionNode->attachObject(collideEntity);
        collisionNode->setPosition(cameraNode->getPosition());
        updateCollisionModel();

        bindToLUA();
        return true;
    }

    void Player::detachCamera(Ogre::Camera *camera)
    {
        cameraRollNode->detachObject(camera);
    }

    void Player::attachCamera(Ogre::Camera *camera)
    {
        cameraRollNode->attachObject(camera);
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
        Ogre::Vector3 xyz = cameraNode->_getDerivedPosition();
        Ogre::String txt = "X: " + Ogre::StringConverter::toString(xyz.x) +
                        "\nY: " + Ogre::StringConverter::toString(xyz.y) +
                        "\nZ: " + Ogre::StringConverter::toString(xyz.z);
        return txt;
    }

    std::string Player::handleConsoleCmd(std::queue<std::string> cmdQueue) {
        std::string returnString;
        std::string nextCmd = cmdQueue.front();
        cmdQueue.pop();
        if(nextCmd == "debug")
        {
            return getDebugString();
        }
        else if(nextCmd == "move")
        {
            float x, y, z;
            Ogre::Vector3 pos = cameraNode->getPosition();
            x = pos.x;
            y = pos.y;
            z = pos.z;
            returnString = "Player move:";
            while(!cmdQueue.empty())
            {
                std::string next = cmdQueue.front();
                cmdQueue.pop();
                if(next == "x")
                {
                    std::string value = cmdQueue.front().c_str();
                    cmdQueue.pop();
                    x = std::atof(value.c_str());
                    returnString += "\nset x to " + value;
                }
                else if(next == "y")
                {
                    std::string value = cmdQueue.front().c_str();
                    cmdQueue.pop();
                    y = std::atof(value.c_str());
                    returnString += "\nset y to " + value;
                }
                else if(next == "z")
                {
                    std::string value = cmdQueue.front().c_str();
                    cmdQueue.pop();
                    z = std::atof(value.c_str());
                    returnString += "\nset z to " + value;
                }
                else
                {
                    return "Error, invalid argument; expected: x, y, or z";
                }
            }
            cameraNode->setPosition(x, y, z);
        }
        else
        {
            return "Player: invalid command";
        }
        return returnString;
    }

    void Player::setCollisionHandler(MOC::CollisionTools *ct)
    {
        collisionHandler = ct;
        collisionHandler->setHeightAdjust(playerHeight);
        if(enabledCollision)
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
        case UserEvent::PLAYER_RUN_ON:
            moveSpeed *= 5;
            break;
        case UserEvent::PLAYER_RUN_OFF:
            moveSpeed /= 5;
            break;
        case UserEvent::PLAYER_SET_CLIPPING:
            //enabledCollision = (enabledCollision == false) ? true : false;
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
        bool move = false;
        if(PlayerAction & PLAYER_UP)
        {
            cameraNode->setPosition(cameraNode->getPosition() + Ogre::Vector3(0, moveScale, 0));
            move = true;
        }
        if(PlayerAction & PLAYER_DOWN)
        {
            cameraNode->setPosition(cameraNode->getPosition() - Ogre::Vector3(0, moveScale, 0));
            move = true;
        }
        if(PlayerAction & PLAYER_FORWARD)
        {
            translateVector.z = -moveScale;
            move = true;
        }
        if(PlayerAction & PLAYER_BACKWARD)
        {
            translateVector.z = moveScale;
            move = true;
        }
        if(PlayerAction & PLAYER_LEFT)
        {
            translateVector.x = -moveScale;
            move = true;
        }
        if(PlayerAction & PLAYER_RIGHT)
        {
            translateVector.x = moveScale;
            move = true;
        }

        Ogre::Vector3 oldPos = cameraNode->getPosition();

        Ogre::Real pitchAngle;
        Ogre::Real pitchAngleSign;

        cameraYawNode->yaw(rotX);
        cameraPitchNode->pitch(rotY);

        if(move) {
            cameraNode->translate(cameraYawNode->getOrientation() *
                                cameraPitchNode->getOrientation() *
                                translateVector, Ogre::SceneNode::TS_LOCAL);
        }

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

        if(!move) {
            return;
        }

        /*if(enabledCollision)
        {
            collisionHandler->calculateY(cameraNode,true,true,1.5f,1);
            if (collisionHandler->collidesWithEntity(oldPos, cameraNode->getPosition(), collisionRadius, -1.0f, -1))
            {
                cameraNode->setPosition(oldPos);
            }
            else if(cameraNode->getPosition().y - oldPos.y > playerHeight * .4)
            {
                cameraNode->setPosition(oldPos);
            }
        }*/
        float rayHeightLevel = -1.0f;
        Vector3 fromPoint = oldPos;
        Vector3 toPoint = cameraNode->getPosition();
        collisionNode->setPosition(toPoint.x, toPoint.y, toPoint.z);
        updateCollisionModel();
        Vector3 fromPointAdj(fromPoint.x, fromPoint.y + rayHeightLevel, fromPoint.z);
        Vector3 toPointAdj(toPoint.x, toPoint.y + rayHeightLevel, toPoint.z);
        Vector3 normal = toPointAdj - fromPointAdj;
        float distToDest = normal.normalise();

        Ogre::Ray ray(fromPointAdj, normal);
        RSQ->setRay(ray);
        RSQ->setSortByDistance(true);
        Ogre::RaySceneQueryResult &result = RSQ->execute();
        Ogre::RaySceneQueryResult::iterator itr;
        for(itr = result.begin(); itr != result.end(); itr++) {
            if ((itr->movable != NULL)  &&
                (itr->movable->getMovableType().compare("Entity") == 0)) {
                Ogre::Entity *pentity = static_cast<Ogre::Entity*>(itr->movable);
                if(pentity->getName() == "collisionEntity")
                    continue;
                SpellByte::Object* obj;
                try {
                     obj = Ogre::any_cast<SpellByte::Object*>(pentity->getParentSceneNode()->getUserAny());
                } catch(Ogre::Exception &e) {
                    continue;
                }
                if(collisionModel->collision(obj->coldetModel)) {
                    move = false;
                    break;
                    //cameraNode->setPosition(oldPos);
                    //Ogre::Entity *ent = sceneMgr->createEntity("collisionEntity", "player.mesh", "General");
                    //collisionNode->attachObject(ent);
                    float collision[3];
                    obj->coldetModel->getCollisionPoint(collision, false);
                    Ogre::Vector3 coll = Ogre::Vector3(collision[0], collision[1], collision[2]);
                    //Ogre::SceneNode collideNode = sceneMgr->getRootSceneNode()->createChildSceneNode("CollideNode");
                    //Ogre::Entity *collideEntity = sceneMgr->createEntity("CollideEntity", "player.mesh", "General");
                    //collideNode->attachObject(collideEntity);
                    collisionNode->setPosition(coll);
                    std::cout << "Possible collision with: " << obj->getName() << std::endl;
                    std::cout << "X: " << collision[0] << " Y: " << collision[1] << " Z: " << collision[2] << std::endl;
                    std::cout << "X: " << oldPos.x << " Y: " << oldPos.y << " Z: " << oldPos.z << std::endl;
                    std::cout << "Distance to Dest: " << oldPos.distance(toPoint) <<
                                "Distance to Collision: " << oldPos.distance(coll) << std::endl;
                    if(oldPos.distance(coll) <= distToDest) {
                        std::cout << "TRUE TRUE TRUE" << std::endl;
                        cameraNode->setPosition(oldPos);
                        //collisionNode->setPosition(oldPos);
                        break;
                    }
                }
                /*Ogre::Vector3 tmp = ray.getOrigin();
                float origin[3] = { tmp[0], tmp[1], tmp[2] };
                tmp = ray.getDirection();
                float direction[3] = { tmp[0], tmp[1], tmp[2] };
                if(obj->coldetModel->rayCollision(origin, direction)) {
                    std::cout << "Collision Name: " << obj->getName() << std::endl;
                    float collision[3];
                    obj->coldetModel->getCollisionPoint(collision, false);
                    std::cout << "X: " << collision[0] << " Y: " << collision[1] << " Z: " << collision[2] << std::endl;
                    std::cout << "X: " << oldPos.x << " Y: " << oldPos.y << " Z: " << oldPos.z << std::endl;
                    Ogre::Vector3 coll = Ogre::Vector3(collision[0], collision[1], collision[2]);
                    std::cout << "Distance to Dest: " << oldPos.distance(toPoint) <<
                                "Distance to Collision: " << oldPos.distance(coll) << std::endl;
                    if(oldPos.distance(coll) - 3.0f <= distToDest) {
                        std::cout << "TRUE TRUE TRUE" << std::endl;
                        cameraNode->setPosition(oldPos);
                        break;
                    }
                }*/
            }
        }
        //collisionHandler->collidesWithEntity(oldPos, cameraNode->getPosition(), collisionRadius, -1.0f, -1);
    }

    void Player::updateCollisionModel() {
        Ogre::Matrix4 matrix4 = collisionNode->_getFullTransform().transpose();

        float matrix[16];

        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                matrix[j + (i * 4)] = matrix4[i][j];
            }
        }

        collisionModel->setTransform(matrix);
    }
}
