/*
 * SpellByte
 * 2013 (C) Thomas Gull
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

// Use Courier to access MessageDispatcher singleton
#define Courier MessageDispatcher::getInstance()

namespace SpellByte {
     // predefined values
    const double SEND_MSG_IMMEDIATELY = 0.0;
    const int NO_ADDITIONAL_INFO = 0;
    const int SENDER_ID_IRRELEVANT = -1;

    class Player;

    class MessageDispatcher {
    private:
        bool boundToLUA;

        static MessageDispatcher *Instance;
        // Use standard set container to auto sort and remove duplicates
        std::set<Telegram> PriorityQ;

        // Method used by DispatchMsg and DispatchDelayedMessages
        // Calls message handling member function of receiver
        // for the Telegram
        // Need to replace with ManagerBase
        void Discharge(BaseActor *receiver, const Telegram &msg);

        MessageDispatcher();

        // Bind MessageDispatcher to LUA
        void bindToLUA();

        // Keep copy ctor and assignment as private
        MessageDispatcher(const MessageDispatcher&);
        MessageDispatcher &operator=(const MessageDispatcher&);

        BaseActor *player;
        int playerID;

    public:
        ~MessageDispatcher() {
            player = nullptr;
        }

        void setPlayer(int ID, BaseActor *player);

        static MessageDispatcher *getInstance();
        // send message to another Actor.  Receiving Actor is referenced
        // by ID.
        void DispatchMsg(double delay, int sender, int receiver, int msg, void *additionalInfo = nullptr);
        // For const void*
        void DispatchMsg(double delay, int sender, int receiver, int msg, const void *additionalInfo);

        // send out any delayed messages.  Called once each time through main loop
        void DispatchDelayedMessages();
    };
}

#endif // MESSAGE_DISPATCHER_H
