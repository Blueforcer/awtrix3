#ifndef SlotMachine_h
#define SlotMachine_h

#include <Arduino.h>

#define NUM_REELS 3
#define REEL_WIDTH 8
#define REEL_HEIGHT 8

#define LEFT_MARGIN 2
#define GAP_BETWEEN_REELS 2

#define NUM_SYMBOLS 8

enum SlotMachineState
{
    SLOT_IDLE,
    SLOT_SPINNING,
    SLOT_RESULT,
    SLOT_BLINKWIN
};

class SlotMachine_
{
private:
    SlotMachine_() = default;

    SlotMachineState currentState = SLOT_IDLE;
    int points = 0;
    unsigned long reelStartTime[NUM_REELS]; // Wann begann die Walze zu drehen?
    float reelStartSpeed[NUM_REELS];        // Mit welcher Geschwindigkeit startete die Walze?
bool reelShouldStop[NUM_REELS];  // true, wenn Endzeit erreicht
bool reelIsStopping[NUM_REELS];  // könnte man sich überlegen, optional
    int reelSymbols[NUM_REELS];
    float reelOffsets[NUM_REELS];
    float reelSpeeds[NUM_REELS];
    unsigned long spinEndTime[NUM_REELS];
    int reelNextSymbols[NUM_REELS]; // Nächstes Symbol, das gleich reingescrollt wird
    bool winningReels[NUM_REELS];
    bool drawWinningSymbols = true;
    int blinkCount = 0;
    unsigned long nextBlinkTime = 0;

    void drawReel(int reelIndex);
    void startSpin();
    void stopReel(int reelIndex);
    bool allReelsStopped();
    void drawSlots();
    bool checkWin();
    void resetWinningReels();

public:
    static SlotMachine_ &getInstance();
    void selectPressed();
    void setup();
    void tick();
    void ControllerInput(const char* cmd);
};

extern SlotMachine_ &SlotMachine;

#endif
