/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 by Daniel Eichhorn
 * Copyright (c) 2016 by Fabrice Weinberg
 * Copyright (c) 2023 by Stephan Muehl (Blueforcer)
 * Note: This old lib for SSD1306 displays has been extremly
 * modified for AWTRIX Light and has nothing to do with the original purposes.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
int pixelDelays[32][8];
#include "MatrixDisplayUi.h"
#include "AwtrixFont.h"
#include "effects.h"
#include "Globals.h"

GifPlayer gif1;
GifPlayer gif2;

MatrixDisplayUi::MatrixDisplayUi(FastLED_NeoMatrix *matrix)
{
  this->matrix = matrix;
}

void MatrixDisplayUi::init()
{
  this->matrix->begin();
  this->matrix->setTextWrap(false);
  this->matrix->setBrightness(70);
  this->matrix->setFont(&AwtrixFont);
  gif1.setMatrix(this->matrix);
  gif2.setMatrix(this->matrix);

  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      pixelDelays[i][j] = random(256);
    }
  }
}

void MatrixDisplayUi::setTargetFPS(uint8_t fps)
{
  float oldInterval = this->updateInterval;
  this->updateInterval = ((float)1.0 / (float)fps) * 1000;

  // Calculate new ticksPerApp
  float changeRatio = oldInterval / (float)this->updateInterval;
  // this->ticksPerApp *= changeRatio;
  this->ticksPerTransition *= changeRatio;
}

void MatrixDisplayUi::setBackgroundEffect(int effect)
{
  this->BackgroundEffect = effect;
}

// -/------ Automatic controll ------\-

void MatrixDisplayUi::enablesetAutoTransition()
{
  this->setAutoTransition = true;
}
void MatrixDisplayUi::disablesetAutoTransition()
{
  this->setAutoTransition = false;
}
void MatrixDisplayUi::setsetAutoTransitionForwards()
{
  this->state.appTransitionDirection = 1;
  this->lastTransitionDirection = 1;
}
void MatrixDisplayUi::setsetAutoTransitionBackwards()
{
  this->state.appTransitionDirection = -1;
  this->lastTransitionDirection = -1;
}
void MatrixDisplayUi::setTimePerApp(long time)
{
  this->ticksPerApp = time / updateInterval;
}
void MatrixDisplayUi::setTimePerTransition(uint16_t time)
{
  this->ticksPerTransition = (int)((float)time / (float)updateInterval);
}

// -/----- App settings -----\-
void MatrixDisplayUi::setAppAnimation(AnimationDirection dir)
{
  this->appAnimationDirection = dir;
}

void MatrixDisplayUi::setApps(const std::vector<std::pair<String, AppCallback>> &appPairs)
{

  delete[] AppFunctions;
  AppCount = appPairs.size();
  AppFunctions = new AppCallback[AppCount];
  for (size_t i = 0; i < AppCount; ++i)
  {
    AppFunctions[i] = appPairs[i].second;
  }
  this->resetState();
  DisplayManager.sendAppLoop();
}

// -/----- Overlays ------\-
void MatrixDisplayUi::setOverlays(OverlayCallback *overlayFunctions, uint8_t overlayCount)
{
  this->overlayFunctions = overlayFunctions;
  this->overlayCount = overlayCount;
}

void MatrixDisplayUi::setBackground(BackgroundCallback backgroundFunction)
{
  this->backgroundFunction = backgroundFunction;
}

// -/----- Manuel control -----\-
void MatrixDisplayUi::nextApp()
{
  if (this->state.appState != IN_TRANSITION)
  {
    this->state.manuelControll = true;
    this->state.appState = IN_TRANSITION;
    this->state.ticksSinceLastStateSwitch = 0;
    this->lastTransitionDirection = this->state.appTransitionDirection;
    this->state.appTransitionDirection = 1;
  }
}
void MatrixDisplayUi::previousApp()
{
  if (this->state.appState != IN_TRANSITION)
  {
    this->state.manuelControll = true;
    this->state.appState = IN_TRANSITION;
    this->state.ticksSinceLastStateSwitch = 0;
    this->lastTransitionDirection = this->state.appTransitionDirection;
    this->state.appTransitionDirection = -1;
  }
}

bool MatrixDisplayUi::switchToApp(uint8_t app)
{
  if (app >= this->AppCount)
    return false;
  this->state.ticksSinceLastStateSwitch = 0;
  if (app == this->state.currentApp)
    return false;
  this->state.appState = FIXED;
  this->state.currentApp = app;
  return true;
}

void MatrixDisplayUi::transitionToApp(uint8_t app)
{
  if (app >= this->AppCount)
    return;
  this->state.ticksSinceLastStateSwitch = 0;
  if (app == this->state.currentApp)
    return;
  this->nextAppNumber = app;
  this->lastTransitionDirection = this->state.appTransitionDirection;
  this->state.manuelControll = true;
  this->state.appState = IN_TRANSITION;

  this->state.appTransitionDirection = app < this->state.currentApp ? -1 : 1;
}

// -/----- State information -----\-
MatrixDisplayUiState *MatrixDisplayUi::getUiState()
{
  return &this->state;
}

int8_t MatrixDisplayUi::update()
{
  long appStart = millis();
  int8_t timeBudget = this->updateInterval - (appStart - this->state.lastUpdate);
  if (timeBudget <= 0)
  {
    // Implement frame skipping to ensure time budget is keept
    if (this->setAutoTransition && this->state.lastUpdate != 0)
      this->state.ticksSinceLastStateSwitch += ceil(-timeBudget / this->updateInterval);

    this->state.lastUpdate = appStart;
    this->tick();
  }

  return this->updateInterval - (millis() - appStart);
}

void MatrixDisplayUi::tick()
{
  this->state.ticksSinceLastStateSwitch++;

  if (this->AppCount > 0)
  {
    switch (this->state.appState)
    {
    case IN_TRANSITION:
      if (this->state.ticksSinceLastStateSwitch >= this->ticksPerTransition)
      {
        this->state.appState = FIXED;
        this->state.currentApp = getnextAppNumber();
        this->state.ticksSinceLastStateSwitch = 0;
        this->nextAppNumber = -1;
      }
      break;
    case FIXED:
      // Revert manuelControll
      if (this->state.manuelControll)
      {
        this->state.appTransitionDirection = 1;
        this->state.manuelControll = false;
      }
      if (this->state.ticksSinceLastStateSwitch >= this->ticksPerApp)
      {
        if (this->setAutoTransition)
        {
          this->state.appState = IN_TRANSITION;
        }
        this->state.ticksSinceLastStateSwitch = 0;
      }
      break;
    }
  }

  this->matrix->clear();

  if (BackgroundEffect > -1)
  {
    callEffect(this->matrix, 0, 0, BackgroundEffect);
  }

  if (this->AppCount > 0)
    this->drawApp();
  this->drawOverlays();
  this->drawIndicators();
  DisplayManager.gammaCorrection();
  this->matrix->show();
}

void MatrixDisplayUi::drawIndicators()
{

  if (indicator1State && indicator1Blink && (millis() % (2 * indicator1Blink)) < indicator1Blink)
  {
    matrix->drawPixel(31, 0, indicator1Color);
    matrix->drawPixel(30, 0, indicator1Color);
    matrix->drawPixel(31, 1, indicator1Color);
  }

  if (indicator2State && indicator2Blink && (millis() % (2 * indicator2Blink)) < indicator2Blink)
  {
    matrix->drawPixel(31, 3, indicator2Color);
    matrix->drawPixel(31, 4, indicator2Color);
  }

  if (indicator3State && indicator3Blink && (millis() % (2 * indicator3Blink)) < indicator3Blink)
  {
    matrix->drawPixel(31, 7, indicator3Color);
    matrix->drawPixel(31, 6, indicator3Color);
    matrix->drawPixel(30, 7, indicator3Color);
  }

  if (indicator1State && !indicator1Blink)
  {
    matrix->drawPixel(31, 0, indicator1Color);
    matrix->drawPixel(30, 0, indicator1Color);
    matrix->drawPixel(31, 1, indicator1Color);
  }

  if (indicator2State && !indicator2Blink)
  {
    matrix->drawPixel(31, 3, indicator2Color);
    matrix->drawPixel(31, 4, indicator2Color);
  }

  if (indicator3State && !indicator3Blink)
  {
    matrix->drawPixel(31, 7, indicator3Color);
    matrix->drawPixel(31, 6, indicator3Color);
    matrix->drawPixel(30, 7, indicator3Color);
  }
}

uint8_t currentTransition;
bool gotNewTransition = true;
TransitionType getRandomTransition()
{
  // RANDOM is now index 0, so we add 1 to the result to ensure it's never selected
  return static_cast<TransitionType>((rand() % (CROSSFADE)) + 1);
}

void MatrixDisplayUi::drawApp()
{
  switch (this->state.appState)
  {
  case IN_TRANSITION:
  {
    gotNewTransition = false;
    if (currentTransition == SLIDE)
    {
      slideTransition();
    }
    else if (currentTransition == FADE)
    {
      fadeTransition();
    }
    else if (currentTransition == ZOOM)
    {
      zoomTransition();
    }
    else if (currentTransition == ROTATE)
    {
      rotateTransition();
    }
    else if (currentTransition == PIXELATE)
    {
      pixelateTransition();
    }
    else if (currentTransition == CURTAIN)
    {
      curtainTransition();
    }
    else if (currentTransition == RIPPLE)
    {
      rippleTransition();
    }
    else if (currentTransition == BLINK)
    {
      blinkTransition();
    }
    else if (currentTransition == RELOAD)
    {
      reloadTransition();
    }
    else if (currentTransition == CROSSFADE)
    {
      crossfadeTransition();
    }
    break;
  }
  case FIXED:
    if (TRANS_EFFECT == RANDOM)
    {
      if (gotNewTransition == false)
      {
        currentTransition = getRandomTransition(); // Wähle einen neuen zufälligen Übergang aus, wenn TRANS_EFFECT auf RANDOM gesetzt ist
        gotNewTransition = true;
      }
    }
    else
    {
      currentTransition = TRANS_EFFECT; // Wenn TRANS_EFFECT nicht RANDOM ist, setzen Sie currentTransition auf TRANS_EFFECT
    }
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif2);
    break;
  }
}

bool MatrixDisplayUi::isCurrentAppValid()
{
  for (size_t i = 0; i < AppCount; ++i)
  {
    if (AppFunctions[i] == AppFunctions[this->state.currentApp])
    {
      return true;
    }
  }
  return false;
}

void MatrixDisplayUi::resetState()
{
  if (!isCurrentAppValid())
  {
    this->state.lastUpdate = 0;
    this->state.ticksSinceLastStateSwitch = 0;
    this->state.appState = FIXED;
    this->state.currentApp = 0;
  }
}

void MatrixDisplayUi::forceResetState()
{
  this->state.lastUpdate = 0;
  this->state.ticksSinceLastStateSwitch = 0;
  this->state.appState = FIXED;
  this->state.currentApp = 0;
}

void MatrixDisplayUi::drawOverlays()
{
  for (uint8_t i = 0; i < this->overlayCount; i++)
  {
    (this->overlayFunctions[i])(this->matrix, &this->state, &gif2);
  }
}

void MatrixDisplayUi::drawBackground()
{
  this->backgroundFunction(this->matrix);
}

uint8_t MatrixDisplayUi::getnextAppNumber()
{
  if (this->nextAppNumber != -1)
    return this->nextAppNumber;
  return (this->state.currentApp + this->AppCount + this->state.appTransitionDirection) % this->AppCount;
}

void MatrixDisplayUi::setIndicator1Color(uint16_t color)
{
  this->indicator1Color = color;
}

void MatrixDisplayUi::setIndicator1State(bool state)
{
  this->indicator1State = state;
}

void MatrixDisplayUi::setIndicator1Blink(int blink)
{
  this->indicator1Blink = blink;
}

void MatrixDisplayUi::setIndicator2Color(uint16_t color)
{
  this->indicator2Color = color;
}

void MatrixDisplayUi::setIndicator2State(bool state)
{
  this->indicator2State = state;
}

void MatrixDisplayUi::setIndicator2Blink(int blink)
{
  this->indicator2Blink = blink;
}

void MatrixDisplayUi::setIndicator3Color(uint16_t color)
{
  this->indicator3Color = color;
}

void MatrixDisplayUi::setIndicator3State(bool state)
{
  this->indicator3State = state;
}

void MatrixDisplayUi::setIndicator3Blink(int blink)
{
  this->indicator3Blink = blink;
}

// ------------------ TRANSITIONS -------------------
float distance(int x1, int y1, int x2, int y2)
{
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void rotate(int &x, int &y, float angle)
{
  // Move the point to the origin
  x -= 16;
  y -= 4;

  // Perform the rotation
  int newX = x * cos(angle) - y * sin(angle);
  int newY = x * sin(angle) + y * cos(angle);

  // Move the point back to the actual origin
  x = newX + 16;
  y = newY + 4;
}

void MatrixDisplayUi::fadeTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
  int fadeValue;
  if (progress < 0.5)
  {
    fadeValue = pow(progress * 2, 2) * 255; // Fading out the old app (progress from 0 to 0.5)
  }
  else
  {
    fadeValue = pow((1.0 - progress) * 2, 2) * 255; // Fading in the new app (progress from 0.5 to 1.0)
  }
  this->matrix->clear(); // Clear the matrix
  // If fading out the old app
  if (progress < 0.5)
  {
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
  }
  else
  {
    // Otherwise fading in the new app
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
  }

  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      CRGB color = DisplayManager.getLeds()[this->matrix->XY(i, j)];
      color.fadeToBlackBy(fadeValue);
      DisplayManager.getLeds()[this->matrix->XY(i, j)] = color;
    }
  }
}

void MatrixDisplayUi::slideTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
  int16_t x, y, x1, y1;
  switch (this->appAnimationDirection)
  {
  case SLIDE_UP:
    x = 0;
    y = -8 * progress;
    x1 = 0;
    y1 = y + 8;
    break;
  case SLIDE_DOWN:
    x = 0;
    y = 8 * progress;
    x1 = 0;
    y1 = y - 8;
    break;
  }
  // Invert animation if direction is reversed.
  int8_t dir = this->state.appTransitionDirection >= 0 ? 1 : -1;
  x *= dir;
  y *= dir;
  x1 *= dir;
  y1 *= dir;
  this->matrix->drawRect(x, y, x1, y1, matrix->Color(0, 0, 0));
  (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, x, y, &gif1);
  (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, x1, y1, &gif2);
}

void MatrixDisplayUi::curtainTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
  if (this->state.ticksSinceLastStateSwitch == 0)
  {
    // At the beginning of the transition, copy the current app image to the ledsCopy array
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);

    for (int i = 0; i < 32; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        ledsCopy[i + j * 32] = DisplayManager.getLeds()[this->matrix->XY(i, j)];
      }
    }
  }

  // Draw the new app
  (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

  // Create the curtain effect
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (i < 16 - progress * 16 || i > 15 + progress * 16)
      {
        DisplayManager.getLeds()[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
      }
    }
  }
}

void MatrixDisplayUi::zoomTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
  float scale = 1.0;
  // If zooming out the old app
  if (progress < 0.5)
  {
    scale = 1 - progress * 2; // scale will change from 1.0 to 0.0
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
  }
  else
  {
    // Otherwise zooming in the new app
    scale = (progress - 0.5) * 2; // scale will change from 0.0 to 1.0
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
  }

  // Copy the data to the temporary array ledsCopy
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      ledsCopy[i + j * 32] = DisplayManager.getLeds()[this->matrix->XY(i, j)];
    }
  }

  // Scale the data and copy back to the matrix
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      int iScaled = 16 + (i - 16) * scale;
      int jScaled = 4 + (j - 4) * scale;

      if (iScaled < 0)
        iScaled = 0;
      if (iScaled >= 32)
        iScaled = 31;
      if (jScaled < 0)
        jScaled = 0;
      if (jScaled >= 8)
        jScaled = 7;
      DisplayManager.getLeds()[this->matrix->XY(i, j)] = ledsCopy[iScaled + jScaled * 32];
    }
  }
}

void MatrixDisplayUi::rotateTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
  float angle = progress * 2 * PI; // Rotate 360 degrees over the transition

  // Determine which app to draw
  if (progress < 0.5)
  {
    // Rotate out the old app (progress from 0 to 0.5)
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
  }
  else
  {
    // Rotate in the new app (progress from 0.5 to 1.0)
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
  }

  // Copy the data to the temporary array ledsCopy
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      ledsCopy[i + j * 32] = DisplayManager.getLeds()[this->matrix->XY(i, j)];
    }
  }

  // Rotate the data and copy back to the matrix
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      int iRotated = i;
      int jRotated = j;
      rotate(iRotated, jRotated, angle);

      if (iRotated < 0)
        iRotated = 0;
      if (iRotated >= 32)
        iRotated = 31;
      if (jRotated < 0)
        jRotated = 0;
      if (jRotated >= 8)
        jRotated = 7;

      DisplayManager.getLeds()[this->matrix->XY(i, j)] = ledsCopy[iRotated + jRotated * 32];
    }
  }
}

void MatrixDisplayUi::pixelateTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

  // Draw the old app and copy to ledsCopy
  (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      ledsCopy[i + j * 32] = DisplayManager.getLeds()[this->matrix->XY(i, j)];
    }
  }

  // Clear the screen and draw the new app
  this->matrix->clear();
  (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

  // Apply the random pixel swap transition effect
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      // If the random number is greater than the progress, display the pixel from the old app
      if (random(255) > progress * 255)
      {
        DisplayManager.getLeds()[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
      }
      // Otherwise, keep the pixel from the new app
    }
  }
}

void MatrixDisplayUi::rippleTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

  // Draw the old app and copy to ledsCopy
  (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      ledsCopy[i + j * 32] = DisplayManager.getLeds()[this->matrix->XY(i, j)];
    }
  }

  // Clear the screen and draw the new app
  this->matrix->clear();
  (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

  // Apply the checkerboard transition effect
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      // If the sum of i and j is an even number and the progress is less than 0.5, display the pixel from the old app
      if ((i + j) % 2 == 0 && progress < 0.5)
      {
        DisplayManager.getLeds()[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
      }
      // If the sum of i and j is an odd number and the progress is more than 0.5, display the pixel from the old app
      else if ((i + j) % 2 != 0 && progress >= 0.5)
      {
        DisplayManager.getLeds()[this->matrix->XY(i, j)] = ledsCopy[i + j * 32];
      }
      // Otherwise, keep the pixel from the new app
    }
  }
}

void MatrixDisplayUi::blinkTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

  // Number of blinks during the transition
  int blinks = 3;

  // Calculate the current blink state (on or off) by considering the number of blinks and the progress
  bool blinkState = (int)(progress * blinks) % 2 == 0;

  // Depending on the blinkState and the progress, draw the old or the new app
  if (blinkState)
  {
    // If blinkState is true, draw the old app if progress is less than 0.5, otherwise draw the new app
    if (progress < 0.5)
    {
      (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);
    }
    else
    {
      (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);
    }
  }
  else
  {
    // If blinkState is false, clear the matrix (display off)
    this->matrix->clear();
  }
}

void MatrixDisplayUi::reloadTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;
  int visiblePixel;

  if (progress < 0.5)
  {

    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);

    // Calculating pixel to be visible based on progress
    visiblePixel = 32 * (1.0 - (progress * 2));
    if (visiblePixel < 0)
      visiblePixel = 0;

    for (int i = visiblePixel; i < 32; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        // Turning the pixels off to create a fly out effect
        DisplayManager.getLeds()[this->matrix->XY(i, j)] = CRGB::Black;
      }
    }
  }
  else
  {
    // Draw the new app and let the pixels fly in
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

    // Calculating pixel to be visible based on progress
    visiblePixel = 32 * ((progress - 0.5) * 2);
    if (visiblePixel > 32)
      visiblePixel = 32;

    for (int i = visiblePixel; i < 32; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        // Turning the pixels off to create a fly in effect
        DisplayManager.getLeds()[this->matrix->XY(i, j)] = CRGB::Black;
      }
    }
  }
}

void MatrixDisplayUi::crossfadeTransition()
{
  float progress = (float)this->state.ticksSinceLastStateSwitch / (float)this->ticksPerTransition;

  // Draw the old app
  (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, &gif1);

  // Copy the old app data to ledsCopy array
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      ledsCopy[i + j * 32] = DisplayManager.getLeds()[this->matrix->XY(i, j)];
    }
  }

  // Clear the matrix before drawing the new app
  this->matrix->fillScreen(0);

  // Draw the new app
  (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, 0, 0, &gif2);

  // Linearly interpolate between old and new pixel colors based on the progress
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      CRGB pixelOld = ledsCopy[i + j * 32];
      CRGB pixelNew = DisplayManager.getLeds()[this->matrix->XY(i, j)];
      DisplayManager.getLeds()[this->matrix->XY(i, j)] = pixelOld.lerp8(pixelNew, progress * 255);
    }
  }
}
