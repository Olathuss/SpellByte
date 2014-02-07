#include "console.h"
#include "../define.h"
#include "console_communicator.h"
#include "LuaManager.h"

namespace SpellByte
{
    GameConsoleWindow *GameConsoleWindow::Instance = NULL;

    GameConsoleWindow::GameConsoleWindow() {
        rootWindow = NULL;
        ConsoleWindow = NULL;
        Initialized = false;
        setVisible(false);
        Console = false;
    }

    GameConsoleWindow *GameConsoleWindow::getInstance() {
        if(!Instance) {
            Instance = new GameConsoleWindow();
        }

        return Instance;
    }

    bool GameConsoleWindow::Init(CEGUI::Window *root) {
        rootWindow = root;
        Initialized = createCEGUIWindow();
        setVisible(false);
        LUAMANAGER->subscribe();
        bindToLUA();
        return Initialized;
    }

    void GameConsoleWindow::bindToLUA() {
        SLB::Class< GameConsoleWindow, SLB::Instance::NoCopyNoDestroy >("SpellByte::GCW")
            .set("debugText", ( void(GameConsoleWindow::*)(std::string) ) &GameConsoleWindow::debugText);
        SLB::setGlobal<GameConsoleWindow*>(&(*LUAMANAGER->LUA), getInstance(), "console");
    }

    void GameConsoleWindow::Disable() {
        if(ConsoleWindow) {
            setVisible(false);
            rootWindow->destroyChild(ConsoleWindow);
            ConsoleWindow = NULL;
        }
        if(rootWindow)
            rootWindow = NULL;

        Initialized = false;
    }

    GameConsoleWindow::~GameConsoleWindow()
    {

    }

    bool GameConsoleWindow::createCEGUIWindow()
    {
        // Get a local pointer to the CEGUI Window Manager
        CEGUI::WindowManager *WindowManager = CEGUI::WindowManager::getSingletonPtr();

        // Load layout file
        ConsoleWindow = WindowManager->loadLayoutFromFile("VanillaConsole.layout");

        if(ConsoleWindow && rootWindow) {
            LOG("CEGUI Console Created");
            rootWindow->addChild(ConsoleWindow);
            ConsoleWindow->getChild("History")->setText("SpellByte Console");
            (this)->registerHandlers();
            return true;
        } else {
            LOG("Error: Unable to load ConsoleWindow from .layout");
            return false;
        }
        return false;
    }

    void GameConsoleWindow::registerHandlers() {
        LOG("GCW Registering Handlers");
        ConsoleWindow->getChild("Editbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
                                CEGUI::Event::Subscriber(&GameConsoleWindow::handleTextSubmitted, this));
        LOG("CEGUI Handlers registered");
    }

    bool GameConsoleWindow::handleTextSubmitted(const CEGUI::EventArgs &e) {
        if(!Initialized)
            return false;

        //const CEGUI::WindowEventArgs *args = static_cast<const CEGUI::WindowEventArgs*>(&e);
        LOG("Handling text");

        CEGUI::String Msg = ConsoleWindow->getChild("Editbox")->getText();

        (this)->parseText(Msg);

        ConsoleWindow->getChild("Editbox")->setText("");

        return true;
    }

    void GameConsoleWindow::parseText(CEGUI::String inMsg) {
        if(!Initialized)
            return;

        std::string inString = inMsg.c_str();

        if(inString.length() >= 1) {
            (this)->outputText(inString);
            (this)->outputText(COMM->handleConsoleCmd(inString));
        }
    }

    void GameConsoleWindow::debugText(std::string debugText) {
        outputText(debugText.c_str());
    }

    void GameConsoleWindow::outputText(CEGUI::String inMsg) {
        if(!Initialized)
            return;

        CEGUI::String output = ConsoleWindow->getChild("History")->getText() + inMsg;
        if(output.length() > CONSOLE_BUFFER_SIZE) {
            output = output.substr(CONSOLE_BUFFER_SIZE / 2, output.length());
        }
        ConsoleWindow->getChild("History")->setText(output);
        CEGUI::MultiLineEditbox *history = static_cast<CEGUI::MultiLineEditbox*>(ConsoleWindow->getChild("History"));
        history->getVertScrollbar()->setScrollPosition(history->getVertScrollbar()->getDocumentSize() - history->getVertScrollbar()->getPageSize());
    }

    void GameConsoleWindow::setVisible(bool visible) {
        if(!Initialized)
            return;

        LOG("Console Visibility Being Changed");
        ConsoleWindow->setVisible(visible);
        LOG("Console Visibility Has Been Changed");
        Console = visible;

        CEGUI::Editbox *editBox = static_cast<CEGUI::Editbox*>(ConsoleWindow->getChild("Editbox"));
        editBox->setText("");
        if(visible) {
            editBox->activate();
        } else {
            editBox->deactivate();
        }
    }

    bool GameConsoleWindow::isVisible() {
        if(!Initialized)
            return false;

        return Console;
    }
}
