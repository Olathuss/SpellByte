#include "MessageDispatcher.h"
#include "../ActorManager.h"
#include "../define.h"
#include "../console/LuaManager.h"

namespace SpellByte {
    MessageDispatcher *MessageDispatcher::Instance = nullptr;

    MessageDispatcher::MessageDispatcher() {
        playerID = INVALID_ACTOR_ID;
        player = nullptr;
        PriorityQ.clear();
    }

    MessageDispatcher *MessageDispatcher::getInstance() {
        if (!Instance) {
            Instance = new MessageDispatcher();
            Instance->bindToLUA();
        }

        return Instance;
    }

    void MessageDispatcher::Discharge(BaseActor *receiver, const Telegram &telegram) {
        if (!receiver->handleMessage(telegram))
            LOG("Message could not be handled");
    }

    void MessageDispatcher::setPlayer(int ID, BaseActor *player) {
        playerID = ID;
        this->player = player;
    }

    void MessageDispatcher::DispatchMsg(double delay, int sender, int receiver, int msg, void *additionalInfo) {
        // ActorMgr not yet implemented
        BaseActor *pReceiver = nullptr;
        if (receiver == playerID && playerID != INVALID_ACTOR_ID && player)
            pReceiver = player;
        else
            pReceiver = ActorMgr->getActorFromID(receiver);

        // ensure receiver is valid target
        if(pReceiver == nullptr)
        {
            LOG("Warning! No receiver with ID of " + Ogre::StringConverter::toString(receiver) + " found for dispatching message");

            return;
        }

        // create telegram
        Telegram telegram(0, sender, receiver, msg, additionalInfo);

        // if there is no delay, send off msg now
        if(delay <= 0.0)
        {
            LOG("Telegram dispatched");

            Discharge(pReceiver, telegram);
        }
        else
        {
            double CurrentTime = APP->getCurrentFrame();

            telegram.DispatchTime = CurrentTime + delay;

            // put it in the queue
            PriorityQ.insert(telegram);

            LOG("Delayed telegram");
        }
    }

    void MessageDispatcher::DispatchDelayedMessages() {
        // first get current time
        double CurrentTime = APP->getCurrentFrame();

        // now dispatch any telegrams that need to be dispatched
        while(!PriorityQ.empty() &&
              (PriorityQ.begin()->DispatchTime < CurrentTime) &&
              (PriorityQ.begin()->DispatchTime > 0))
        {
            // read telegram from front of queue
            const Telegram &telegram = *PriorityQ.begin();

            // get recipient
            Actor *receiver = ActorMgr->getActorFromID(telegram.Receiver);

            LOG("Sending telegram ready for dispatch");

            // send telegram
            Discharge(receiver, telegram);

            // remove from queue
            PriorityQ.erase(PriorityQ.begin());
        }
    }

    void MessageDispatcher::bindToLUA() {
        SLB::Class< MessageDispatcher, SLB::Instance::NoCopyNoDestroy >("SpellByte::MessageDispatcher")
            .set< MessageDispatcher, void, double, int, int, int, void* >("DispatchMsg", &MessageDispatcher::DispatchMsg);

        // Set global access to LUA
        SLB::setGlobal<MessageDispatcher*>(&(*LUAMANAGER->LUA), Courier, "Courier");
    }
}
