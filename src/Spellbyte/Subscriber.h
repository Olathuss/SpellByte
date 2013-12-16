#ifndef _SUBSCRIBER_H_
#define _SUBSCRIBER_H_

#include <string>
#include <queue>
#include "define.h"

namespace SpellByte
{
    class ConsoleComm;

    class Subscriber
    {
    private:
        bool debugEnabled;

    public:
        Subscriber(){debugEnabled = true;};
        virtual ~Subscriber(){};
        virtual std::string handleConsoleCmd(std::queue<std::string> cmdQueue) = 0;

        void debug(std::string debugText)
        {
            if(debugEnabled)
                LOG(debugText);
        };

        void debug(CEGUI::String debugText)
        {
            if(debugEnabled)
                LOG(debugText.c_str());
        };

        void setDebug(bool enableDebug){debugEnabled=enableDebug;};
    };
}

#endif // _SUBSCRIBER_H_
