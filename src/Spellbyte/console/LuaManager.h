#ifndef _LUA_INT_H_
#define _LUA_INT_H_

#include "SLB.hpp"
#include "../Subscriber.h"
#include "../stdafx.h"
#include "../define.h"

#define LUAMANAGER LuaManager::getInstance()

namespace SpellByte
{
    class TestClass;
    class LuaManager : public Subscriber
    {
    public:
        // LUA related
        lua_State *LUA;

        ~LuaManager();
        static LuaManager* getInstance();
        void Init();
        void subscribe();

        void interpret(std::string lua_text);

        virtual std::string handleConsoleCmd(std::queue<std::string> cmdQueue);

    private:
        LuaManager();
        LuaManager &operator=(const LuaManager&);
        LuaManager(const LuaManager&);

        bool runScript(Ogre::String script, Ogre::String group="LUA");

        static LuaManager *Instance;
        std::string embedScript;
        bool getScript;
        bool executeScript;

        TestClass *test;
    };

    class TestClass
    {
    public:
        TestClass()
        {
            myNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode("blah");
        };

        void makeEntity(const char* entityName, const char* meshName, const char* groupName)
        {
            myEntity = APP->SceneMgr->createEntity(entityName, meshName, groupName);
            myNode->attachObject(myEntity);
        };
        void setScale(float x, float y, float z)
        {
            myNode->setScale(x, y, z);
        }
        void setPosition(float x, float y, float z)
        {
            myNode->setPosition(x, y, z);
        }
        void callFunc(const char* myfunc)
        {
            LOG("Setting object");
        }

    private:
        Ogre::SceneNode *myNode;
        Ogre::Entity *myEntity;
    };
}

#endif // _LUA_INT_H_

