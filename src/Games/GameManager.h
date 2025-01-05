#ifndef GameManager_h
#define GameManager_h

#include <Arduino.h>
#include "SlotMachine.h"
#include "AwtrixSays.h"
class SlotMachine;
class AwtrixSays;

class GameManager_
{
    enum GameState
    {
        None,
        Slot_Machine,
        AWTRIX_Says
    };

private:
    GameState currentState = None;
    SlotMachine_ &slotMachine = SlotMachine_::getInstance();
    AwtrixSays_ &AwtrixSays = AwtrixSays_::getInstance();
    GameManager_() = default;

public:
    static GameManager_ &getInstance();
    void setup();
    void tick();
    void start(bool active);
    void ControllerInput(const char *cmd);
    void selectPressed();
    void ChooseGame(short game);
    void sendPoints(int points);
};

extern GameManager_ &GameManager;

#endif