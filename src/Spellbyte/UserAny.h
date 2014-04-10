#ifndef USER_ANY_H
#define USER_ANY_H

/*
    This class is used to attach as Ogre's Ogre::Any
    to entities, so SpellByte Object/Actors can be
    retrieved from Ogre::Entity
*/

namespace SpellByte {
    class UserAny : public Ogre::Entity {
    public:
        enum {
            OBJECT = 0,
            ACTOR
        };
        UserAny(){};
        UserAny(int type, int id){ Type = type; ID = id; }
        ~UserAny(){};

        int Type;
        int ID;
    };
}

#endif // USER_ANY_H
