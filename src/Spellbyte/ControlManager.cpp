#include "ControlManager.h"
#include "SpellByte.h"

namespace SpellByte
{
    bool ControlManager::instanceFlag = false;
    ControlManager* ControlManager::Instance = NULL;

    UserEvent::UserEvent(int nType)
    {
        // Initialize event to INVALID data
        absX = absY = relX = relY = type = INVALID;
        type = nType;
    }

    ControlManager::ControlManager()
    {
        currentEvent = NULL;
    }

    ControlManager* ControlManager::getInstance()
    {
        if(!instanceFlag)
        {
            Instance = new ControlManager();
            instanceFlag = true;
            return Instance;
        }
        else
        {
            return Instance;
        }
    }

    ControlManager::~ControlManager()
    {
        instanceFlag = false;
    }

    void ControlManager::shutDown()
    {
        while(!EventQueue.empty())
        {
            if(currentEvent)
                delete currentEvent;
            currentEvent = EventQueue.front();
            EventQueue.pop();
        }
        if(currentEvent)
            delete currentEvent;
    }

    void ControlManager::addEvent(UserEvent* evt)
    {
        EventQueue.push(evt);
    }

    bool ControlManager::hasEvent()
    {
        if(EventQueue.empty())
            return false;
        if(currentEvent)
            delete currentEvent;
        currentEvent = EventQueue.front();
        EventQueue.pop();
        return true;
    }

    const UserEvent* ControlManager::getEvent() const
    {
        return currentEvent;
    }

    const int UserEvent::getType() const
    {
        return type;
    }

    bool UserEvent::operator==(const UserEvent &rhs)
    {
        return ((this->type == rhs.type) &&
                (this->absX == rhs.absX) &&
                (this->absY == rhs.absY) &&
                (this->relX == rhs.relX) &&
                (this->relY == rhs.relY));
    }

    const Ogre::String UserEvent::getName() const
    {
        switch(type)
        {
        case START:
            {
                return "START";
                break;
            }
        case MOUSE_CLICK:
            {
                return "MOUSE_CLICK";
                break;
            }
        case MOUSE_MOVE:
            {
                return "MOUSE_MOVE";
                break;
            }
        case MOUSE_RELEASE:
            {
                return "MOUSE_RELEASE";
                break;
            }
        case END:
            {
                return "END";
                break;
            }
        default:
            return "UNKNOWN";
        }
    }

    void UserEvent::setAbs(const int &x, const int &y)
    {
        absX = x;
        absY = y;
    }

    void UserEvent::setRel(const int &x, const int &y)
    {
        relX = x;
        relY = y;
    }

    void UserEvent::getAbs(int &x, int &y) const
    {
        x = absX;
        y = absY;
    }

    void UserEvent::getRel(int &x, int &y) const
    {
        x = relX;
        y = relY;
    }
}

