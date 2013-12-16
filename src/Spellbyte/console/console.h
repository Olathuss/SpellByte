#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <CEGUI/CEGUI.h>
#include <iostream>
#include "SLB.hpp"

#define GCW GameConsoleWindow::getInstance()

namespace SpellByte
{
    class ConsoleComm;
    class GameConsoleWindow
    {
    friend ConsoleComm;
    public:
        static GameConsoleWindow* getInstance();
        bool Init(CEGUI::Window *root);
        ~GameConsoleWindow();
        // Hide or show the console
        void setVisible(bool visible);
        // return true if visible
        bool isVisible();
        void Disable();
        void debugText(std::string debugText);
        void bindToLUA();

    private:
        GameConsoleWindow();
        GameConsoleWindow &operator=(const GameConsoleWindow&);
        GameConsoleWindow(const GameConsoleWindow&);
        // Function which will load CEGUI Window and register event handlers
        bool createCEGUIWindow();
        // Register handler functions
        void registerHandlers();
        // Handle when user presses Enter after typing
        bool handleTextSubmitted(const CEGUI::EventArgs &e);
        // Parse text user has submitted
        void parseText(CEGUI::String inMsg);
        // Post the message to the ChatHistory listbox, which white default color
        void outputText(CEGUI::String inMsg);

        CEGUI::Window *rootWindow;
        // Pointer to the ConsoleRoot window
        CEGUI::Window *ConsoleWindow;
        bool Console;
        bool Initialized;
        static const unsigned int CONSOLE_BUFFER_SIZE = 1024;
        lua_State *LUA;
        static GameConsoleWindow *Instance;
    };
}

#endif // _CONSOLE_H_
