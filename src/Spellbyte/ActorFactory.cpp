#include "ActorFactory.h"
#include "SpellByte.h"

namespace SpellByte
{
    ActorFactory::ActorFactory(void)
    {
        dsl = new Ogre::DotSceneLoader();
        lastActorID = Actor::getNextValidID();
    }

    ActorFactory::~ActorFactory()
    {
        if(dsl)
        {
            delete dsl;
            dsl = 0;
        }
    }

    Actor *ActorFactory::createActor(Ogre::SceneManager *SceneMgr, Ogre::TerrainGroup *tGrp, tinyxml2::XMLElement* xmlRoot)
    {
        Actor *newActor = new Actor(getNextActorID());
        //ActorPtr newActorPtr(newActor);

        tinyxml2::XMLElement *sceneElt = xmlRoot->FirstChildElement("scene");
        Ogre::String scene;
        if(sceneElt)
        {
            scene = Ogre::String(sceneElt->Attribute("name"));
        }

        float x, y, z;
        if(!loadPosition(xmlRoot, x, y, z))
        {
            x = y = z = 0.0;
        }

        newActor->actorSceneNode = SceneMgr->getRootSceneNode()->createChildSceneNode();
        dsl->parseDotScene(scene, "General", SceneMgr, newActor->actorSceneNode, Ogre::StringConverter::toString(newActor->getID()));
        newActor->actorSceneNode->setPosition(Ogre::Vector3(x, tGrp->getHeightAtWorldPosition(x, y, z), z));

        return newActor;
    }

    bool ActorFactory::loadPosition(tinyxml2::XMLElement *elt, float &x, float &y, float &z)
    {
        tinyxml2::XMLElement *pos = elt->FirstChildElement("position");
        if(!pos)
        {
            return false;
        }
        x = pos->FloatAttribute("x");
        y = pos->FloatAttribute("y");
        z = pos->FloatAttribute("z");
        return true;
    }
}
