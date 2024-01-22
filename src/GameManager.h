#ifndef GameManager_h
#define GameManager_h

#include <Arduino.h>

class GameManager_
{
private:
    GameManager_() = default;
public:
    static GameManager_ &getInstance();
    void setup();
    void tick();
    void loadLevel();
    void updatePlayerPosition();
    void drawVisibleMapSection();
    void applyGravityAndJump();
    bool checkCollisionWithObjects(int x, int y);
    void drawPlayer();
    void jump();
    void ControllerInput(String key, String status);
};

extern GameManager_ &GameManager;
#endif
