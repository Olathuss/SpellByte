#ifndef _OBJ_GROUP_H_
#define _OBJ_GROUP_H_

#include <vector>
#include "Object.h"
#include "utilities/tinyxml2.h"

namespace SpellByte {
    class ObjectFactory;

    class ObjectGroup {
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

        std::string GroupName;
        bool Enabled;

        // Store children objects here
        std::vector<Object*> ObjectVector;
        // Store child object groups here
        std::vector<ObjectGroup*> ObjectGroupVector;
        int ysnap;
        static int NextValidID;
        int ID;

    public:
        ObjectGroup();
        ~ObjectGroup();

        void init(tinyxml2::XMLElement *groupElt, ObjectFactory *objFactory, Ogre::SceneNode *parentNode = NULL, ObjectGroup *parentGroup = NULL);

        void loadSubGroups(tinyxml2::XMLElement *groupElt, ObjectFactory *objFactory);
        void loadObjects(tinyxml2::XMLElement *objectElt, ObjectFactory *objFactory);

        void addObject(Object *addObj);
        void removeObject(const Object *removeObj);

        void addObjectGroup(ObjectGroup *addObjGroup);
        void resetY();

        // save group to XML
        void saveGroup(tinyxml2::XMLDocument *xmlDoc, tinyxml2::XMLElement *elt);

        Ogre::SceneNode *GroupNode;
        ObjectGroup *parentGroup;
    };
}

#endif // _OBJ_GROUP_H_
