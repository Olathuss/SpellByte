#include "ActorManager.h"
#include "Actor.h"
#include "define.h"
#include "console/LuaManager.h"

namespace SpellByte {
    ActorManager *ActorManager::Instance = nullptr;
    unsigned int ActorManager::ActorCount = 0;

    ActorManager::ActorManager() {
        Actor::bindToLUA();
        ActorVector = std::vector<Actor>(32);
        ActorCount = 0;
    }

    ActorManager::~ActorManager() {
        this->reset();
    }

    ActorManager *ActorManager::getInstance() {
        if (!Instance) {
            Instance = new ActorManager();
            bindToLUA();
            Instance->loadActorSets();
        }

        return Instance;
    }

    void ActorManager::bindToLUA() {
        SLB::Class< ActorManager, SLB::Instance::NoCopyNoDestroy >("SpellByte::ActorManager")
            .set< ActorManager, Actor*, int>("getActorFromID", &ActorManager::getActorFromID)
            .set< ActorManager, int >("getActorCount", &ActorManager::getActorCount);

        SLB::setGlobal<ActorManager*>(&(*LUAMANAGER->LUA), ActorManager::getInstance(), "ActorMgr");
    }

    void ActorManager::loadActorSets() {
        LOG("ActorManager: Loading actor_sets.xml");
        XMLResourcePtr xmlFile = APP->xmlManager->load("actor_sets.xml","General");
        tinyxml2::XMLDocument *actorDoc = xmlFile->getXML();
        if(!actorDoc)
        {
            Ogre::String desc = "Error, actor_sets.xml missing";
            throw(Ogre::Exception(20, desc, "ActorManager"));
        }

        tinyxml2::XMLElement *actorElt = actorDoc->FirstChildElement("actor");
        tinyxml2::XMLElement *requiredElt = actorElt->FirstChildElement("required");
        tinyxml2::XMLElement *reqItem = requiredElt->FirstChildElement("item");
        while (reqItem) {
            Ogre::String requiredName = reqItem->GetText();
            Required.push_back(requiredName);
            tinyxml2::XMLElement *reqSet = actorElt->FirstChildElement(requiredName.c_str());
            ActorSet tmp;
            if (reqSet) {
                tinyxml2::XMLElement *nextItem  = reqSet->FirstChildElement("item");
                while (nextItem) {
                    Ogre::String itemName = nextItem->GetText();
                    tmp.push_back(itemName);
                    nextItem = nextItem->NextSiblingElement("item");
                }
            }
            RequiredSubsets[requiredName] = tmp;
            reqItem = reqItem->NextSiblingElement("item");
        }
        tinyxml2::XMLElement *setElt = actorElt->FirstChildElement("set");
        while (setElt) {
            Ogre::String setName = setElt->Attribute("name");
            SetNames.push_back(setName);
            SetPair setVector;
            tinyxml2::XMLElement *itemElt = setElt->FirstChildElement("item");
            while (itemElt) {
                setVector.push_back(std::make_pair(itemElt->Attribute("type"), itemElt->GetText()));
                itemElt = itemElt->NextSiblingElement("item");
            }
            ActorSets.insert(std::make_pair(setName, setVector));
            setElt = setElt->NextSiblingElement("set");
        }
    }

    ActorManager::ActorSet ActorManager::getRandomSkin() {
        unsigned int whichSet = rand() % SetNames.size();
        ActorSet haveParts;
        ActorSet skinSet;
        std::map<Ogre::String, SetPair>::iterator it;
        it = ActorSets.find(SetNames[whichSet]);
        if (it == ActorSets.end())
            throw(Ogre::Exception(20, "Invalid Actor Set: " + SetNames[whichSet], "ActorManager"));
        for (unsigned int i = 0; i < it->second.size(); ++i) {
            haveParts.push_back(it->second[i].first);
            skinSet.push_back(it->second[i].second);
        }
        std::map<Ogre::String, ActorSet>::iterator getPartIterator;
        for (unsigned int requiredPart = 0; requiredPart < Required.size(); ++requiredPart) {
            bool hasRequiredPart = false;
            for (unsigned int hasPart = 0; hasPart < haveParts.size(); ++hasPart) {
                if (haveParts[hasPart] == Required[requiredPart])
                    hasRequiredPart = true;
            }
            if (!hasRequiredPart) {
                getPartIterator = RequiredSubsets.find(Required[requiredPart]);
                skinSet.push_back(getPartIterator->second[rand() % getPartIterator->second.size()]);
            }
        }
        return skinSet;
    }

    Actor *ActorManager::getFreeActor() {
        if (ActorCount < ActorVector.size()) {
            unsigned int id = ActorCount++;
            ActorVector[id].ID = id;
            return &ActorVector[id];
        } else {
            Actor newActor = Actor();
            newActor.ID = ActorVector.size();
            ActorVector.push_back(newActor);
            ActorCount++;
            return &ActorVector[ActorCount];
        }
    }

    void ActorManager::reset() {
        for (unsigned int i = 0; i < ActorVector.size(); ++i) {
            ActorVector[i].reset();
        }
        ActorCount = 0;
    }

    void ActorManager::clearActors() {
        for (unsigned int i = 0; i < ActorVector.size(); ++i) {
            ActorVector[i].clear();
        }
    }

    Actor *ActorManager::getActorFromID(int id) {
        if ((unsigned int)id >= ActorCount || (unsigned int)id >= ActorVector.size()) {
            LOG("Invalid Actor ID:" + Ogre::StringConverter::toString(id));
            return nullptr;
        }
        return &ActorVector[id];
    }

    void ActorManager::update(const Ogre::FrameEvent &evt) {
        for (unsigned int i = 0; i < ActorCount; ++i) {
            ActorVector[i].update(evt);
        }
    }

    void ActorManager::removeActor(Actor *removeActor) {
        int id = removeActor->getID();
        unsigned int vector_size = ActorVector.size();

        assert((unsigned int)id < vector_size && "<ActorManager>::removeActor invalid ID");

        Actor tmp = ActorVector[vector_size - 1];
        ActorVector[vector_size - 1] = ActorVector[id];
        ActorVector[vector_size - 1].deactivate();
        ActorVector[id] = tmp;
        ActorCount--;
    }

    void ActorManager::registerActor(Actor *newActor) {
        // null
    }
}
