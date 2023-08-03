#include "effects.h"

const CRGBPalette16 palette = RainbowColors_p;
CRGB tempLeds[32][8];

void Pacifica(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t sPacificaTime = 0;
    sPacificaTime += settings->speed;
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            uint16_t ulx, uly;

            // Richtung berücksichtigen

            ulx = (sPacificaTime / 8) - (i * 16); // Für UP und DOWN, verändert sich die x-Position
            uly = (sPacificaTime / 4) + (j * 16);

            uint16_t v = 0;
            v += sin16(ulx * 6 + sPacificaTime / 2) / 8 + 127;
            v += sin16(uly * 9 + sPacificaTime / 2) / 8 + 127;
            v += sin16(ulx * 7 + uly * 2 - sPacificaTime) / 16;
            v = v / 3;
            CRGB color = ColorFromPalette(settings->palette, v, 255, settings->blend ? LINEARBLEND : NOBLEND);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

void TheaterChase(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint16_t j = 0;
    static uint32_t lastUpdate = 0;
    // Hier verwenden wir settings->speed um die Geschwindigkeit der Animation zu steuern.
    // Die Geschwindigkeitseinstellung wird verwendet, um die Anzahl der Animationsschritte zu steuern, die übersprungen werden.
    if (millis() - lastUpdate > 100 - settings->speed * 10)
    {
        lastUpdate = millis();
        j += 1;
    }

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t k = 0; k < rows; k++)
        {
            if ((i + j) % 3 == 0)
            {
                uint8_t colorIndex = (i * 256 / cols) & 255;
                CRGB color = ColorFromPalette(settings->palette, colorIndex, 255, settings->blend ? LINEARBLEND : NOBLEND);
                matrix->drawPixel(x + i, y + k, color);
            }
            else
            {
                matrix->drawPixel(x + i, y + k, matrix->Color(0, 0, 0));
            }
        }
    }
}

void Plasma(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static double time = 0;

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            uint8_t value = sin8(i * 10 + time) + sin8(j * 10 + time / 2) + sin8((i + j) * 10 + time / 3) / 3;
            CRGB color = ColorFromPalette(settings->palette, value, 255, settings->blend ? LINEARBLEND : NOBLEND);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
    time += settings->speed;
}

struct MatrixColumn
{
    int16_t position; // Y-Position des "fallenden" Pixels
    CRGB color;       // Farbe des Pixels
};

void Matrix(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t lastMove = 0;

    // Retrieve colors from the palette
    static CRGB spawnColor = CRGB(175, 255, 175);
    static CRGB trailColor = CRGB(27, 130, 39);

    static uint8_t intensity = 8; // Adjust to manage the frequency of new "drops"

    // Create a static matrix to hold the state of each pixel
    static CRGB ledState[32][8];

    uint8_t baseSpeed = 180;                            // Base value for speed calculation
    uint8_t speed = baseSpeed - (settings->speed * 15); // Adjust speed based on settings

    uint8_t fadeSpeed = 100;                        // Value for fade calculation
    uint8_t fade = map(fadeSpeed, 0, 255, 50, 250); // equals trail size

    // Update animation based on speed
    if (millis() - lastMove >= speed)
    {
        lastMove = millis();

        // Shift rows down and update the ledState array
        for (uint16_t i = 0; i < cols; i++)
        {
            for (uint16_t j = rows - 1; j > 0; j--)
            {
                ledState[i][j] = ledState[i][j - 1];
            }
        }

        // Fade top row and spawn new pixels
        for (uint16_t i = 0; i < cols; i++)
        {
            if (ledState[i][0] == spawnColor)
            {
                ledState[i][0] = trailColor;
            }
            else
            {
                ledState[i][0].fadeToBlackBy(fade);
            }

            // Randomly spawn new pixels
            if (random8() < intensity)
            {
                ledState[i][0] = spawnColor;
            }
        }
    }

    // Always draw the current state
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            matrix->drawPixel(x + i, y + j, ledState[i][j]);
        }
    }
}

void SwirlIn(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    static uint32_t lastUpdate = 0;
    static uint16_t angle = 0;
    if (millis() - lastUpdate > 100 - settings->speed * 10)
    {
        lastUpdate = millis();
        angle += 4;
    }

    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            float xDiff = 16 - i;
            float yDiff = 4 - j;
            uint16_t dist = sqrt(xDiff * xDiff + yDiff * yDiff);
            uint8_t hue = map(dist, 0, sqrt(16 * 16 + 4 * 4), 0, 255) + angle;
            CRGB color = ColorFromPalette(settings->palette, hue, 255, settings->blend ? LINEARBLEND : NOBLEND);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

void SwirlOut(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    static uint32_t lastUpdate = 0;
    static uint16_t angle = 0;

    if (millis() - lastUpdate > 100 - settings->speed * 10)
    {
        lastUpdate = millis();
        angle += 4;
    }
    float centerX = 16.0;
    float centerY = 4.0;
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            float xDiff = centerX - i;
            float yDiff = centerY - j;
            uint16_t dist = sqrt(xDiff * xDiff + yDiff * yDiff);
            uint8_t hue = 255 - map(dist, 0, sqrt(centerX * centerX + centerY * centerY), 0, 255) + angle;
            CRGB color = ColorFromPalette(settings->palette, hue, 255, settings->blend ? LINEARBLEND : NOBLEND);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

void ColorWaves(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    // Map palette colors to 0-255
    float colorIndex = 255 / (32 - 1);
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            // Calculate index into palette using column position, time, and speed
            uint8_t paletteIndex = ((uint8_t)(i * colorIndex + millis() * settings->speed / 100)) % 256;
            // Get color from palette
            CRGB color = ColorFromPalette(settings->palette, paletteIndex, 255, settings->blend ? LINEARBLEND : NOBLEND);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

#define MAX_BRIGHTNESS 255
#define FADE_RATE 0.02

struct Star
{
    CRGB color;
    float brightness;
};

Star stars[32][8]; // Create a buffer to store the state of the LEDs

void TwinklingStars(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t lastUpdate = 0;

    // Fade all LEDs each frame
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            stars[i][j].brightness -= 0.01;
            if (stars[i][j].brightness < 0)
                stars[i][j].brightness = 0;
            matrix->drawPixel(x + i, y + j, stars[i][j].color.nscale8_video(stars[i][j].brightness * 255));
        }
    }

    // Randomly twinkle new stars each frame
    if (millis() - lastUpdate > (uint32_t)1000 / settings->speed)
    {
        lastUpdate = millis();
        uint8_t numStars = random(1, 5); // Create between 1-5 new stars per frame
        for (uint8_t i = 0; i < numStars; i++)
        {
            uint16_t starX = random(cols);
            uint16_t starY = random(rows);
            // Star color - varying the saturation and value for shades of blue and white
            stars[starX][starY].color = ColorFromPalette(settings->palette, random8(), 255, settings->blend ? LINEARBLEND : NOBLEND);
            stars[starX][starY].brightness = 1.0;
            matrix->drawPixel(x + starX, y + starY, stars[starX][starY].color);
        }
    }
}

// ######## LookingEyes ############

int blinkIndex[] = {1, 2, 3, 4, 3, 2, 1};
int blinkCountdown = 60;
int gazeCountdown = 30;
int gazeFrames = 5;
int eyeX = 9;
int eyeY = 3;
int newX = 9;
int newY = 3;
int dX = 6;
int dY = 0;
int PET_MOOD = 0;

uint16_t eye[5][64] = {
    {0, 0, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

void LookingEyes(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    if (blinkCountdown < sizeof(blinkIndex) / sizeof(blinkIndex[0]) - 1)
    {
        matrix->drawRGBBitmap(6 + x, 0 + y, eye[blinkIndex[blinkCountdown]], 8, 8);
        matrix->drawRGBBitmap(18 + x, 0 + y, eye[blinkIndex[blinkCountdown]], 8, 8);
    }
    else
    {
        matrix->drawRGBBitmap(6 + x, 0 + y, eye[0], 8, 8);
        matrix->drawRGBBitmap(18 + x, 0 + y, eye[0], 8, 8);
    }

    blinkCountdown = blinkCountdown - 0.1;
    if (blinkCountdown == 0)
    {
        blinkCountdown = random(60, 350);
    }

    if (gazeCountdown <= gazeFrames)
    {
        gazeCountdown -= 0.5;
        matrix->fillRect(newX - (dX * gazeCountdown / gazeFrames) + 6 + x, newY - (dY * gazeCountdown / gazeFrames) + y, 2, 2, 0);
        matrix->fillRect(newX - (dX * gazeCountdown / gazeFrames) + 18 + x, newY - (dY * gazeCountdown / gazeFrames) + y, 2, 2, 0);
        if (gazeCountdown == 0)
        {
            eyeX = newX;
            eyeY = newY;
            do
            {
                switch (PET_MOOD)
                {
                case 0:
                    newX = random(0, 6);
                    newY = random(0, 6);
                    dX = newX - 4;
                    dY = newY - 4;
                    break;
                case 1:
                    newX = random(0, 7);
                    newY = random(0, 7);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                case 2:
                    newX = random(1, 7);
                    newY = random(1, 4);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                case 3:
                    newX = random(0, 7);
                    newY = random(3, 7);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                }
            } while (((dX * dX + dY * dY) <= 3));
            dX = newX - eyeX;
            dY = newY - eyeY;
            gazeFrames = random(15, 40);
            gazeCountdown = random(gazeFrames, 120);
        }
    }
    else
    {
        gazeCountdown -= 0.5;
        matrix->fillRect(eyeX + 6 + x, eyeY + y, 2, 2, 0);
        matrix->fillRect(eyeX + 18 + x, eyeY + y, 2, 2, 0);
    }
}

// ######## Snake ############
#define MAX_SNAKE_LENGTH 64
#define INITIAL_SNAKE_LENGTH 2
#define SNAKE_SPEED 60

struct Point
{
    int16_t x, y;
    uint8_t colorIndex; // add a color index to each segment
};

Point snake[MAX_SNAKE_LENGTH];
bool shouldMove[MAX_SNAKE_LENGTH]; // new array to track whether each segment should move
uint8_t snakeLength = INITIAL_SNAKE_LENGTH;
uint8_t snakeDirection = 0; // 0=up, 1=right, 2=down, 3=left
uint32_t lastUpdate = 0;
bool isGameOver = false;
Point apple;
uint8_t colorIndex = 0;

bool willCollide(int16_t x, int16_t y)
{
    // Check if the given position will collide with the snake's body
    for (uint8_t i = 1; i < snakeLength; i++)
    {
        if (x == snake[i].x && y == snake[i].y)
        {
            return true;
        }
    }
    return false;
}

void updateDirection()
{
    // Simple strategy to move towards apple
    if (snakeDirection % 2 == 0)
    { // currently moving vertically
        // Only change direction if snake is horizontally aligned with the apple
        if (apple.y == snake[0].y)
        {
            if (apple.x > snake[0].x && snakeDirection != 3)
            {                       // prevent moving left if currently moving right
                snakeDirection = 1; // move right
            }
            else if (apple.x < snake[0].x && snakeDirection != 1)
            {                       // prevent moving right if currently moving left
                snakeDirection = 3; // move left
            }
        }
    }
    else
    { // currently moving horizontally
        // Only change direction if snake is vertically aligned with the apple
        if (apple.x == snake[0].x)
        {
            if (apple.y > snake[0].y && snakeDirection != 0)
            {                       // prevent moving up if currently moving down
                snakeDirection = 2; // move down
            }
            else if (apple.y < snake[0].y && snakeDirection != 2)
            {                       // prevent moving down if currently moving up
                snakeDirection = 0; // move up
            }
        }
    }
    // Predict next position of snake's head
    Point nextPos = snake[0];
    if (snakeDirection == 0)
    {
        nextPos.y--;
    }
    else if (snakeDirection == 1)
    {
        nextPos.x++;
    }
    else if (snakeDirection == 2)
    {
        nextPos.y++;
    }
    else if (snakeDirection == 3)
    {
        nextPos.x--;
    }
    // Check if next position will collide with snake's body
    for (uint8_t i = 1; i < snakeLength; i++)
    {
        if (nextPos.x == snake[i].x && nextPos.y == snake[i].y)
        {
            // If snake is moving vertically, try to turn left or right
            if (snakeDirection % 2 == 0)
            {
                if (snakeDirection != 1 && snake[0].x + 1 < 32 && !willCollide(snake[0].x + 1, snake[0].y))
                {
                    snakeDirection = 1; // turn right
                }
                else if (snakeDirection != 3 && snake[0].x - 1 >= 0 && !willCollide(snake[0].x - 1, snake[0].y))
                {
                    snakeDirection = 3; // turn left
                }
            }
            else
            { // If snake is moving horizontally, try to turn up or down
                if (snakeDirection != 0 && snake[0].y + 1 < 8 && !willCollide(snake[0].x, snake[0].y + 1))
                {
                    snakeDirection = 2; // turn down
                }
                else if (snakeDirection != 2 && snake[0].y - 1 >= 0 && !willCollide(snake[0].x, snake[0].y - 1))
                {
                    snakeDirection = 0; // turn up
                }
            }
        }
    }
}

void SnakeGame(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    if (isGameOver)
    {
        for (uint8_t i = 0; i < snakeLength; i++)
        {
            snake[i] = {0, i};
            shouldMove[i] = true; // all segments should move initially
        }
        snakeDirection = 1; // start moving right
        isGameOver = false;
        snakeLength = INITIAL_SNAKE_LENGTH;
        apple = {(int16_t)random(32), (int16_t)random(8)}; // spawn apple at random position
                                                           // Initialize color index for each segment
        for (uint8_t i = 0; i < snakeLength; i++)
        {
            snake[i].colorIndex = colorIndex;
        }
    }
    if (millis() - lastUpdate > 150 - settings->speed * 10)
    {
        lastUpdate = millis();
        // Update direction
        updateDirection();
        if (snake[0].x == apple.x && snake[0].y == apple.y)
        {
            if (snakeLength < MAX_SNAKE_LENGTH)
            {
                snake[snakeLength] = apple;      // add new tail segment
                shouldMove[snakeLength] = false; // new segment should not move initially
                snakeLength++;
            }
            apple = {(int16_t)random(32), (int16_t)random(8)}; // spawn new apple
        }
        // Move snake
        for (uint8_t i = snakeLength - 1; i > 0; i--)
        {
            if (shouldMove[i])
            {
                snake[i] = snake[i - 1];
            }
            else
            {
                shouldMove[i] = true; // new segment should start moving from next frame
            }
        }
        // Update head position based on direction
        if (snakeDirection == 0)
        {
            snake[0].y--;
        }
        else if (snakeDirection == 1)
        {
            snake[0].x++;
        }
        else if (snakeDirection == 2)
        {
            snake[0].y++;
        }
        else if (snakeDirection == 3)
        {
            snake[0].x--;
        }

        // Check for wall collision and wrap around
        if (snake[0].x < 0)
        {
            snake[0].x = cols - 1;
            colorIndex = (colorIndex + 10) % 255;
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }
        else if (snake[0].x >= cols)
        {
            snake[0].x = 0;
            colorIndex = (colorIndex + 10) % 255;
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }
        else if (snake[0].y < 0)
        {
            snake[0].y = rows - 1;
            colorIndex = (colorIndex + 10) % 255;
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }
        else if (snake[0].y >= rows)
        {
            snake[0].y = 0;
            colorIndex = (colorIndex + 10) % 255;
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }

        // Check for self-collision
        for (uint8_t i = 1; i < snakeLength; i++)
        {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            {
                isGameOver = true;
            }
        }
    }
    // Draw the snake
    for (uint8_t i = 0; i < snakeLength; i++)
    {
        CRGB color = ColorFromPalette(settings->palette, snake[i].colorIndex, 255, settings->blend ? LINEARBLEND : NOBLEND);
        matrix->drawPixel(x + snake[i].x, y + snake[i].y, color);
    }
    // Draw the apple
    matrix->drawPixel(x + apple.x, y + apple.y, matrix->Color(255, 0, 0));
}

// ######## Fireworks ############
struct Firework
{
    float x, y;
    uint8_t life;
    bool exploded;
    CRGB color;
    uint16_t peak;
    float speed; // Add a speed variable to each firework
};

#define MAX_FIREWORKS 5
Firework fireworks[MAX_FIREWORKS];
uint32_t lastFireworkTime = 0;
uint32_t fireworkInterval = 350; // milliseconds between new fireworks

void Fireworks(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    // Spawn new firework if enough time has passed
    if (millis() - lastFireworkTime >= 1000 / fireworkInterval && random(100) < 50)
    { // 50% chance to spawn firework
        for (uint8_t i = 0; i < MAX_FIREWORKS; i++)
        {
            if (fireworks[i].life == 0)
            { // find an unused firework
                fireworks[i].x = random(cols);
                fireworks[i].y = rows - 1;
                fireworks[i].life = 255;
                fireworks[i].exploded = false;
                fireworks[i].color = CRGB::White;           // The unexploded firework is white
                fireworks[i].peak = random(1, 5);           // Set a random peak height for the firework
                fireworks[i].speed = settings->speed * 0.3; // Set a speed for the firework
                lastFireworkTime = millis();
                break;
            }
        }
    }

    // Move, explode and fade out fireworks
    for (uint8_t i = 0; i < MAX_FIREWORKS; i++)
    {
        if (fireworks[i].life > 0)
        {
            fireworks[i].life -= 5 * settings->speed; // decrease life by 5 each frame
            if (fireworks[i].y > fireworks[i].peak && !fireworks[i].exploded)
            {
                fireworks[i].y -= fireworks[i].speed; // Move the firework up by its speed
            }
            else
            {
                fireworks[i].exploded = true;
                fireworks[i].color = ColorFromPalette(settings->palette, random8(), 255, settings->blend ? LINEARBLEND : NOBLEND); // The exploded firework has a random color
            }
            matrix->drawPixel(x + fireworks[i].x, y + fireworks[i].y, fireworks[i].color);
            // Draw a few pixels around the firework to simulate explosion
            if (fireworks[i].exploded)
            {
                if (fireworks[i].x > 0)
                    matrix->drawPixel(x + fireworks[i].x - 1, y + fireworks[i].y, fireworks[i].color);
                if (fireworks[i].x < cols - 1)
                    matrix->drawPixel(x + fireworks[i].x + 1, y + fireworks[i].y, fireworks[i].color);
                if (fireworks[i].y > 0)
                    matrix->drawPixel(x + fireworks[i].x, y + fireworks[i].y - 1, fireworks[i].color);
                if (fireworks[i].y < rows - 1)
                    matrix->drawPixel(x + fireworks[i].x, y + fireworks[i].y + 1, fireworks[i].color);
            }
        }
    }
}

// ######## Ripple ############

#define RIPPLE_SIZE 30

struct Ripple
{
    uint16_t x, y;
    float life;
    CRGB color;
};

Ripple ripple;

void RippleEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    // If ripple has finished, spawn a new one at a random location
    if (ripple.life > RIPPLE_SIZE)
    {
        ripple.x = random(32);
        ripple.y = random(8);
        ripple.life = 0;
        ripple.color = ColorFromPalette(settings->palette, random(255), 255, settings->blend ? LINEARBLEND : NOBLEND); // random color with full saturation and value
    }

    // Apply pseudo-blur
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            tempLeds[i][j].fadeToBlackBy(45);
        }
    }

    // Draw the ripple
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            // Calculate distance from the ripple center
            uint16_t dx = abs(i - ripple.x);
            uint16_t dy = abs(j - ripple.y);
            uint16_t dist = sqrt(dx * dx + dy * dy);

            // If this pixel is on the ripple ring, color it
            if (dist >= ripple.life && dist < ripple.life + 2)
            {
                tempLeds[i][j] = ripple.color;
            }

            // Apply the temporary LEDs to the actual matrix
            matrix->drawPixel(x + i, y + j, tempLeds[i][j]);
        }
    }

    // Increase the ripple size
    ripple.life += settings->speed * 0.5;
}

// ######## PlasmaCloud ############

void PlasmaCloudEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    static float plasmaTime = 0;
    static float hueShift = 0;
    // For each pixel, calculate a noise value based on its position and the current time
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            float noise = inoise8(i * 16, j * 16, plasmaTime);
            uint8_t hue = noise * 255 / 1024 + hueShift; // Map noise value to hue (0-255) and add hueShift
            CRGB color = ColorFromPalette(settings->palette, hue, 255, settings->blend ? LINEARBLEND : NOBLEND);
            matrix->drawPixel(x + i, y + j, color);
        }
    }

    // Increase time to animate the plasma clouds
    plasmaTime += settings->speed;

    // Increase hueShift to gradually change color over time
    hueShift += settings->speed / 5;
}

void CheckerboardEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    static float colorIndex1 = 0;
    static float colorIndex2 = 0;
    CRGB color1 = ColorFromPalette(settings->palette, colorIndex1, 255, settings->blend ? LINEARBLEND : NOBLEND);
    CRGB color2 = ColorFromPalette(settings->palette, colorIndex2, 255, settings->blend ? LINEARBLEND : NOBLEND);

    // For each pixel, check if its x and y coordinates are both even or both odd
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            if ((i % 2 == 0 && j % 2 == 0) || (i % 2 != 0 && j % 2 != 0))
            {
                matrix->drawPixel(x + i, y + j, color1);
            }
            else
            {
                matrix->drawPixel(x + i, y + j, color2);
            }
        }
    }

    // Increase colorIndex to change colors over time
    colorIndex1 += settings->speed * 0.6;
    colorIndex2 += settings->speed;
}

void RadarEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    static float beamAngle = 0;
    // Fade the previous state of the leds
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            tempLeds[i][j].fadeToBlackBy(20);
        }
    }

    // Draw the beam
    for (uint8_t r = 0; r <= max(32, 8); r++)
    {
        uint16_t i = 16 + r * (cos8(beamAngle) - 128) / 128.0;
        uint16_t j = 4 - r * (sin8(beamAngle) - 128) / 128.0;
        if (i < 32 && j < 8)
        {
            tempLeds[i][j] = ColorFromPalette(settings->palette, beamAngle, 255, settings->blend ? LINEARBLEND : NOBLEND);
        }
    }

    // Copy the temporary leds array to the actual matrix
    for (uint16_t i = 0; i < 32; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            matrix->drawPixel(x + i, y + j, tempLeds[i][j]);
        }
    }

    // Increment the beam angle
    beamAngle += settings->speed;
}

// ######## Balls ############
#define PADDLE_HEIGHT 3 // Height of the paddles
#define BALL_SIZE 1     // Size of the ball

struct Paddle
{
    int y;  // Y position of the paddle
    int dy; // Speed of the paddle
};

struct Ball
{
    int x, y;   // Position of the ball
    int dx, dy; // Speed of the ball
};

void PingPongEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    static Paddle paddle1 = {rows / 2, 1};         // Paddle on the left side
    static Paddle paddle2 = {rows / 2, -1};        // Paddle on the right side
    static Ball ball = {cols / 2, rows / 2, 1, 1}; // Ball in the middle of the matrix
    static uint32_t lastUpdate = 0;
    // Move the paddles

    if (millis() - lastUpdate > settings->speed * 100)
    {
        lastUpdate = millis();
        paddle1.y += paddle1.dy;
        paddle2.y += paddle2.dy;

        // If a paddle has reached the top or bottom edge, change its direction
        if (paddle1.y <= 0 || paddle1.y + PADDLE_HEIGHT >= rows)
        {
            paddle1.dy = -paddle1.dy;
        }
        if (paddle2.y <= 0 || paddle2.y + PADDLE_HEIGHT >= rows)
        {
            paddle2.dy = -paddle2.dy;
        }

        // Move the ball
        ball.x += ball.dx;
        ball.y += ball.dy;

        // If the ball has reached the top or bottom edge, change its direction
        if (ball.y <= 0 || ball.y + BALL_SIZE >= rows)
        {
            ball.dy = -ball.dy;
        }

        // If the ball has reached the left or right edge without hitting a paddle, restart the game
        if ((ball.x < 0 && (ball.y < paddle1.y || ball.y > paddle1.y + PADDLE_HEIGHT)) ||
            (ball.x + BALL_SIZE > cols && (ball.y < paddle2.y || ball.y > paddle2.y + PADDLE_HEIGHT)))
        {
            ball.x = cols / 2; // Reset ball position
            ball.y = rows / 2;
            ball.dx = 1; // Reset ball direction
            ball.dy = 1;
        }

        // If the ball has reached a paddle, change its direction
        if (ball.x <= 0 && ball.y >= paddle1.y && ball.y < paddle1.y + PADDLE_HEIGHT)
        {
            ball.dx = -ball.dx;
        }
        if (ball.x + BALL_SIZE >= cols && ball.y >= paddle2.y && ball.y < paddle2.y + PADDLE_HEIGHT)
        {
            ball.dx = -ball.dx;
        }
    }
    // Draw the paddles and the ball
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        matrix->drawPixel(x, y + paddle1.y + i, matrix->Color(255, 255, 255));
        matrix->drawPixel(x + cols - 1, y + paddle2.y + i, matrix->Color(255, 255, 255));
    }
    for (int i = 0; i < BALL_SIZE; i++)
    {
        matrix->drawPixel(x + ball.x, y + ball.y + i, matrix->Color(255, 0, 0));
    }
}

// ######## Bricks ############
#define PADDLE_WIDTH 3 // Width of the paddle
#define BALL_SIZE 1    // Size of the ball
#define BRICK_COLS 16  // Number of bricks in a row
#define BRICK_ROWS 3   // Number of brick rows

struct BricksPaddle
{
    int x;  // X position of the paddle
    int dx; // Speed of the paddle
};

struct BricksBall
{
    int x, y;   // Position of the ball
    int dx, dy; // Speed of the ball
};

bool bricks[BRICK_ROWS][BRICK_COLS]; // State of the bricks

void resetBricks()
{
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            bricks[i][j] = true;
        }
    }
}

bool bricksRemain()
{
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            if (bricks[i][j])
            {
                return true;
            }
        }
    }
    return false;
}

void BrickBreakerEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    static BricksPaddle paddle = {cols / 2, 1};          // Paddle in the middle of the matrix
    static BricksBall ball = {cols / 2, rows / 2, 1, 1}; // Ball in the middle of the matrix

    // Initialize the bricks
    static bool firstTime = true;
    if (firstTime)
    {
        for (int i = 0; i < BRICK_ROWS; i++)
        {
            for (int j = 0; j < BRICK_COLS; j++)
            {
                bricks[i][j] = true; // All bricks exist at the beginning
            }
        }
        firstTime = false;
    }

    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate > 100)
    {
        lastUpdate = millis();

        // Move the ball
        ball.x += ball.dx;
        ball.y += ball.dy;

        // Move the paddle
        paddle.x = ball.x - 1;

        // If the paddle has reached the left or right edge, change its direction
        if (paddle.x <= 0 || paddle.x + PADDLE_WIDTH >= cols)
        {
            paddle.dx = -paddle.dx;
        }

        // If the ball has reached the top, bottom, left, or right edge, change its direction
        if (ball.x <= 0 || ball.x + BALL_SIZE >= cols)
        {
            ball.dx = -ball.dx;
        }
        if (ball.y <= 0 || ball.y + BALL_SIZE >= rows)
        {
            ball.dy = -ball.dy;
        }

        for (int i = 0; i < BRICK_ROWS; i++)
        {
            for (int j = 0; j < BRICK_COLS; j++)
            {
                if (bricks[i][j] && ball.y == i && ball.x >= j * (cols / BRICK_COLS) && ball.x < (j + 1) * (cols / BRICK_COLS))
                {
                    bricks[i][j] = false;
                    ball.dy = -ball.dy;
                }
            }
        }

        // If no bricks remain, reset the bricks
        if (!bricksRemain())
        {
            resetBricks();
        }
    }
    // Draw the paddle, the ball, and the bricks
    for (int i = 0; i < PADDLE_WIDTH; i++)
    {
        matrix->drawPixel(x + paddle.x + i, y + rows - 1, matrix->Color(255, 255, 255));
    }
    for (int i = 0; i < BALL_SIZE; i++)
    {
        matrix->drawPixel(x + ball.x, y + ball.y + i, matrix->Color(255, 0, 0));
    }
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            if (bricks[i][j])
            {
                matrix->drawPixel(x + j * (cols / BRICK_COLS), y + i, matrix->Color(0, 0, 255));
            }
        }
    }
}

void MovingLine(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    static int16_t linePosition = 0; // Start position of the line
    static int8_t direction = 1;     // Direction of the line movement

    // Control the speed of the line
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate > 100 - settings->speed * 10)
    {
        lastUpdate = millis();

        // Move the line
        linePosition += direction;
        if (linePosition <= 0 || linePosition + 1 >= 8)
        {
            direction = -direction;
        }
        colorIndex += 1;
    }

    // Draw the line
    for (int16_t i = 0; i < 1; i++)
    {
        for (uint16_t j = 0; j < 32; j++)
        {
            matrix->drawPixel(x + j, y + linePosition + i, ColorFromPalette(settings->palette, colorIndex, 255, settings->blend ? LINEARBLEND : NOBLEND));
        }
    }
}

void Fade(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, EffectSettings *settings)
{
    static uint8_t hue = 0; // Hue for the color of the rows
    // Change the color of the rows based on speed
    hue += settings->speed;
    // Draw the rows with fading colors based on the palette

    for (int16_t i = 8 - 1; i >= 0; i--)
    {
        for (uint16_t j = 0; j < 32; j++)
        {
            CRGB color = ColorFromPalette(settings->palette, hue + (i * 256 / 8), 255, settings->blend ? LINEARBLEND : NOBLEND);
            matrix->drawPixel(x + j, y + i, color);
        }
    }
}

Effect effects[] = {
    {"Fade", Fade, EffectSettings(1, RainbowColors_p, true)},
    {"MovingLine", MovingLine, EffectSettings(1, RainbowColors_p, true)},
    {"BrickBreaker", BrickBreakerEffect, EffectSettings()},
    {"PingPong", PingPongEffect, EffectSettings(8, RainbowColors_p, true)},
    {"Radar", RadarEffect, EffectSettings(1, RainbowColors_p, true)},
    {"Checkerboard", CheckerboardEffect, EffectSettings(1, RainbowColors_p, true)},
    {"Fireworks", Fireworks, EffectSettings(0.8, RainbowColors_p)},
    {"PlasmaCloud", PlasmaCloudEffect, EffectSettings(3, RainbowColors_p, true)},
    {"Ripple", RippleEffect, EffectSettings(3, RainbowColors_p, true)},
    {"Snake", SnakeGame, EffectSettings(3, RainbowColors_p, true)},
    {"Pacifica", Pacifica, EffectSettings(3, OceanColors_p, true)},
    {"TheaterChase", TheaterChase, EffectSettings(3, RainbowColors_p,true)},
    {"Plasma", Plasma, EffectSettings(2, RainbowColors_p, true)},
    {"Matrix", Matrix, EffectSettings(8, ForestColors_p, false)},
    {"SwirlIn", SwirlIn, EffectSettings(4, RainbowColors_p, true)},
    {"SwirlOut", SwirlOut, EffectSettings(4, RainbowColors_p, true)},
    {"LookingEyes", LookingEyes, EffectSettings()},
    {"TwinklingStars", TwinklingStars, EffectSettings(4, OceanColors_p, true)},
    {"ColorWaves", ColorWaves, EffectSettings(3, RainbowColors_p, true)}};

// ######## Helper functions ############

void callEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, u_int8_t index)
{
    if (index > -1)
    {
        effects[index].func(matrix, x, y, &effects[index].settings);
    }
}

int getEffectIndex(String name)
{
    for (uint8_t i = 0; i < sizeof(effects) / sizeof(effects[0]); i++)
    {
        if (effects[i].name == name)
        {
            return i;
        }
    }
    return -1;
}

// Loads a palette from the LittleFS filesystem
CRGBPalette16 loadPaletteFromLittleFS(String paletteName)
{
    Serial.println("Loading palette: " + paletteName);
    File paletteFile = LittleFS.open("/PALETTES/" + paletteName + ".txt", "r");
    if (!paletteFile)
    {
        Serial.println("Failed to open palette file " + paletteName);
        return RainbowColors_p; // default palette
    }

    CRGBPalette16 palette;
    String colorString;
    int i = 0;
    while (paletteFile.available() && i < 16)
    {
        colorString = paletteFile.readStringUntil('\n');
        colorString.trim(); // Remove any whitespace
        uint32_t colorValue = (uint32_t)strtol(colorString.c_str(), NULL, 16);
        palette[i] = CRGB(colorValue);
        i++;
    }
    paletteFile.close();

    Serial.println("Palette loaded: " + paletteName);
    return palette;
}

// Returns a palette based on the given name
CRGBPalette16 getPalette(String palette)
{
    if (palette == "Cloud")
    {
        return CloudColors_p;
    }
    else if (palette == "Lava")
    {
        return LavaColors_p;
    }
    else if (palette == "Ocean")
    {
        return OceanColors_p;
    }
    else if (palette == "Forest")
    {
        return ForestColors_p;
    }
    else if (palette == "Stripe")
    {
        return RainbowStripeColors_p;
    }
    else if (palette == "Party")
    {
        return PartyColors_p;
    }
    else if (palette == "Heat")
    {
        return HeatColors_p;
    }
    else
    {
        return loadPaletteFromLittleFS(palette);
    }
}

void updateEffectSettings(u_int8_t index, String json)
{
    if (index != -1)
    {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, json);

        if (doc.containsKey("speed"))
        {
            effects[index].settings.speed = doc["speed"];
        }

        if (doc.containsKey("palette"))
        {
            effects[index].settings.palette = getPalette(doc["palette"].as<String>());
        }

        if (doc.containsKey("blend"))
        {
            effects[index].settings.blend = doc["blend"].as<bool>();
        }
        doc.clear();
    }
}
