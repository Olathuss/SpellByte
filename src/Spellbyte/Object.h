#ifndef _OBJECT_H
#define _OBJECT_H

#include <string>
#include <map>
#include "utilities/tinyxml2.h"
#include "stdafx.h"
#include "coldet/coldet.h"

namespace SpellByte {

    void getMeshInformation(const Ogre::Mesh* const mesh,
                        size_t &vertex_count,
                        Ogre::Vector3* &vertices,
                        size_t &index_count,
                        unsigned long* &indices,
                        const Ogre::Vector3 &position,
                        const Ogre::Quaternion &orient,
                        const Ogre::Vector3 &scale);
    void addTrianglesToColdet(Ogre::Entity *entity, CollisionModel3D *coldetModel);

    class World;
    typedef int ObjectID;
    struct Telegram;
    class ObjectPtr;

    class ObjectGroup;
    class Object : public Ogre::Any {
    friend class ObjectFactory;

    public:
    private:
        enum Y_SNAP {
            // Snap to terrain
            Y_TERRAIN,
            // Snap relative to terrain
            Y_RELATIVE,
            // Snap to absolute Y position
            Y_ABSOLUTE,
            Y_INVALID
        };
        static bool BoundToLUA;

        // Each Object needs a unique ID
        ObjectID ID;

        // object name
        std::string ObjectName;
        // default group from which to load meshes
        std::string DefaultGroup;

        // next valid ID, each type an Object is created
        // this is updated
        static int nextValidID;

        // Sets ID of the Object
        void setID(int value);

        // Bind to LUA
        void bindToLUA();

        // create scene node
        void createNode(Ogre::SceneNode *parentNode = NULL);
        void createNode(std::string objectName, Ogre::SceneNode *parentNode = NULL);

        int ysnap;

        std::map<std::string, std::string> meshMaterials;

    protected:
        // Special case component
        Ogre::SceneNode *ObjectNode;
        //std::vector<Ogre::Entity*> objectEntities;

        // Mark Object has active or inactive
        bool active;

        // # of entities
        int entityCount;

        ObjectGroup *ObjectParent;

        World* WorldPtr;

    public:
        CollisionModel3D *coldetModel;
        int coldetID;

    public:
        Object();
        Object(ObjectID id);
        bool operator==(const Object &rhs) {
            return ID == rhs.ID;
        }
        bool init(tinyxml2::XMLElement *objElt, Ogre::SceneNode *parentNode = NULL);
        bool init();

        ~Object();

        // Add entity to object from Mesh
        Ogre::Entity* addEntity(std::string meshName, std::string groupName);

        void setGroup(ObjectGroup *parent);
        ObjectGroup *getGroup() const;

        Ogre::SceneNode *getNode() const;

        const std::string getName() {
            return ObjectName;
        }

        void saveObject(tinyxml2::XMLDocument *xmlDoc, tinyxml2::XMLElement *elt);

        // set object's ysnap, used when setting object's position
        void setYsnap(int newSnap);
        const int getYsnap() const;

        void resetY();
        void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);
        void setPosition(Ogre::Vector3 newPosition);
        /*
        void setPosition(Ogre::Vector3 move);
        Ogre::Vector3 getPosition() const;
        void translate(Ogre::Real x, Ogre::Real y, Ogre::Real z);
        Ogre::Real getX();
        Ogre::Real getY();
        Ogre::Real getZ();
        void setX(Ogre::Real x);
        void setY(Ogre::Real y);
        void setZ(Ogre::Real z);
        void setScale(Ogre::Vector3 scale);
        void setScale(Ogre::Real x, Ogre::Real y, Ogre::Real z);
        Vector3 &getScale();*/

        // Update components
        virtual void update(const Ogre::FrameEvent &evt);

        // entities handle messages, pass telegram to each component
        virtual void handleMessage(const Telegram &msg);

        // grab next valid id
        static int getNextValidID();

        // reset entities next id
        static void resetNextValidID();

        virtual int getID() const {
            return ID;
        }
    };

    // Use smart pointer for entities
    // Which allows for automatic garbage collection
    class ObjectPtr : public Ogre::SharedPtr<Object> {
    public:
        ObjectPtr() : Ogre::SharedPtr<Object>() {}
        explicit ObjectPtr(Object *rep) : Ogre::SharedPtr<Object>(rep) {}
        ObjectPtr(const ObjectPtr &r) : Ogre::SharedPtr<Object>(r) {}
    };
}

#endif // _OBJECT_H
