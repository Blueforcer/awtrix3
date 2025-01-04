#include <GameManager.h>
#include "Globals.h"
#include "DisplayManager.h"

int mapWidth;
const int mapHeight = 8;
const float gravity = 0.1;
const float jumpSpeed = 2.0;
bool GameStarted = false;

enum Direction
{
    LEFT,
    RIGHT,
    STOP
};

struct PlayerStats
{
    int x, y;
    bool isJumping;
    float verticalSpeed;
    uint32_t color;
    PlayerStats() : x(0), y(0), isJumping(false), verticalSpeed(0), color(0xFFFFFF) {}
} GamePlayer;

Direction dir = STOP;
bool pressed = false;
bool jumpButtonPressed = false;

const char *levelMap[] = {
    "                                ",
    "#                               ",
    "#  O   O   O      O  O   O  O   ",
    "#         P                     ",
    "####   ####   ####    ####  ####",
    "#  #   #  #   #  #    #  #  #   ",
    "#  #S            #S             ",
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"};

// The getter for the instantiated singleton instance
GameManager_ &GameManager_::getInstance()
{
    static GameManager_ instance;
    return instance;
}

// Initialize the global shared instance
GameManager_ &GameManager = GameManager.getInstance();

void GameManager_::loadLevel()
{
    mapWidth = strlen(levelMap[0]);
    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = 0; x < mapWidth; x++)
        {
            if (levelMap[y][x] == 'P')
            {
                GamePlayer.x = x;
                GamePlayer.y = y;
                return;
            }
        }
    }
    drawVisibleMapSection();
}

void GameManager_::tick()
{
    if (!GameStarted)
        loadLevel();
    if (jumpButtonPressed)
    {
        jump();
    }
    applyGravityAndJump();
    updatePlayerPosition();
    drawVisibleMapSection();
    drawPlayer();
}

void GameManager_::ControllerInput(String key, String status)
{
    Serial.print(key);
    Serial.print(":");
    Serial.print(status);
    Serial.println();
    if (key == "LEFT")
    {
        dir = LEFT;
        pressed = (status == "1");
    }
    else if (key == "RIGHT")
    {
        dir = RIGHT;
        pressed = (status == "1");
    }
    else if (key == "JUMP")
    {
        jumpButtonPressed = (status == "1");
    }
    else
    {
    }
}

void GameManager_::updatePlayerPosition()
{
    if (pressed)
    {
        switch (dir)
        {
        case LEFT:
            GamePlayer.x = max(0, GamePlayer.x - 1);
            break;
        case RIGHT:
            GamePlayer.x = min(mapWidth - 1, GamePlayer.x + 1);
            break;
        default:
            break;
        }
    }
}

void GameManager_::applyGravityAndJump()
{
    if (GamePlayer.isJumping)
    {
        GamePlayer.y += GamePlayer.verticalSpeed;
        GamePlayer.verticalSpeed -= gravity;
        if (GamePlayer.verticalSpeed < 0 && checkCollisionWithObjects(GamePlayer.x, GamePlayer.y))
        {
            GamePlayer.y = floor(GamePlayer.y);
            GamePlayer.isJumping = false;
            GamePlayer.verticalSpeed = 0;
        }
    }
}

void GameManager_::jump()
{
    if (!GamePlayer.isJumping)
    {
        GamePlayer.isJumping = true;
        GamePlayer.verticalSpeed = jumpSpeed;
    }
}

bool GameManager_::checkCollisionWithObjects(int x, int y)
{

    if (y >= mapHeight)
    {
        return false;
    }

    char mapChar = levelMap[y][x];
    switch (mapChar)
    {
    case '#':
    case '+':
        return true;
    default:
        return false;
    }
}

void GameManager_::drawVisibleMapSection()
{
    int startColumn = max(0, GamePlayer.x - 16);
    int endColumn = min(mapWidth, startColumn + 32);

    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = startColumn; x < endColumn; x++)
        {
            char mapChar = levelMap[y][x];
            uint32_t objectColor = 0x0000;

            switch (mapChar)
            {
            case 'G': // Boden
                objectColor = 0xFF0000;
                break;
            case '#': // Block
                objectColor = 0x00FF00;
                break;
            case 'O': // Münze
                objectColor = 0xFFFF00;
                break;
            case 'S': // Spezielles Objekt
                objectColor = 0x0000FF;
                break;
            case '+': // Zerbrechlicher Block
                objectColor = 0xFF00FF;
                break;
            default:
                objectColor = 0x000000;
                break;
            }
            DisplayManager.drawPixel(x - startColumn, y, objectColor);
        }
    }
}

void GameManager_::drawPlayer()
{
    int playerScreenX = GamePlayer.x - max(0, GamePlayer.x - 16);
    DisplayManager.drawPixel(playerScreenX, GamePlayer.y, GamePlayer.color);
}