#ifndef ACTOR_H
#define ACTOR_H

#include <map>
#include "stdafx.h"
// Define unique ID for component type

namespace SpellByte
{
    typedef int ActorID;
    struct Telegram;
    class ActorPtr;
    // each component has a type, each Actor
    // should only have one of any component type
    typedef int componentType;

    class BaseComponent
    {
    protected:
        ActorPtr *owner;
    public:
        BaseComponent(ActorPtr *owningActor):owner(owningActor)
        {
            // empty
        }

        virtual ~BaseComponent()
        {
            // Do not delete owner, just lose reference to it
            // Manager should delete the object
            if(owner)
                owner = 0;
        }

        virtual void update()=0;

        virtual void handleMessage(const Telegram &msg)=0;
    };

    // Smart pointer for component
    class ComponentPtr : public Ogre::SharedPtr<BaseComponent>
    {
    public:
        ComponentPtr() : Ogre::SharedPtr<BaseComponent>() {}
        explicit ComponentPtr(BaseComponent *rep) : Ogre::SharedPtr<BaseComponent>(rep) {}
        ComponentPtr(const ComponentPtr &r) : Ogre::SharedPtr<BaseComponent>(r) {}
    };

    class Actor
    {
    friend class ActorFactory;
    typedef std::map<componentType, ComponentPtr> ActorComponents;

    public:

    private:
        // Each Actor needs a unique ID
        ActorID ID;

        // all components for Actor
        ActorComponents components;
        // Special case component
        Ogre::SceneNode *actorSceneNode;

        // Mark Actor has active or inactive
        bool active;

        // next valid ID, each type an Actor is created
        // this is updated
        static int nextValidID;

        // Sets ID of the Actor
        void setID(int value);

    public:
        Actor(ActorID id);

        ~Actor(){}

        // May be unnecessary
        void render();

        // Attach component to Actor
        void attachComponent(componentType type, ComponentPtr component);

        // Detach component from Actor
        void detachComponent(componentType type);

        // Update components
        void update();

        // Update component type
        void updateComponent(componentType);

        // entities handle messages, pass telegram to each component
        void handleMessage(const Telegram &msg);

        // Get pointer to actor's component of type
        ComponentPtr getComponent(componentType type);

        // grab next valid id
        static int getNextValidID();

        // reset entities next id
        static void resetNextValidID();

        int getID()
        {
            return ID;
        }
    };

    // Use smart pointer for entities
    // Which allows for automatic garbage collection
    class ActorPtr : public Ogre::SharedPtr<Actor>
    {
    public:
        ActorPtr() : Ogre::SharedPtr<Actor>() {}
        explicit ActorPtr(Actor *rep) : Ogre::SharedPtr<Actor>(rep) {}
        ActorPtr(const ActorPtr &r) : Ogre::SharedPtr<Actor>(r) {}
    };
}

#endif // ACTOR_H
