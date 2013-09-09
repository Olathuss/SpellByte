/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Telegram
 *
 * Used as an interface to send telegrams from
 * different entities to others
 * Singleton class
 *
 * Based off Mat Buckland's Telegram from
 * Programming Game AI by Example
 *
 */

#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H

#include <set>
#include "../stdafx.h"
#include "../Actor.h"

#include "telegram.h"

// Simply, use Courier to access MessageDispatcher singleton
#define Courier MessageDispatcher::getSingletonPtr()

namespace SpellByte
{
     // predefined values
    const double SEND_MSG_IMMEDIATELY = 0.0;
    const int NO_ADDITIONAL_INFO = 0;
    const int SENDER_ID_IRRELEVANT = -1;

    class MessageDispatcher : public Ogre::Singleton<MessageDispatcher>
    {
    private:
        // Use standard set container to auto sort and remove duplicates
        std::set<Telegram> PriorityQ;

        // Method used by DispatchMsg and DispatchDelayedMessages
        // Calls message handling member function of receiver
        // for the Telegram
        // Need to replace with ManagerBase
        void Discharge(ActorPtr receiver, const Telegram &msg);

        MessageDispatcher(){}

        // Keep copy ctor and assignment as private
        MessageDispatcher(const MessageDispatcher&);
        MessageDispatcher &operator=(const MessageDispatcher&);

    public:
        // send message to another Actor.  Receiving Actor is referenced
        // by ID.
        void DispatchMsg(double delay, int sender, int receiver, int msg, void* additionalInfo);

        // send out any delayed messages.  Called once each time through main loop
        void DispatchDelayedMessages();
    };

}

#endif // MESSAGE_DISPATCHER_H
