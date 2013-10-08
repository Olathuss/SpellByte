#include "MessageDispatcher.h"
#include "../ActorManager.h"
#include "../SpellByte.h"

namespace SpellByte
{
    template<> MessageDispatcher* Ogre::Singleton<MessageDispatcher>::msSingleton = 0;

    void MessageDispatcher::Discharge(ActorPtr receiver, const Telegram &telegram)
    {
        receiver->handleMessage(telegram);
    }

    void MessageDispatcher::DispatchMsg(double delay, int sender, int receiver, int msg, void *additionalInfo = NULL)
    {
        // ActorMgr not yet implemented
        /*ActorPtr pReceiver = ActorMgr->getActorFromID(receiver);

        // ensure receiver is valid target
        if(pReceiver.isNull())
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
        }*/
    }

    void MessageDispatcher::DispatchDelayedMessages()
    {
        // first get current time
        /*double CurrentTime = APP->getCurrentFrame();

        // now dispatch any telegrams that need to be dispatched
        while(!PriorityQ.empty() &&
              (PriorityQ.begin()->DispatchTime < CurrentTime) &&
              (PriorityQ.begin()->DispatchTime > 0))
        {
            // read telegram from front of queue
            const Telegram &telegram = *PriorityQ.begin();

            // get recipient
            ActorPtr receiver = ActorMgr->getActorFromID(telegram.Receiver);

            LOG("Sending telegram ready for dispatch");

            // send telegram
            Discharge(receiver, telegram);

            // remove from queue
            PriorityQ.erase(PriorityQ.begin());
        }*/
    }
}
