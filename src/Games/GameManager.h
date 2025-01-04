#ifndef GameManager_h
#define GameManager_h

#include <Arduino.h>
#include "SlotMachine.h"
#include "SimonSays.h"
class SlotMachine;
class SimonSays;

class GameManager_
{
    enum GameState
    {
        None,
        Slot_Machine,
        Simon_Says
    };

private:
    GameState currentState = None;
    SlotMachine_ &slotMachine = SlotMachine_::getInstance();
    SimonSays_ &simonSays = SimonSays_::getInstance();
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