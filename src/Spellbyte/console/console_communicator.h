/* This class is used to communicate between the console
    and other parts of the SpellByte engine. */

#ifndef _CONSOLE_COM_H_
#define _CONSOLE_COM_H_

#include <map>
#include "../Subscriber.h"

#define COMM ConsoleComm::getInstance()

namespace SpellByte
{
    class ConsoleComm
    {
    public:
        static ConsoleComm* getInstance();
        bool registerSubscriber(std::string cmd, Subscriber *registrar);
        std::string handleConsoleCmd(std::string command);

        void setRedirect(Subscriber *subscriber);
        void clearRedirect();

    private:
        ConsoleComm()
        {
            redirect = NULL;
            haveRedirect = false;
        };
        ConsoleComm(const ConsoleComm&);
        ConsoleComm &operator=(const ConsoleComm&);
        Subscriber *redirect;
        bool haveRedirect;

        std::map<const std::string, Subscriber*> SubscriberMap;
        static ConsoleComm *Instance;
    };
}

#endif // _CONSOLE_COM_H_
