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

#include "MatrixDisplayUi.h"
#include "Fonts/AwtrixFont.h"

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
}

void MatrixDisplayUi::setTargetFPS(uint8_t fps)
{
  float oldInterval = this->updateInterval;
  this->updateInterval = ((float)1.0 / (float)fps) * 1000;

  // Calculate new ticksPerApp
  float changeRatio = oldInterval / (float)this->updateInterval;
  this->ticksPerApp *= changeRatio;
  this->ticksPerTransition *= changeRatio;
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
  this->ticksPerApp = time / (long)updateInterval;
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

void MatrixDisplayUi::switchToApp(uint8_t app)
{
  if (app >= this->AppCount)
    return;
  this->state.ticksSinceLastStateSwitch = 0;
  if (app == this->state.currentApp)
    return;
  this->state.appState = FIXED;
  this->state.currentApp = app;
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

  if (this->AppCount > 0)
    this->drawApp();
  this->drawOverlays();
  this->drawIndicators();
  DisplayManager.gammaCorrection();
  this->matrix->show();
}

void MatrixDisplayUi::drawIndicators()
{
  if (indicator1State && !indicator1Blink)
  {
    matrix->drawPixel(31, 0, indicator1Color);
    matrix->drawPixel(30, 0, indicator1Color);
    matrix->drawPixel(31, 1, indicator1Color);
  }
  if (indicator2State && !indicator2Blink)
  {
    matrix->drawPixel(31, 7, indicator2Color);
    matrix->drawPixel(31, 6, indicator2Color);
    matrix->drawPixel(30, 7, indicator2Color);
  }

  if (indicator1State && indicator1Blink && (millis() % 1000) < 500)
  {
    matrix->drawPixel(31, 0, indicator1Color);
    matrix->drawPixel(30, 0, indicator1Color);
    matrix->drawPixel(31, 1, indicator1Color);
  }

  if (indicator2State && indicator2Blink && (millis() % 1000) < 500)
  {
    matrix->drawPixel(31, 7, indicator2Color);
    matrix->drawPixel(31, 6, indicator2Color);
    matrix->drawPixel(30, 7, indicator2Color);
  }
}

void MatrixDisplayUi::drawApp()
{
  switch (this->state.appState)
  {
  case IN_TRANSITION:
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
    bool FirstApp = progress < 0.2;
    bool LastApp = progress > 0.8;
    this->matrix->drawRect(x, y, x1, y1, matrix->Color(0, 0, 0));
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, x, y, FirstApp, LastApp, &gif1);
    (this->AppFunctions[this->getnextAppNumber()])(this->matrix, &this->state, x1, y1, FirstApp, LastApp, &gif2);
    break;
  }
  case FIXED:
    (this->AppFunctions[this->state.currentApp])(this->matrix, &this->state, 0, 0, false, false, &gif2);
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

void MatrixDisplayUi::setIndicator2Color(uint16_t color)
{
  this->indicator2Color = color;
}

void MatrixDisplayUi::setIndicator2State(bool state)
{
  this->indicator2State = state;
}

void MatrixDisplayUi::setIndicator1Blink(bool blink)
{
  this->indicator1Blink = blink;
}

void MatrixDisplayUi::setIndicator2Blink(bool blink)
{
  this->indicator2Blink = blink;
}
