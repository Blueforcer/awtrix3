#ifndef AwtrixSays_h
#define AwtrixSays_h

#include <Arduino.h>

enum AwtrixSaysState
{
    AWTRIX_READY,
    AWTRIX_SHOWSEQ,
    AWTRIX_USERINPUT,
    AWTRIX_PAUSE,
    AWTRIX_LOSE,
    AWTRIX_WIN
};

class AwtrixSays_
{
private:
    AwtrixSays_() = default;
    AwtrixSaysState currentState = AWTRIX_READY;
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
    static AwtrixSays_ &getInstance();
    void setup();
    void tick();
    void selectPressed();
    void ControllerInput(const char *cmd);
};

extern AwtrixSays_ &AwtrixSays;

#endif
