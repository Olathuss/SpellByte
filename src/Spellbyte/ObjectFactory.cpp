
#include "ObjectFactory.h"
#include "SpellByte.h"

namespace SpellByte
{
    ObjectFactory::ObjectFactory(void)
    {
        dsl = new Ogre::DotSceneLoader();
        lastObjectID = 1;
    }

    ObjectFactory::~ObjectFactory()
    {
        if(dsl)
        {
            delete dsl;
            dsl = 0;
        }
    }

    Object* ObjectFactory::createObject(tinyxml2::XMLElement* xmlRoot, Ogre::SceneNode *parentNode)
    {
        Object *newObject = new Object();
        newObject->init(xmlRoot, parentNode);

        return newObject;
    }

    bool ObjectFactory::createObjectDefunct(Ogre::SceneManager *SceneMgr, Ogre::SceneNode *parentNode, Ogre::TerrainGroup *tGrp, tinyxml2::XMLElement* xmlRoot)
    {
        Ogre::SceneNode *newObject = parentNode->createChildSceneNode(Ogre::StringConverter::toString(getNextObjectID()));

        const char *grp = xmlRoot->Attribute("group");
        Ogre::String groupName;
        if(grp != NULL)
        {
            groupName = Ogre::String(grp);
        }
        else
        {
            groupName = "General";
        }
        const char *nme = xmlRoot->Attribute("name");
        Ogre::String objectName;
        if(nme != NULL)
        {
            objectName = Ogre::String(nme) + Ogre::StringConverter::toString(getNextObjectID());
        }
        else
        {
            objectName = "Object" + Ogre::StringConverter::toString(getNextObjectID());
        }
        LOG("Loading Object: " + objectName + " from Group: " + groupName);
        tinyxml2::XMLElement *sceneElt = xmlRoot->FirstChildElement("scene");
        if(sceneElt)
        {
            Ogre::String scene;
            scene = Ogre::String(sceneElt->Attribute("name"));
            dsl->parseDotScene(scene, "General", SceneMgr, newObject, Ogre::StringConverter::toString(getNextObjectID()));
        }
        tinyxml2::XMLElement *meshElt = xmlRoot->FirstChildElement("mesh");
        int i = 0;
        while(meshElt != 0)
        {
            Ogre::Entity *entity;
            Ogre::String meshFile;
            meshFile = Ogre::String(meshElt->Attribute("file"));
            LOG("Loading mesh: " + meshFile);
            //MeshManager::getSingleton().load(mesh, groupName);
            entity = SceneMgr->createEntity("ENT" + objectName + Ogre::StringConverter::toString(i), meshFile, groupName);
            if(meshElt->Attribute("material") != NULL)
            {
                entity->setMaterialName(meshElt->Attribute("material"));
            }
            //newObject->actorEntity->setCastShadows(castShadows);
            newObject->attachObject(entity);
            meshElt = meshElt->NextSiblingElement("mesh");
            ++i;
        }
        /*tinyxml2::XMLElement *materialElt = xmlRoot->FirstChildElement("material");
        if(materialElt)
        {
            Ogre::String materialFile;
            materialFile = Ogre::String(materialElt->Attribute("file"));
            newObject->actorEntity->setMaterialName(materialFile);
        }*/

        float x, y, z;
        x = y = z = 0.0;
        int ysnap = Y_TERRAIN;
        if(loadPosition(xmlRoot, x, y, z, ysnap))
        {
            if(ysnap == Y_TERRAIN)
            {
                y = tGrp->getHeightAtWorldPosition(x, y, z);
                LOG("Object Position Snapped to Terrain: " + Ogre::StringConverter::toString(y));
                newObject->setPosition(Ogre::Vector3(x, y, z));
            }
            else if(ysnap == Y_RELATIVE)
            {
                y += tGrp->getHeightAtWorldPosition(x, y, z);
                LOG("Object Position Relative to Terrain: " + Ogre::StringConverter::toString(y));
                newObject->setPosition(Ogre::Vector3(x, y, z));
            }
            else if(ysnap == Y_ABSOLUTE)
            {
                LOG("Object Position Absolute: " + Ogre::StringConverter::toString(y));
                newObject->setPosition(Ogre::Vector3(x, y, z));
            }
        }

        float sx, sy, sz;
        if(loadScale(xmlRoot, sx, sy, sz))
        {
            newObject->setScale(sx, sy, sz);
        }
        float roll, pitch, yaw;
        roll = pitch = yaw = 0;
        if(loadRotate(xmlRoot, roll, pitch, yaw))
        {
            newObject->roll(Ogre::Radian(Ogre::Math::DegreesToRadians(roll)));
            newObject->pitch(Ogre::Radian(Ogre::Math::DegreesToRadians(pitch)));
            newObject->yaw(Ogre::Radian(Ogre::Math::DegreesToRadians(yaw)));
        }

        return true;
    }

    bool ObjectFactory::loadPosition(tinyxml2::XMLElement *elt, float &x, float &y, float &z, int &ysnap)
    {
        tinyxml2::XMLElement *pos = elt->FirstChildElement("position");
        if(!pos)
        {
            return false;
        }
        x = pos->FloatAttribute("x");
        y = pos->FloatAttribute("y");
        z = pos->FloatAttribute("z");
        const char *ypos = pos->Attribute("ysnap");
        if(ypos == NULL)
        {
            ysnap = Y_TERRAIN;
        }
        else
        {
            if(strcmp(ypos, "relative") == 0)
            {
                ysnap = Y_RELATIVE;
            }
            else if(strcmp(ypos, "absolute") == 0)
            {
                ysnap = Y_ABSOLUTE;
            }
            else
            {
                ysnap = Y_TERRAIN;
            }
        }
        return true;
    }

    bool ObjectFactory::loadScale(tinyxml2::XMLElement *elt, float &x, float &y, float &z)
    {
        tinyxml2::XMLElement *pos = elt->FirstChildElement("scale");
        if(!pos)
        {
            return false;
        }
        x = pos->FloatAttribute("x");
        y = pos->FloatAttribute("y");
        z = pos->FloatAttribute("z");
        return true;
    }

    bool ObjectFactory::loadRotate(tinyxml2::XMLElement *elt, float &roll, float &pitch, float &yaw)
    {
        tinyxml2::XMLElement *pos = elt->FirstChildElement("rotate");
        if(!pos)
        {
            return false;
        }
        roll = pos->FloatAttribute("roll");
        pitch = pos->FloatAttribute("pitch");
        yaw = pos->FloatAttribute("yaw");
        return true;
    }
}
