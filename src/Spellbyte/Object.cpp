#include <cassert>

#include "Object.h"
#include "define.h"
#include "World.h"
#include "utilities/utils.h"

namespace SpellByte
{
    bool Object::BoundToLUA = false;

    int Object::nextValidID = 0;

    Object::Object(int id)
    {
        setID(id);
    }

    Object::Object()
    {
        entityCount = 0;
        setID(nextValidID);
        ysnap = Y_TERRAIN;
    }

    Object::~Object()
    {
        ObjectNode->removeAndDestroyAllChildren();
    }

    bool Object::init()
    {
        WorldPtr = APP->getWorldPtr();
        createNode();
        return true;
    }

    bool Object::init(tinyxml2::XMLElement *objElt, Ogre::SceneNode *parentNode)
    {
        WorldPtr = APP->getWorldPtr();
        const char* objName = objElt->Attribute("name");
        if(objName != NULL)
        {
            ObjectName = std::string(objName);
            createNode(objName, parentNode);
        }
        else
        {
            createNode(parentNode);
        }

        const char* grp = objElt->Attribute("group");
        if(grp != NULL)
        {
            DefaultGroup = std::string(grp);
        }
        else
        {
            DefaultGroup = "General";
        }

        tinyxml2::XMLElement *meshElt = objElt->FirstChildElement("mesh");

        while(meshElt != 0)
        {
            Ogre::String meshFile;
            meshFile = Ogre::String(meshElt->Attribute("file"));
            const char* entGroup = meshElt->Attribute("group");
            Ogre::String meshGroup;
            if(entGroup != NULL)
            {
                meshGroup = Ogre::String(entGroup);
            }
            else
            {
                meshGroup = DefaultGroup;
            }
            LOG("Loading mesh: " + meshFile);
            //MeshManager::getSingleton().load(mesh, groupName);
            //entity = SceneMgr->createEntity("ENT" + objectName + Ogre::StringConverter::toString(i), meshFile, groupName);
            Ogre::Entity* entity = addEntity(meshFile, meshGroup);
            if(entity != NULL) {
                const char* matName = meshElt->Attribute("material");
                if(matName != NULL) {
                    entity->setMaterialName(matName);
                }
            }
            //newObject->actorEntity->setCastShadows(castShadows);
            meshElt = meshElt->NextSiblingElement("mesh");
        }
        const int DEGREES = 0;
        const int RADIANS = 0;
        int rotateType = 0;
        setYsnap(Y_TERRAIN);
        tinyxml2::XMLElement *settingElt = objElt->FirstChildElement("setting");
        if(settingElt)
        {
            const char *ypos = settingElt->Attribute("ysnap");
            if(ypos != NULL)
            {
                if(strcmp(ypos, "relative") == 0)
                {
                    setYsnap(Y_RELATIVE);
                }
                else if(strcmp(ypos, "absolute") == 0)
                {
                    setYsnap(Y_ABSOLUTE);
                }
                else
                {
                    setYsnap(Y_TERRAIN);
                }
            }
            const char *rot = settingElt->Attribute("rotation");
            if(rot != NULL)
            {
                if(strcmp(rot, "degrees") == 0)
                {
                    rotateType = DEGREES;
                }
                else if(strcmp(rot, "radians") == 0)
                {
                    rotateType = RADIANS;
                }
                else
                {
                    rotateType = DEGREES;
                }
            }
        }

        float x, y, z;
        x = y = z = 0.0;
        loadPosition(objElt, x, y, z);
        if (ysnap == Y_ABSOLUTE) {
            ObjectNode->setPosition(x, y, z);
        } else if (ysnap == Y_RELATIVE) {
            y +=  WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z);
            ObjectNode->setPosition(x, y, z);
        } else {
            ObjectNode->setPosition(x, WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z), z);
        }

        if (loadScale(objElt, x, y, z)) {
            ObjectNode->setScale(x, y, z);
        }

        float roll, pitch, yaw, qw, qx, qy, qz;
        roll = pitch = yaw = qw = qx = qy = qz = 0;
        int rotResult = loadRotate(objElt, roll, pitch, yaw, qw, qx, qy, qz);
        if(rotResult == 2) {
            if(rotateType == DEGREES) {
                ObjectNode->roll(Ogre::Radian(Ogre::Math::DegreesToRadians(roll)));
                ObjectNode->pitch(Ogre::Radian(Ogre::Math::DegreesToRadians(pitch)));
                ObjectNode->yaw(Ogre::Radian(Ogre::Math::DegreesToRadians(yaw)));
            }
            else if(rotateType == RADIANS) {
                ObjectNode->roll(Ogre::Radian(roll));
                ObjectNode->pitch(Ogre::Radian(pitch));
                ObjectNode->yaw(Ogre::Radian(yaw));
            }
        }
        else if(rotResult == 1) {
            Ogre::Quaternion rotQ = Ogre::Quaternion(qw, qx, qy, qz);
            ObjectNode->setOrientation(rotQ);
        }

        return true;
    }

    void Object::setYsnap(int newSnap) {
        // snap to terrain by default
        if(newSnap < Y_TERRAIN || newSnap >= Y_INVALID) {
            ysnap = Y_TERRAIN;
        }
        else {
            ysnap = newSnap;
        }
    }

    int Object::getYsnap() {
        return ysnap;
    }

    void Object::update()
    {
        // empty for now
    }

    void Object::saveObject(tinyxml2::XMLDocument *xmlDoc, tinyxml2::XMLElement *elt)
    {
        tinyxml2::XMLElement *objElt = xmlDoc->NewElement("static");
        elt->InsertEndChild(objElt);
        objElt->SetAttribute("name", ObjectName.c_str());
        objElt->SetAttribute("group", DefaultGroup.c_str());

        const Ogre::SceneNode::ObjectIterator it = ObjectNode->getAttachedObjectIterator();
        while(it.hasMoreElements())
        {
            tinyxml2::XMLElement *meshElt = xmlDoc->NewElement("mesh");
            objElt->InsertEndChild(meshElt);
            Ogre::Entity *entity = static_cast<Entity*>(it.getNext());
            meshElt->SetAttribute("file", entity->getMesh()->getName().c_str());
            meshElt->SetAttribute("group", entity->getMesh()->getGroup().c_str());
        }

        tinyxml2::XMLElement *settingElt = xmlDoc->NewElement("setting");
        settingElt->SetAttribute("ysnap", "absolute");
        objElt->InsertEndChild(settingElt);

        tinyxml2::XMLElement *posElt = xmlDoc->NewElement("position");
        Ogre::Vector3 pos = ObjectNode->_getDerivedPosition();
        posElt->SetAttribute("x", pos.x);
        posElt->SetAttribute("y", pos.y);
        posElt->SetAttribute("z", pos.z);
        objElt->InsertEndChild(posElt);

        tinyxml2::XMLElement *rotElt = xmlDoc->NewElement("rotate");
        Ogre::Quaternion quat = ObjectNode->_getDerivedOrientation();
        //ObjectNode->_getDerivedOrientation() +
        //rotElt->SetAttribute("roll", quat.getRoll().valueDegrees());
        //rotElt->SetAttribute("pitch", quat.getPitch().valueDegrees());
        //rotElt->SetAttribute("yaw", quat.getYaw().valueDegrees());
        rotElt->SetAttribute("qw", quat.w);
        rotElt->SetAttribute("qx", quat.x);
        rotElt->SetAttribute("qy", quat.y);
        rotElt->SetAttribute("qz", quat.z);
        objElt->InsertEndChild(rotElt);

        tinyxml2::XMLElement *scaleElt = xmlDoc->NewElement("scale");
        Ogre::Vector3 scale = ObjectNode->_getDerivedScale();
        scaleElt->SetAttribute("x", scale.x);
        scaleElt->SetAttribute("y", scale.y);
        scaleElt->SetAttribute("z", scale.z);
        objElt->InsertEndChild(scaleElt);
    }

    void Object::createNode(Ogre::SceneNode *parentNode)
    {
        Ogre::String nodeName = "NODE" + Ogre::StringConverter::toString((int)ID);
        if(parentNode)
        {
            ObjectNode = parentNode->createChildSceneNode(nodeName);
        }
        else
        {
            ObjectNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
        }
        ObjectNode->setUserAny(Ogre::Any(this));
    }

    void Object::createNode(std::string objectName, Ogre::SceneNode *parentNode)
    {
        Ogre::String nodeName = "NODE" + objectName + Ogre::StringConverter::toString((int)ID);
        if(parentNode)
        {
            ObjectNode = parentNode->createChildSceneNode(nodeName);
        }
        else
        {
            ObjectNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
        }
        ObjectNode->setUserAny(Ogre::Any(this));
    }

    void Object::handleMessage(const Telegram &msg)
    {
        // Handle message code for actor here

        // Pass message to each component
        std::map<componentType, ComponentPtr>::iterator it;
        for(it = components.begin(); it != components.end(); ++it)
        {
            it->second->handleMessage(msg);
        }
    }

    Ogre::Entity* Object::addEntity(std::string meshName, std::string groupName)
    {
        Ogre::String entityName = "ENT" + Ogre::StringConverter::toString(ID) + "_" + Ogre::StringConverter::toString(entityCount++);
        Ogre::Entity *entity = NULL;
        try
        {
            entity = APP->SceneMgr->createEntity(entityName, meshName, groupName);

            ObjectNode->attachObject(entity);
        }
        catch (Ogre::Exception &e)
        {
            return NULL;
        }
        return entity;
    }

    void Object::resetY()
    {
        Ogre::Vector3 newPosition = ObjectNode->_getDerivedPosition();
        if(ysnap == Y_TERRAIN)
        {
            newPosition.y = WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        }
        else if(ysnap == Y_RELATIVE)
        {
            newPosition.y += WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        }
        ObjectNode->_setDerivedPosition(newPosition);
    }

    void Object::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
    {
        if(ysnap == Y_TERRAIN)
        {
            y = WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z);
        }
        else if(ysnap == Y_RELATIVE)
        {
            y += WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z);
        }
        ObjectNode->setPosition(x, y, z);
    }

    void Object::setPosition(Ogre::Vector3 newPosition)
    {
        if(ysnap == Y_TERRAIN)
        {
            newPosition.y = WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        }
        else if(ysnap == Y_RELATIVE)
        {
            newPosition.y += WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        }
        ObjectNode->setPosition(newPosition);
    }

    void Object::setGroup(ObjectGroup *parent)
    {
        ObjectParent = parent;
    }

    ObjectGroup *Object::getGroup() const
    {
        return ObjectParent;
    }

    Ogre::SceneNode *Object::getNode() const
    {
        return ObjectNode;
    }

    void Object::setID(int val)
    {
        assert((val >= nextValidID) && "<Object::setID>: invalid ID");

        ID = val;
        nextValidID = ID + 1;
    }

    void Object::resetNextValidID()
    {
        nextValidID = 0;
    }

    int Object::getNextValidID()
    {
        return nextValidID;
    }
}
