#include "ActorFactory.h"
#include "define.h"

namespace SpellByte {
    ActorFactory::ActorFactory(void) {
        dsl = new Ogre::DotSceneLoader();
        lastActorID = Actor::getNextValidID();
    }

    ActorFactory::~ActorFactory() {
        if (dsl) {
            delete dsl;
            dsl = 0;
        }
    }

    Actor *ActorFactory::createActor(Ogre::SceneManager *SceneMgr,
                                     Ogre::SceneNode *parentNode,
                                     Ogre::TerrainGroup *tGrp,
                                     tinyxml2::XMLElement* xmlRoot) {

        // Create actor and set unique id
        Actor *newActor = new Actor(getNextActorID());

        // Create SceneNode for Actor object
        newActor->actorSceneNode = parentNode->createChildSceneNode(Ogre::StringConverter::toString(newActor->getID()));

        // All of this is defunct, but kept here to redo later
        const char *grp = xmlRoot->Attribute("group");
        Ogre::String groupName;
        if (grp != NULL) {
            groupName = Ogre::String(grp);
        } else {
            groupName = "General";
        }
        const char *nme = xmlRoot->Attribute("name");
        Ogre::String actorName;
        if (nme != NULL) {
            actorName = Ogre::String(nme) + Ogre::StringConverter::toString(newActor->getID());
        } else {
            actorName = "Actor" + Ogre::StringConverter::toString(newActor->getID());
        }
        LOG("Loading Actor: " + actorName + " from Group: " + groupName);
        tinyxml2::XMLElement *sceneElt = xmlRoot->FirstChildElement("scene");
        if (sceneElt) {
            Ogre::String scene;
            scene = Ogre::String(sceneElt->Attribute("name"));
            dsl->parseDotScene(scene, "General", SceneMgr, newActor->actorSceneNode, Ogre::StringConverter::toString(newActor->getID()));
        }
        tinyxml2::XMLElement *meshElt = xmlRoot->FirstChildElement("mesh");
        int i = 0;
        while (meshElt != 0) {
            Ogre::Entity *entity;
            Ogre::String meshFile;
            meshFile = Ogre::String(meshElt->Attribute("file"));
            LOG("Loading mesh: " + meshFile);
            //MeshManager::getSingleton().load(mesh, groupName);
            entity = SceneMgr->createEntity(actorName + Ogre::StringConverter::toString(i), meshFile, groupName);
            //newActor->actorEntity->setCastShadows(castShadows);
            newActor->actorSceneNode->attachObject(entity);
            meshElt = meshElt->NextSiblingElement("mesh");
            ++i;
        }
        tinyxml2::XMLElement *materialElt = xmlRoot->FirstChildElement("material");
        if (materialElt) {
            Ogre::String materialFile;
            materialFile = Ogre::String(materialElt->Attribute("file"));
            newActor->actorEntity->setMaterialName(materialFile);
        }

        float x, y, z;
        if (!loadPosition(xmlRoot, x, y, z)) {
            x = y = z = 0.0;
        }

        float sx, sy, sz;
        if (loadScale(xmlRoot, sx, sy, sz)) {
            newActor->actorSceneNode->setScale(sx, sy, sz);
        }
        float roll, pitch, yaw;
        roll = pitch = yaw = 0;
        if (loadRotate(xmlRoot, roll, pitch, yaw)) {
            newActor->actorSceneNode->roll(Ogre::Radian(Ogre::Math::DegreesToRadians(roll)));
            newActor->actorSceneNode->pitch(Ogre::Radian(Ogre::Math::DegreesToRadians(pitch)));
            newActor->actorSceneNode->yaw(Ogre::Radian(Ogre::Math::DegreesToRadians(yaw)));
        }

        newActor->actorSceneNode->setPosition(Ogre::Vector3(x, tGrp->getHeightAtWorldPosition(x, y, z), z));

        return newActor;
    }

    bool ActorFactory::loadPosition(tinyxml2::XMLElement *elt, float &x, float &y, float &z) {
        tinyxml2::XMLElement *pos = elt->FirstChildElement("position");
        if (!pos) {
            return false;
        }
        x = pos->FloatAttribute("x");
        y = pos->FloatAttribute("y");
        z = pos->FloatAttribute("z");
        return true;
    }

    bool ActorFactory::loadScale(tinyxml2::XMLElement *elt, float &x, float &y, float &z) {
        tinyxml2::XMLElement *pos = elt->FirstChildElement("scale");
        if (!pos) {
            return false;
        }
        x = pos->FloatAttribute("x");
        y = pos->FloatAttribute("y");
        z = pos->FloatAttribute("z");
        return true;
    }

    bool ActorFactory::loadRotate(tinyxml2::XMLElement *elt, float &roll, float &pitch, float &yaw) {
        tinyxml2::XMLElement *pos = elt->FirstChildElement("rotate");
        if(!pos) {
            return false;
        }
        roll = pos->FloatAttribute("roll");
        pitch = pos->FloatAttribute("pitch");
        yaw = pos->FloatAttribute("yaw");
        return true;
    }
}
