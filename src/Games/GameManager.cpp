#include "GameManager.h"
#include "Globals.h"
#include "ServerManager.h"

GameManager_ &GameManager_::getInstance()
{
    static GameManager_ instance;
    return instance;
}

GameManager_ &GameManager = GameManager.getInstance();

void GameManager_::setup()
{
    switch (currentState)
    {
    case Slot_Machine:
        slotMachine.setup();
        break;
    case AWTRIX_Says:
        AwtrixSays.setup();
        break;
    default:
        break;
    }
}

void GameManager_::tick()
{
    switch (currentState)
    {
    case None:
        DisplayManager.drawFilledRect(0, 0, 32, 8, 0);
        DisplayManager.printText(0, 6, "SELECT", true, 0);
        break;
    case Slot_Machine:
        slotMachine.tick();
        break;
    case AWTRIX_Says:
        AwtrixSays.tick();
        break;
    default:
        break;
    }
}

void GameManager_::selectPressed()
{
    switch (currentState)
    {
    case Slot_Machine:
        slotMachine.selectPressed();
        break;
    case AWTRIX_Says:
        AwtrixSays.selectPressed();
        break;
    default:
        break;
    }
}

void GameManager_::ControllerInput(const char *cmd)
{
    switch (currentState)
    {
    case Slot_Machine:
        slotMachine.ControllerInput(cmd);
        break;
    case AWTRIX_Says:
        AwtrixSays.ControllerInput(cmd);
        break;
    default:
        break;
    }
}

void GameManager_::start(bool active)
{
    currentState = None;
    GAME_ACTIVE = active;
}

void GameManager_::ChooseGame(short game)
{
    sendPoints(0);
    switch (game)
    {
    case 0:
        currentState = Slot_Machine;
        setup();
        break;
    case 1:
        currentState = AWTRIX_Says;
        setup();
        break;
    default:
        break;
    }
}

void GameManager_::sendPoints(int points)
{
    DynamicJsonDocument doc(1024);
    doc["points"] = points;
    String jsonString;
    serializeJson(doc, jsonString);
    ServerManager.sendTCP(jsonString);
}