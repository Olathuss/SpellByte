#include <cctype>
#include "Player.h"
#include "console/console_communicator.h"
#include "SLB.hpp"
#include "console/LuaManager.h"
#include "World.h"
#include "Object.h"
#include "ActorManager.h"
#include "utilities/Messages.h"
#include "utilities/MessageDispatcher.h"

namespace SpellByte {
    Player::Player() : Subscriber() {
        PlayerID = -1;
        Courier->setPlayer(PlayerID, this);
        translateVector = Ogre::Vector3::ZERO;
        PlayerAction = 0;
        moveScale = 0.0f;
        playerHeight = APP->getConfigFloat("height");
        moveSpeed = APP->getConfigFloat("speed");
        collisionRadius = APP->getConfigFloat("player_radius");
        collisionMask = 0;
        COMM->registerSubscriber("player", this);
        collisionNode = NULL;
        Target = nullptr;
        BloodScreen = nullptr;
        SleepScreen = nullptr;
        TargetedActor = -1;
    }

    Player::~Player() {
        //BloodScreen->
    }

    void Player::bindToLUA() {
        SLB::Class<Player>("SpellByte::Player")
            .constructor()
            .property("speed", &Player::moveSpeed)
            .property("height", &Player::playerHeight)
            .property("collision", &Player::enabledCollision);

        SLB::setGlobal<Player*>(&(*LUAMANAGER->LUA), this, "player");
    }

    bool Player::init(Ogre::SceneManager *sceneManager, Ogre::Camera *Camera, World *world) {
        sceneMgr = sceneManager;
        WorldPtr = world;
        RSQ = sceneMgr->createRayQuery(Ogre::Ray());

        ActorNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        Camera->setNearClipDistance(0.1);
        Camera->setFarClipDistance(APP->getConfigFloat("draw_distance"));

        cameraYawNode = ActorNode->createChildSceneNode();

        cameraPitchNode = cameraYawNode->createChildSceneNode();

        cameraRollNode = cameraPitchNode->createChildSceneNode();
        cameraRollNode->attachObject(Camera);
        ActorNode->setPosition(Ogre::Vector3::ZERO);
        ActorNode->setPosition(APP->getConfigFloat("playerstartx"), APP->getConfigFloat("playerstarty"), APP->getConfigFloat("playerstartz"));
        ActorNode->setDirection(Ogre::Vector3::UNIT_Z);

        enabledCollision = true;
        collisionMask = 0;
        collisionMask |= World::COLLISION_MASK::STATIC;
        collisionMask |= World::COLLISION_MASK::ACTOR;
        collisionMask &= ~(World::COLLISION_MASK::GRAPH_NODE);
        //collisionNode = sceneMgr->getRootSceneNode()->createChildSceneNode("PlayCollisionNode");
        //Ogre::Entity *collideEntity = sceneMgr->createEntity("PlayerEntity", "player.mesh", "General");
        //collisionModel = newCollisionModel3D(false);
        //addTrianglesToColdet(collideEntity, collisionModel);
        //collisionModel->finalize();
        //collisionNode->attachObject(collideEntity);
        //collisionNode->setPosition(ActorNode->getPosition());
        //updateCollisionModel();

        targetNode = sceneMgr->getRootSceneNode()->createChildSceneNode("PlayerTargetNode");

        bloodCount = 0.0;
        bloodUp = false;
        showBlood = false;

        if(!BloodScreen) {
            BloodScreen = CEGUI::WindowManager::getSingleton().createWindow("SpellByte/StaticImage","BloodUI");
            BloodScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
            BloodScreen->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
            BloodScreen->setProperty("Image", "SpellByteImages/Blood");
            BloodScreen->setAlpha(0);
            BloodScreen->setVisible(false);
            APP->ceguiContext->getRootWindow()->addChild(BloodScreen);
        }

        sleepCount = 0.0;
        sleepUp = false;

        if(!SleepScreen) {
            SleepScreen = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","SleepUI");
            SleepScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
            SleepScreen->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
            //BloodScreen->setProperty("Image", "SpellByteImages/Blood");
            SleepScreen->setAlpha(0);
            SleepScreen->setVisible(false);
            APP->ceguiContext->getRootWindow()->addChild(SleepScreen);
        }

        bindToLUA();
        return true;
    }

    void Player::detachCamera(Ogre::Camera *camera) {
        cameraRollNode->detachObject(camera);
    }

    void Player::attachCamera(Ogre::Camera *camera) {
        cameraRollNode->attachObject(camera);
    }

    Ogre::SceneNode *Player::getCameraNode() {
        return ActorNode;
    }

    const Ogre::Vector3 Player::getPosition() {
        return ActorNode->getPosition();
    }

    Ogre::String Player::getDebugString() {
        Ogre::Vector3 xyz = ActorNode->_getDerivedPosition();
        Ogre::String txt = "X: " + Ogre::StringConverter::toString(xyz.x) +
                        "\nY: " + Ogre::StringConverter::toString(xyz.y) +
                        "\nZ: " + Ogre::StringConverter::toString(xyz.z);
        return txt;
    }

    std::string Player::handleConsoleCmd(std::queue<std::string> cmdQueue) {
        std::string returnString;
        std::string nextCmd = cmdQueue.front();
        cmdQueue.pop();
        if(nextCmd == "debug") {
            return getDebugString();
        } else if(nextCmd == "move") {
            float x, y, z;
            Ogre::Vector3 pos = ActorNode->getPosition();
            x = pos.x;
            y = pos.y;
            z = pos.z;
            returnString = "Player move:";
            while(!cmdQueue.empty()) {
                std::string next = cmdQueue.front();
                cmdQueue.pop();
                if(next == "x") {
                    std::string value = cmdQueue.front().c_str();
                    cmdQueue.pop();
                    x = std::atof(value.c_str());
                    returnString += "\nset x to " + value;
                } else if(next == "y") {
                    std::string value = cmdQueue.front().c_str();
                    cmdQueue.pop();
                    y = std::atof(value.c_str());
                    returnString += "\nset y to " + value;
                } else if(next == "z") {
                    std::string value = cmdQueue.front().c_str();
                    cmdQueue.pop();
                    z = std::atof(value.c_str());
                    returnString += "\nset z to " + value;
                } else {
                    return "Error, invalid argument; expected: x, y, or z";
                }
            }
            ActorNode->setPosition(x, y, z);
        } else {
            return "Player: invalid command";
        }
        return returnString;
    }

    void Player::setCollisionHandler(MOC::CollisionTools *ct) {
        collisionHandler = ct;
        collisionHandler->setHeightAdjust(playerHeight);
        if(enabledCollision)
            collisionHandler->calculateY(ActorNode);
    }

    void Player::update(const Ogre::FrameEvent &evt) {
        if (PlayerAction & PLAYER_SLEEP) {
            displaySleep(evt);
            return;
        }
        moveScale = moveSpeed * evt.timeSinceLastFrame;
        moveCamera();
        rotX = rotY = Ogre::Radian(0);

        getTarget();

        if (PlayerAction & PLAYER_FEED) {
            disableAction(PLAYER_FEED);
            playerFeed();
        }
        if (showBlood)
            displayBlood(evt);
    }

    void Player::playerFeed() {
        if (TargetedActor < 0 && TargetedActor != -2)
            return;
        Courier->DispatchMsg(SEND_MSG_IMMEDIATELY, -1, TargetedActor, MessageType::PLAYER_INTERACT, &ActorNode->getPosition());
    }

    bool Player::handleMessage(const Telegram &msg) {
        LOG("Player Received Message: " + msgToString(msg.Msg));
        if (msg.Msg == MessageType::FEED_SUCCESSFUL) {
            LOG("Feed success");
            feedSuccess();
            return true;
        } else if (msg.Msg == MessageType::SLEEP) {
            enableAction(PLAYER_SLEEP);
            SleepScreen->setVisible(true);
            Courier->DispatchMsg(5000, SENDER_ID_IRRELEVANT, -1, MessageType::END_SLEEP);
            return true;
        } else if( msg.Msg == MessageType::END_SLEEP) {
            disableAction(PLAYER_SLEEP);
            SleepScreen->setVisible(false);
            return true;
        }
        return false;
    }

    void Player::feedSuccess() {
        BloodScreen->setVisible(true);
        showBlood = true;
        bloodTime = APP->getCurrentFrame() + 4000;
        bloodCount = 0.0;
    }

    void Player::displayBlood(const Ogre::FrameEvent &evt) {
        if (bloodTime < APP->getCurrentFrame()) {
            showBlood = false;
            BloodScreen->setVisible(false);
            return;
        }

        if (bloodCount <= 0)
            bloodUp = true;
        else if (bloodCount >= 1.0)
            bloodUp = false;
        if (bloodUp)
            bloodCount  += 0.4 * evt.timeSinceLastFrame;
        else
            bloodCount -= 0.4 * evt.timeSinceLastFrame;
        BloodScreen->setAlpha(bloodCount);
        BloodScreen->invalidate();
    }

    void Player::displaySleep(const Ogre::FrameEvent &evt) {
        if (sleepCount <= 0)
            sleepUp = true;
        else if (sleepCount >= 1.0)
            sleepUp = false;
        if (sleepUp)
            sleepCount  += 0.4 * evt.timeSinceLastFrame;
        else
            sleepCount -= 0.4 * evt.timeSinceLastFrame;
        SleepScreen->setAlpha(sleepCount);
        SleepScreen->invalidate();
    }

    void Player::getTarget() {
        if (TargetedActor >= 0 || TargetedActor == -2) {
            Courier->DispatchMsg(SEND_MSG_IMMEDIATELY, -1, TargetedActor, MessageType::NOT_TARGETED);
        }
        Ogre::Vector3 translateVector = Ogre::Vector3::ZERO;
        translateVector.z = 5.0f;
        translateVector.y = -(playerHeight / 2);

        Ogre::Vector3 oldPos = ActorNode->getPosition();

        targetNode->setPosition(oldPos);
        targetNode->translate(cameraYawNode->getOrientation() *
                                cameraPitchNode->getOrientation() *
                                translateVector, Ogre::SceneNode::TS_LOCAL);
        //target.normalise();
        Ogre::Ray ray(oldPos, targetNode->getPosition());
        RSQ->setRay(ray);
        RSQ->setQueryMask(World::COLLISION_MASK::ACTOR);
        RSQ->setSortByDistance(true);
        Ogre::RaySceneQueryResult &result = RSQ->execute();
        Ogre::RaySceneQueryResult::iterator itr;
        for(itr = result.begin(); itr != result.end(); itr++) {
            if ((itr->movable != NULL)  &&
                (itr->movable->getMovableType().compare("Entity") == 0)) {
                Ogre::Entity *pentity = static_cast<Ogre::Entity*>(itr->movable);
                LOG("Potential target: " + pentity->getName());
                Ogre::Any nodeAny = pentity->getParentSceneNode()->getUserAny();
                if (nodeAny.isEmpty())
                    continue;
                SpellByte::UserAny *any = Ogre::any_cast<SpellByte::UserAny*>(nodeAny);
                if (any->Type != UserAny::ACTOR)
                    continue;
                TargetedActor = any->ID;
                LOG("TARGETING:" + Ogre::StringConverter::toString(TargetedActor));
                Courier->DispatchMsg(SEND_MSG_IMMEDIATELY, -1, TargetedActor, MessageType::TARGETED);
                return;
            } // if ((itr->movable != NULL)  &&
        } // for(itr = result.begin(); itr != result.end(); itr++)
        TargetedActor = -1;
    }

    void Player::handleEvent(int event) {
        switch(event) {
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
        case UserEvent::PLAYER_FEED:
            enableAction(PLAYER_FEED);
            break;
        case UserEvent::PLAYER_SET_CLIPPING:
            enabledCollision = enabledCollision ? false : true;
            if(enabledCollision)
                LOG("Collision enabled");
            else
                LOG("Collision disabled");
            break;
        }
    }

    void Player::setRotation(int X, int Y) {
        rotX = Ogre::Degree(-X * 0.13);
        rotY = Ogre::Degree(-Y * 0.13);
    }

    void Player::moveCamera() {
        translateVector = Ogre::Vector3::ZERO;
        bool move = false;
        if((PlayerAction & PLAYER_UP) && !enabledCollision) {
            ActorNode->setPosition(ActorNode->getPosition() + Ogre::Vector3(0, moveScale, 0));
        }
        if((PlayerAction & PLAYER_DOWN) && !enabledCollision) {
            ActorNode->setPosition(ActorNode->getPosition() - Ogre::Vector3(0, moveScale, 0));
        }
        if(PlayerAction & PLAYER_FORWARD) {
            translateVector.z = -moveScale;
            move = true;
        }
        if(PlayerAction & PLAYER_BACKWARD) {
            translateVector.z = moveScale;
            move = true;
        }
        if(PlayerAction & PLAYER_LEFT) {
            translateVector.x = -moveScale;
            move = true;
        }
        if(PlayerAction & PLAYER_RIGHT) {
            translateVector.x = moveScale;
            move = true;
        }

        Ogre::Vector3 oldPos = ActorNode->getPosition();

        Ogre::Real pitchAngle;
        Ogre::Real pitchAngleSign;

        cameraYawNode->yaw(rotX);
        cameraPitchNode->pitch(rotY);

        if(move) {
            ActorNode->translate(cameraYawNode->getOrientation() *
                                cameraPitchNode->getOrientation() *
                                translateVector, Ogre::SceneNode::TS_LOCAL);
        }

        pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(cameraPitchNode->getOrientation().w)).valueDegrees());

        pitchAngleSign = cameraPitchNode->getOrientation().x;

        if(pitchAngle > 90.0f) {
            if(pitchAngleSign > 0) {
                cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                                 Ogre::Math::Sqrt(0.5f), 0, 0));
            }
            else if(pitchAngleSign < 0) {
                cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                                 -Ogre::Math::Sqrt(0.5f), 0, 0));
            }
        }

        if(!move) {
            return;
        }

        if(enabledCollision) {
            collisionHandler->calculateY(ActorNode,true,true,1.5f,1);
            if (collisionHandler->collidesWithEntity(oldPos, ActorNode->getPosition(), collisionRadius, -1.0f, collisionMask)) {
                ActorNode->setPosition(oldPos);
            }
            else if(ActorNode->getPosition().y - oldPos.y > playerHeight * .4) {
                ActorNode->setPosition(oldPos);
            }
        }
        /*float rayHeightLevel = -1.0f;
        Vector3 fromPoint = oldPos;
        Vector3 toPoint = ActorNode->getPosition();
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
                    //ActorNode->setPosition(oldPos);
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
                        ActorNode->setPosition(oldPos);
                        //collisionNode->setPosition(oldPos);
                        break;
                    }
                }*/
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
                        ActorNode->setPosition(oldPos);
                        break;
                    }
                }*/
            //}
        //}
        //collisionHandler->collidesWithEntity(oldPos, ActorNode->getPosition(), collisionRadius, -1.0f, -1);
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
