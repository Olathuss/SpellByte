#include "define.h"
#include "ObjectGroup.h"
#include "ObjectFactory.h"
#include "utilities/utils.h"

namespace SpellByte {
    int ObjectGroup::NextValidID = 0;

    ObjectGroup::ObjectGroup() {
        GroupNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode();
        ID = NextValidID++;
    }

    ObjectGroup::~ObjectGroup() {
        if (!ObjectGroupVector.empty())
            DeleteSTLContainer(ObjectGroupVector);
        ObjectGroupVector.clear();
        if (!ObjectVector.empty())
            DeleteSTLContainer(ObjectVector);
        ObjectVector.clear();
        GroupNode->removeAndDestroyAllChildren();
    }

    void ObjectGroup::init(tinyxml2::XMLElement *groupElt,
                           ObjectFactory *objFactory,
                           Ogre::SceneNode *parentNode,
                           ObjectGroup *parentGroup) {
        const char* grpn = groupElt->Attribute("name");

        GroupName = std::string(grpn);
        Ogre::String grpName = "GRP";
        LOG("Loading group: " + GroupName);
        if (grpn != NULL) {
            grpName += Ogre::String(grpn + Ogre::StringConverter::toString(ID));
        } else {
            grpName += "group" + Ogre::StringConverter::toString(ID);
        }

        if (parentNode) {
            GroupNode = parentNode->createChildSceneNode(grpName);
        } else {
            GroupNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode(grpName);
        }

        loadSubGroups(groupElt, objFactory);
        loadObjects(groupElt, objFactory);

        const int DEGREES = 0;
        const int RADIANS = 0;
        int rotateType = 0;
        tinyxml2::XMLElement *settingElt = groupElt->FirstChildElement("setting");
        if (settingElt) {
            const char *ypos = settingElt->Attribute("ysnap");
            if (ypos != NULL) {
                if (strcmp(ypos, "relative") == 0) {
                    ysnap = Y_RELATIVE;
                } else if (strcmp(ypos, "absolute") == 0) {
                    ysnap = Y_ABSOLUTE;
                } else {
                    ysnap = Y_TERRAIN;
                }
            }
            const char *rot = settingElt->Attribute("rotation");
            if (rot != NULL) {
                if (strcmp(rot, "degrees") == 0) {
                    rotateType = DEGREES;
                }
                else if(strcmp(rot, "radians") == 0) {
                    rotateType = RADIANS;
                } else {
                    rotateType = DEGREES;
                }
            }

            if (settingElt->Attribute("visible") != NULL) {
                if (!settingElt->BoolAttribute("visible")) {
                    GroupNode->setVisible(false);
                }
            }
        }

        float x, y, z;
        if (loadPosition(groupElt, x, y, z)) {
            GroupNode->setPosition(Ogre::Vector3(x, y, z));
        }

        float sx, sy, sz;
        if(loadScale(groupElt, sx, sy, sz)) {
            GroupNode->setScale(sx, sy, sz);
        }

        float roll, pitch, yaw, qw, qx, qy, qz;
        roll = pitch = yaw = qw = qx = qy = qz = 0;
        int rotResult = loadRotate(groupElt, roll, pitch, yaw, qw, qx, qy, qz);
        if (rotResult == 2) {
            if (rotateType == DEGREES) {
                GroupNode->roll(Ogre::Radian(Ogre::Math::DegreesToRadians(roll)));
                GroupNode->pitch(Ogre::Radian(Ogre::Math::DegreesToRadians(pitch)));
                GroupNode->yaw(Ogre::Radian(Ogre::Math::DegreesToRadians(yaw)));
            } else if(rotateType == RADIANS) {
                GroupNode->roll(Ogre::Radian(roll));
                GroupNode->pitch(Ogre::Radian(pitch));
                GroupNode->yaw(Ogre::Radian(yaw));
            }
        }
        else if (rotResult == 1) {
            Ogre::Quaternion rotQ = Ogre::Quaternion(qw, qx, qy, qz);
            GroupNode->setOrientation(rotQ);
        }
    }

    void ObjectGroup::resetY() {
        for (unsigned int i = 0; i < ObjectGroupVector.size(); i++) {
            ObjectGroupVector[i]->resetY();
        }

        for (unsigned int i = 0; i < ObjectVector.size(); i++) {
            ObjectVector[i]->resetY();
        }
    }

    void ObjectGroup::saveGroup(tinyxml2::XMLDocument *xmlDoc,
                                tinyxml2::XMLElement *elt) {
        tinyxml2::XMLElement *grpElt = xmlDoc->NewElement("group");
        elt->InsertEndChild(grpElt);
        grpElt->SetAttribute("name", GroupName.c_str());

        for (unsigned int i = 0; i < ObjectGroupVector.size(); i++) {
            ObjectGroupVector[i]->saveGroup(xmlDoc, grpElt);
        }

        for (unsigned int i = 0; i < ObjectVector.size(); i++) {
            ObjectVector[i]->saveObject(xmlDoc, grpElt);
        }
    }

    void ObjectGroup::loadSubGroups(tinyxml2::XMLElement *groupElt,
                                    ObjectFactory *objFactory) {
        tinyxml2::XMLElement *group = groupElt->FirstChildElement("group");
        while (group) {
            ObjectGroup *subGroup = new ObjectGroup();
            subGroup->init(group, objFactory, GroupNode, this);
            ObjectGroupVector.push_back(subGroup);
            group = group->NextSiblingElement("group");
        }
    }

    void ObjectGroup::loadObjects(tinyxml2::XMLElement *objectElt, ObjectFactory *objFactory) {
        tinyxml2::XMLElement *object = objectElt->FirstChildElement("object");
        while (object) {
            LOG("Creating Objects: " + Ogre::String(object->Attribute("name")));
            addObject(objFactory->createObject(object, GroupNode));
            //ActorMgr->registerActor(newActor);
            //worldActors.push_back(newActor);
            object = object->NextSiblingElement("object");
        }
    }

    void ObjectGroup::addObject(Object *addObj) {
        ObjectVector.push_back(addObj);
    }

    void ObjectGroup::removeObject(const Object *removeObj) {
        std::vector<Object*>::iterator it;
        for (it = ObjectVector.begin(); it != ObjectVector.end(); it++) {
            if (*it == removeObj) {
                ObjectVector.erase(it);
                break;
            }
        }
    }

    void ObjectGroup::addObjectGroup(ObjectGroup *addObjGroup) {
        ObjectGroupVector.push_back(addObjGroup);
    }
}
