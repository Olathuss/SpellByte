/*
 * SpellByte
 * 2013 (C) Thomas Gull
 *
 * Telegram
 *
 * This defines a telegram struct which is a data structure
 * used to record information required to dispatch a message.
 * Messages are used by different game entities to communicate
 * with each other.
 *
 * Based off Mat Buckland's Telegram from
 * Programming Game AI by Example
 *
 */

#ifndef TELEGRAM_H
#define TELEGRAM_H

#include "../stdafx.h"

namespace SpellByte {
    struct Telegram {
        // Messages can be either dispatched immediately or delayed for
        // a specified amount of time.  If a delay is necessary, this
        // field is stamped with the time the message should be dispatched.
        Ogre::Real DispatchTime;

        // Who is sending this message
        int Sender;

        // Who should the component give this message to
        // may be set to -1 if not required
        int Receiver;

        // Message itself, should be one of the several enumerated types
        int Msg;

        // Additional information that might want to be passed along
        const void *ExtraInfo;

        Telegram():DispatchTime(-1),
                    Sender(-1),
                    Receiver(-1),
                    Msg(-1)
        {
            // Empty
        }

        Telegram(double time,
                 int sender,
                 int receiver,
                 int msg,
                 const void *info = nullptr):DispatchTime(time),
                                        Sender(sender),
                                        Receiver(receiver),
                                        Msg(msg),
                                        ExtraInfo(info)
        {
            // Empty
        }

        bool operator<(const Telegram &rhs) {
            return this->DispatchTime < rhs.DispatchTime;
        }
    };

    // Telegrams are stored in a priority queue.  The > operator needs to be
    // overloaded so the queue can sort by time priority.  Times must be smaller
    // than the value SmallestDelay to consider two Telegrams unique
    const double SmallestDelay = 0.25;

    inline bool operator==(const Telegram &t1, const Telegram &t2) {
        return ( fabs(t1.DispatchTime-t2.DispatchTime) < SmallestDelay) &&
                (t1.Sender == t2.Sender) &&
                (t1.Receiver == t1.Receiver) &&
                (t1.Msg == t2.Msg);
    }

    inline bool operator<(const Telegram &t1, const Telegram &t2) {
        if(t1 == t2) {
            return false;
        } else {
            return (t1.DispatchTime < t2.DispatchTime);
        }
    }

    inline std::ostream &operator<<(std::ostream& os, const Telegram &t) {
        os << "Time: " << t.DispatchTime << " Sender: " << t.Sender
            << " Receiver: " << t.Receiver << " Msg: " << t.Msg;

        return os;
    }

    // Helper function to dereference ExtraInfo
    template <class T>
    inline const T& DereferenceToType(const void *p) {
        return *(const T*)(p);
    }
}

#endif // TELEGRAM_H
