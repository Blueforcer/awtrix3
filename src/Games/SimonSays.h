#ifndef SimonSays_h
#define SimonSays_h

#include <Arduino.h>

enum SimonSaysState
{
    SIMON_READY,
    SIMON_SHOWSEQ,
    SIMON_USERINPUT,
    SIMON_PAUSE,
    SIMON_LOSE,
    SIMON_WIN
};

class SimonSays_
{
private:
    SimonSays_() = default;
    SimonSaysState currentState = SIMON_READY;
    static const int MAX_SEQ = 32;
    int sequence[MAX_SEQ];
    int sequenceLength = 0;
    int currentStep = 0;
    bool showing = false;
    unsigned long nextBlink = 0;
    int blinkIndex = 0;
    unsigned long loseStart = 0;
    unsigned long nextSequenceTime = 0;
    bool highlightUserInput = false;
    unsigned long highlightUserInputEnd = 0;
    int lastUserButton = -1;
    

    void generateSequence();
    void showSequence();
    void checkUserInput(int button);
    void drawSquares(bool highlight = false, int highlightIndex = -1);

public:
    static SimonSays_ &getInstance();
    void setup();
    void tick();
    void selectPressed();
    void ControllerInput(const char *cmd);
};

extern SimonSays_ &SimonSays;

#endif
