/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * ControlManager Class
 *
 * This class manages input from the Application Class
 * All input is kept here inside a vector.
 * This is a singleton class.
 *
 */

#ifndef _CONTROL_MANAGER_H_
#define _CONTROL_MANAGER_H_

#include <queue>
#include "stdafx.h"

#define CONTROL ControlManager::getInstance()

namespace SpellByte
{
    // class to handle events
    class UserEvent
    {
    public:
        // ENUM to describe different input cases
        enum
        {
            START = 0,
            ESCAPE,
            MOUSE_CLICK,
            MOUSE_RIGHT_PRESS,
            MOUSE_LEFT_PRESS,
            MOUSE_RIGHT_RELEASE,
            MOUSE_LEFT_RELEASE,
            MOUSE_OTHER_PRESS,
            MOUSE_OTHER_RELEASE,
            MOUSE_RELEASE,
            MOUSE_MOVE,
            ZOOM_IN,
            ZOOM_OUT,
            END
        };
        UserEvent(int nType);
        ~UserEvent(){};
        // Get name of event
        const Ogre::String getName() const;
        // Get type of event
        const int getType() const;

        // Get Abs value of mouse position
        void getAbs(int &x, int &y) const;
        // Get relative x and y of mouse position
        void getRel(int &x, int &y) const;

        // Set absolute mouse X/Y
        void setAbs(const int &x, const int &y);
        // Set relative mouse X/Y
        void setRel(const int &x, const int &y);

        bool operator==(const UserEvent &rhs);

    private:
        int absX;
        int absY;
        int relX;
        int relY;
        int type;
    };

    class ControlManager
    {
    public:
        // Returns singleton instance
        static ControlManager* getInstance();

        // Add Event to ControlManager
        void addEvent(UserEvent* evt);

        // Check for next event, and set it as current event
        bool hasEvent();

        // Get next event
        const UserEvent* getEvent() const;

        void shutDown();

        int EventCount()
        {
            return EventQueue.size();
        };

        // Destructor
        ~ControlManager();

    private:
        UserEvent* currentEvent;
        // Used to declare whether singleton instance has been created
        static bool instanceFlag;
        // Singleton instance
        static ControlManager* Instance;
        // Constructor is private for singleton
        ControlManager();
        // Do not implement next two and keep private
        ControlManager(ControlManager const&); // Do not implement
        void operator=(ControlManager const&); // Do not implement

        std::queue<UserEvent*> EventQueue;
    };
}

#endif // _CONTROL_MANAGER_H_

