#include "SlotMachine.h"
#include "Globals.h"
#include "DisplayManager.h"
#include "GameManager.h"
#include "PeripheryManager.h"

static const uint32_t symbolCherry[64] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x008709, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x008709, 0x000000, 0x008709, 0x000000, 0x000000,
    0x000000, 0x000000, 0x008709, 0x000000, 0x000000, 0x008709, 0x000000, 0x000000,
    0x000000, 0x000000, 0x008709, 0x000000, 0x000000, 0xFF2222, 0xFF2222, 0x000000,
    0x000000, 0xFF2222, 0xFF2222, 0x000000, 0xFF2222, 0xFF7E7E, 0xFF0000, 0x8D0000,
    0xFF2222, 0xFF7E7E, 0xFF0000, 0x8D0000, 0xFF2222, 0xFF0000, 0xFF0000, 0x8D0000,
    0xFF2222, 0xFF0000, 0xFF0000, 0x8D0000, 0x000000, 0x8D0000, 0x8D0000, 0x000000,
    0x000000, 0x8D0000, 0x8D0000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

static const uint32_t symbolDiamond[64] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x000000,
    0x7BFCFF, 0xFFFFFF, 0xFFFFFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF,
    0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF, 0x7BFCFF,
    0x000000, 0x00C6CA, 0x7BFCFF, 0x00C6CA, 0x00C6CA, 0x00C6CA, 0x00C6CA, 0x000000,
    0x000000, 0x000000, 0x00C6CA, 0x7BFCFF, 0x00C6CA, 0x00C6CA, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x00C6CA, 0x00C6CA, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

static const uint32_t symbolSeven[64] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0xFFB638, 0xDB8700, 0xDB8700, 0xDB8700, 0xDB8700, 0x985B00, 0x000000,
    0x000000, 0xFFB638, 0xDB8700, 0x000000, 0x000000, 0xDB8700, 0x985B00, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0xDB8700, 0x985B00, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0xDB8700, 0x985B00, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0xDB8700, 0x985B00, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0xDB8700, 0x985B00, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

static const uint32_t symbolGrape[64] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x008F07, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x008F07, 0x008F07, 0x000000, 0x000000, 0x000000,
    0x000000, 0x8300FF, 0x8300FF, 0x008F07, 0x8300FF, 0x43008A, 0x000000, 0x000000,
    0x000000, 0x8300FF, 0x43008A, 0x8300FF, 0x43008A, 0x8300FF, 0x43008A, 0x000000,
    0x000000, 0x000000, 0x8300FF, 0x8300FF, 0x8300FF, 0x8300FF, 0x43008A, 0x000000,
    0x000000, 0x000000, 0x8300FF, 0x43008A, 0x8300FF, 0x43008A, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x8300FF, 0x8300FF, 0x43008A, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x8300FF, 0x43008A, 0x000000, 0x000000, 0x000000};

static const uint32_t symbolCrown[64] = {
    0x000000, 0x000000, 0x000000, 0xDD9900, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0xDD9900, 0xD10000, 0xDD9900, 0x000000, 0x000000, 0x000000,
    0xDD9900, 0x000000, 0x000000, 0xFFDD00, 0x000000, 0x000000, 0xDD9900, 0x000000,
    0xDD9900, 0xDD9900, 0x000000, 0xFFDD00, 0x000000, 0xDD9900, 0xDD9900, 0x000000,
    0xDD9900, 0xFFDD00, 0xDD9900, 0xFFDD00, 0xDD9900, 0xFFDD00, 0xDD9900, 0x000000,
    0xDD9900, 0xD10000, 0xFFDD00, 0xFFDD00, 0xFFDD00, 0xD10000, 0xDD9900, 0x000000,
    0xAA6600, 0xFFDD00, 0xFFDD00, 0xD10000, 0xFFDD00, 0xFFDD00, 0xAA6600, 0x000000,
    0xAA6600, 0xAA6600, 0xDD9900, 0xDD9900, 0xDD9900, 0xAA6600, 0xAA6600, 0x000000};

static const uint32_t symbolBar[64] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0xEEEEEE, 0xEEEEEE, 0xEEEEEE, 0xEEEEEE, 0xEEEEEE, 0xEEEEEE, 0x000000,
    0xEEEEEE, 0xAAAAAA, 0x222222, 0xAAAAAA, 0xAAAAAA, 0x222222, 0xAAAAAA, 0x777777,
    0xEEEEEE, 0x222222, 0xAAAAAA, 0xAAAAAA, 0x222222, 0xAAAAAA, 0xAAAAAA, 0x777777,
    0xEEEEEE, 0xAAAAAA, 0xAAAAAA, 0x222222, 0xAAAAAA, 0xAAAAAA, 0x222222, 0x777777,
    0xEEEEEE, 0xAAAAAA, 0x222222, 0xAAAAAA, 0xAAAAAA, 0x222222, 0xAAAAAA, 0x777777,
    0x000000, 0x777777, 0x777777, 0x777777, 0x777777, 0x777777, 0x777777, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

static const uint32_t symbolWatermelon[64] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x15FF00, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0x15FF00,
    0x15FF00, 0xFF0000, 0x3D0000, 0xFF0000, 0xFF0000, 0xFF0000, 0x3D0000, 0x15FF00,
    0x15FF00, 0xFF0000, 0xFF0000, 0xFF0000, 0x3D0000, 0xFF0000, 0xFF0000, 0x15FF00,
    0x000000, 0x15FF00, 0xFF0000, 0x3D0000, 0xFF0000, 0xFF0000, 0x15FF00, 0x000000,
    0x000000, 0x000000, 0x15FF00, 0x15FF00, 0x15FF00, 0x15FF00, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

static const uint32_t symbolLemon[64] = {
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0x000000,
    0x000000,
    0x000000,
    0xFFFF00,
    0xFFFFFF,
    0xFFFFFF,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0x000000,
    0xFFFF00,
    0xFFFFFF,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xDDBB00,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xDDBB00,
    0xDDBB00,
    0x000000,
    0xFFFF00,
    0xFFFF00,
    0xFFFF00,
    0xDDBB00,
    0xDDBB00,
    0xDDBB00,
    0x000000,
    0x000000,
    0x000000,
    0xDDBB00,
    0xDDBB00,
    0xDDBB00,
    0xDDBB00,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
    0x000000,
};

static const uint32_t *allSymbols[NUM_SYMBOLS] = {
    symbolCherry,
    symbolLemon,
    symbolBar,
    symbolSeven,
    symbolDiamond,
    symbolCrown,
    symbolGrape,
    symbolWatermelon};

SlotMachine_ &SlotMachine_::getInstance()
{
    static SlotMachine_ instance;
    return instance;
}

SlotMachine_ &SlotMachine = SlotMachine.getInstance();

void SlotMachine_::setup()
{
    currentState = SLOT_IDLE;
    for (int i = 0; i < NUM_REELS; i++)
    {
        reelSymbols[i] = random(0, NUM_SYMBOLS);
        reelNextSymbols[i] = random(0, NUM_SYMBOLS);
        reelOffsets[i] = 0;
        reelSpeeds[i] = 0;
        spinEndTime[i] = 0;
        reelShouldStop[i] = false;
        reelIsStopping[i] = false;
    }
}

void SlotMachine_::tick()
{
    switch (currentState)
    {
    case SLOT_IDLE:

        break;

    case SLOT_SPINNING:
    {
        unsigned long now = millis();

        for (int i = 0; i < NUM_REELS; i++)
        {
            if (!reelShouldStop[i] && now >= spinEndTime[i])
            {
                reelShouldStop[i] = true;
                reelSpeeds[i] *= 0.28f;
            }

            if (!reelShouldStop[i])
            {
                reelOffsets[i] += reelSpeeds[i];
                if (reelOffsets[i] >= REEL_HEIGHT)
                {
                    reelOffsets[i] -= REEL_HEIGHT;
                    reelSymbols[i] = reelNextSymbols[i];
                    reelNextSymbols[i] = random(0, NUM_SYMBOLS);
                }
            }
            else
            {
                reelOffsets[i] += reelSpeeds[i];

                if (reelOffsets[i] >= REEL_HEIGHT)
                {
                    reelOffsets[i] -= REEL_HEIGHT;
                    reelSymbols[i] = reelNextSymbols[i];
                    reelNextSymbols[i] = random(0, NUM_SYMBOLS);
                    reelOffsets[i] = 0;
                    reelSpeeds[i] = 0;
                }
            }
        }
        if (allReelsStopped())
        {
            currentState = SLOT_RESULT;
        }
    }
    break;
    case SLOT_RESULT:
    {
        bool isWin = checkWin();
        if (isWin)
        {
            delay(500);
            currentState = SLOT_BLINKWIN;
            blinkCount = 0;
            drawWinningSymbols = false;
            nextBlinkTime = millis() + 200;
        }
        else
        {
            currentState = SLOT_IDLE;
        }
    }
    break;

    case SLOT_BLINKWIN:
    {

        unsigned long now = millis();
        if (now >= nextBlinkTime)
        {
            drawWinningSymbols = !drawWinningSymbols;
            blinkCount++;

            if (blinkCount >= 6)
            {
                currentState = SLOT_IDLE;
            }
            else
            {
                nextBlinkTime = now + 200;
            }
        }
    }
    break;
    }

    drawSlots();
}

void SlotMachine_::selectPressed()
{
    if (currentState == SLOT_IDLE)
    {
        startSpin();
    }
}

void SlotMachine_::ControllerInput(const char *cmd)
{
    if (currentState == SLOT_IDLE)
    {
        startSpin();
    }
}

void SlotMachine_::startSpin()
{
    Serial.println("Start Spin");
    currentState = SLOT_SPINNING;
    unsigned long now = millis();
    for (int i = 0; i < NUM_REELS; i++)
    {
        reelShouldStop[i] = false;
        reelIsStopping[i] = false;
        reelStartTime[i] = now;
        spinEndTime[i] = now + 1000 + random(500, 5000);
        float speed = 0.4f + (float)random(0, 2);
        reelStartSpeed[i] = speed;
        reelSpeeds[i] = speed;
    }
}

void SlotMachine_::stopReel(int reelIndex)
{
    spinEndTime[reelIndex] = millis();
}

bool SlotMachine_::allReelsStopped()
{
    for (int i = 0; i < NUM_REELS; i++)
    {
        if (reelSpeeds[i] != 0)
            return false;
    }
    return true;
}

void SlotMachine_::drawSlots()
{
    DisplayManager.drawFilledRect(0, 0, 32, REEL_HEIGHT, 0);
    for (int i = 0; i < NUM_REELS; i++)
    {
        drawReel(i);
    }
}

void SlotMachine_::drawReel(int reelIndex)
{
    if (currentState == SLOT_BLINKWIN && winningReels[reelIndex] && !drawWinningSymbols)
    {
        return;
    }

    int startX = LEFT_MARGIN + reelIndex * (REEL_WIDTH + GAP_BETWEEN_REELS);
    int startY = 0;
    int offsetY = (int)reelOffsets[reelIndex];
    const uint32_t *currentSymbol = allSymbols[reelSymbols[reelIndex]];
    DisplayManager.drawRGBBitmap(startX, startY + offsetY, currentSymbol, REEL_WIDTH, REEL_HEIGHT);

    if (offsetY > 0)
    {
        const uint32_t *nextSymbol = allSymbols[reelNextSymbols[reelIndex]];
        DisplayManager.drawRGBBitmap(
            startX,
            (startY + offsetY) - REEL_HEIGHT,
            nextSymbol,
            REEL_WIDTH,
            REEL_HEIGHT);
    }
}

bool SlotMachine_::checkWin()
{
    resetWinningReels();

    if (reelSymbols[0] == reelSymbols[1] && reelSymbols[1] == reelSymbols[2])
    {
        winningReels[0] = winningReels[1] = winningReels[2] = true;

        PeripheryManager.playRTTTLString("jackpot:d=8,o=5,b=120:16c,16e,16g,c6,16p,16c6,16e6,4g6");

        switch (reelSymbols[0])
        {
        case 3:                          // Seven
            GameManager.sendPoints(100); // Jackpot
            break;
        case 4: // Diamond
            GameManager.sendPoints(75);
            break;
        case 5: // Crown
            GameManager.sendPoints(50);
            break;
        default:
            GameManager.sendPoints(25);
            break;
        }
        return true;
    }

    if (reelSymbols[0] == reelSymbols[1])
    {
        winningReels[0] = winningReels[1] = true;
        points += 10;
        PeripheryManager.playRTTTLString("two_match:d=8,o=5,b=140:16c6,16e6,4g6");
    }
    if (reelSymbols[1] == reelSymbols[2])
    {
        winningReels[1] = winningReels[2] = true;
        points += 10;
        PeripheryManager.playRTTTLString("two_match:d=8,o=5,b=140:16c6,16e6,4g6");
    }
    if (reelSymbols[0] == reelSymbols[2])
    {
        winningReels[0] = winningReels[2] = true;
        points += 10;
        PeripheryManager.playRTTTLString("two_match:d=8,o=5,b=140:16c6,16e6,4g6");
    }

    if (points > 0)
    {
        GameManager.sendPoints(points);
        return true;
    }

    return false;
}

void SlotMachine_::resetWinningReels()
{
    for (int i = 0; i < NUM_REELS; i++)
    {
        winningReels[i] = false;
    }
}