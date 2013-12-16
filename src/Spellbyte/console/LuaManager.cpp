#include "../stdafx.h"
#include "LuaManager.h"
#include "console_communicator.h"
#include "console.h"
#include "../define.h"

namespace SpellByte
{
    LuaManager* LuaManager::Instance = NULL;

    LuaManager::LuaManager()
    {
    }

    LuaManager::~LuaManager()
    {
        // Empty
    }

    void LuaManager::Init()
    {
        getScript = false;
        executeScript = false;
        LUA = luaL_newstate();
        luaL_openlibs(LUA);
        SLB::Manager::defaultManager()->registerSLB(LUA);

        SLB::Class< TestClass >("SpellByte::TestClass")
            .constructor()
            .set("makeEntity", ( void(TestClass::*)(const char*, const char*, const char*) ) &TestClass::makeEntity)
            .set("setScale", ( void(TestClass::*)(float, float, float) ) &TestClass::setScale)
            .set("setPosition", ( void(TestClass::*)(float, float, float) ) &TestClass::setPosition)
            .set("callFunc", &TestClass::callFunc);

        /*test = new TestClass();
        //SLB::push<TestClass*>(LUA, test);
        SLB::setGlobal<TestClass*>(LUA, test, "test");*/
    }

    void LuaManager::subscribe()
    {
        COMM->registerSubscriber("lua", this);
    }

    LuaManager* LuaManager::getInstance()
    {
        if(!Instance)
        {
            Instance = new LuaManager();
        }

        return Instance;
    }

    void LuaManager::interpret(std::string lua_text)
    {
        luaL_dostring(LUA, lua_text.c_str());
    }

    std::string LuaManager::handleConsoleCmd(std::queue<std::string> cmdQueue)
    {
        std::string lua_text = "";
        if(cmdQueue.front() == "[")
        {
            COMM->setRedirect(this);
            cmdQueue.pop();
            getScript = true;
        }
        if(getScript && cmdQueue.front() == "]")
        {
            COMM->clearRedirect();
            cmdQueue.pop();
            getScript = false;
            executeScript = true;
        }
        bool nextItem = !cmdQueue.empty();
        while(nextItem)
        {
            lua_text += cmdQueue.front();
            cmdQueue.pop();
            nextItem = !cmdQueue.empty();
            if(nextItem)
            {
                lua_text += " ";
            }
        }

        if(getScript)
        {
            embedScript += lua_text + "\n";
            return ">>";
        }
        else if(executeScript)
        {
            LOG("Executing script: \n" + embedScript);
            luaL_dostring(LUA, embedScript.c_str());
            embedScript = "";
            executeScript = false;
            return "Script completed";
        }
        luaL_dostring(LUA, lua_text.c_str());
        return "Executed: " + lua_text;
    }
}

