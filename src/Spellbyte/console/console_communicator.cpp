#include "console_communicator.h"

#include <string>
#include <queue>

namespace SpellByte
{
    ConsoleComm *ConsoleComm::Instance = NULL;

    ConsoleComm* ConsoleComm::getInstance()
    {
        if(!Instance)
        {
            Instance = new ConsoleComm();
        }

        return Instance;
    }

    void ConsoleComm::setRedirect(Subscriber *subscriber)
    {
        redirect = subscriber;
        haveRedirect = true;
    }

    void ConsoleComm::clearRedirect()
    {
        redirect = NULL;
        haveRedirect = false;
    }

    std::string ConsoleComm::handleConsoleCmd(std::string command)
    {
        std::queue<std::string> cmdQueue;
        std::size_t pos = command.find_first_of(" ");
        std::size_t last_pos = command.find_first_not_of(" ");
        while(pos != std::string::npos || last_pos != std::string::npos)
        {
            cmdQueue.push(command.substr(last_pos, pos - last_pos));
            last_pos = command.find_first_not_of(" ", pos);
            pos = command.find_first_of(" ", last_pos);
        }
        if(haveRedirect)
        {
            if(redirect)
            {
                LOG("Redirecting console input");
                return redirect->handleConsoleCmd(cmdQueue);
            }
            else
                return "An error has occurred, Jim";
        }

        std::map<std::string, Subscriber*>::iterator it;
        it = SubscriberMap.find(cmdQueue.front());
        cmdQueue.pop();
        if(it == SubscriberMap.end())
        {
            return "Invalid command or no subscriber";
        }
        return it->second->handleConsoleCmd(cmdQueue);
    }

    bool ConsoleComm::registerSubscriber(std::string cmd, Subscriber *registrar)
    {
        SubscriberMap.insert(std::make_pair(cmd, registrar));
        return true;
    }
}
