/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 by Daniel Eichhorn
 * Copyright (c) 2016 by Fabrice Weinberg
 * Highly modified 2023 for AWTRIX Light by Stephan Muehl (Blueforcer)
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

#ifndef MatrixDisplayUi_h
#define MatrixDisplayUi_h

#include <Arduino.h>
#include "FastLED_NeoMatrix.h"
#include <vector>

// #define DEBUG_MatrixDisplayUi(...) Serial.printf( __VA_ARGS__ )

#ifndef DEBUG_MatrixDisplayUi
#define DEBUG_MatrixDisplayUi(...)
#endif

enum AnimationDirection
{
  SLIDE_UP,
  SLIDE_DOWN,
  SLIDE_LEFT,
  SLIDE_RIGHT
};

enum FrameState
{
  IN_TRANSITION,
  FIXED
};

// Structure of the UiState
struct MatrixDisplayUiState
{
  u_int64_t lastUpdate = 0;
  uint16_t ticksSinceLastStateSwitch = 0;

  FrameState frameState = FIXED;
  uint8_t currentFrame = 0;

  // Normal = 1, Inverse = -1;
  int8_t frameTransitionDirection = 1;

  bool manuelControll = false;

  // Custom data that can be used by the user
  void *userData = NULL;
};

typedef void (*AppCallback)(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame);
typedef void (*OverlayCallback)(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state);

class MatrixDisplayUi
{
private:
  FastLED_NeoMatrix *matrix;

  // Values for the Frames
  AnimationDirection frameAnimationDirection = SLIDE_RIGHT;

  int8_t lastTransitionDirection = 1;

  uint16_t ticksPerFrame = 151;     // ~ 5000ms at 30 FPS
  uint16_t ticksPerTransition = 15; // ~  500ms at 30 FPS

  bool setAutoTransition = true;

  AppCallback *AppFunctions;

  uint8_t AppCount = 0;

  // Internally used to transition to a specific frame
  int8_t nextAppNumber = -1;

  // Values for Overlays
  OverlayCallback *overlayFunctions;
  uint8_t overlayCount = 0;

  // UI State
  MatrixDisplayUiState state;

  // Bookeeping for update
  uint8_t updateInterval = 33;

  uint8_t getnextAppNumber();
  void drawIndicator();
  void drawApp();
  void drawOverlays();
  void tick();
  void resetState();

public:
  MatrixDisplayUi(FastLED_NeoMatrix *matrix);

  /**
   * Initialise the display
   */
  void init();

  /**
   * Configure the internal used target FPS
   */
  void setTargetFPS(uint8_t fps);

  // Automatic Controll
  /**
   * Enable automatic transition to next frame after the some time can be configured with `setTimePerApp` and `setTimePerTransition`.
   */
  void enablesetAutoTransition();

  /**
   * Disable automatic transition to next frame.
   */
  void disablesetAutoTransition();

  /**
   * Set the direction if the automatic transitioning
   */
  void setsetAutoTransitionForwards();
  void setsetAutoTransitionBackwards();

  /**
   *  Set the approx. time a frame is displayed
   */
  void setTimePerApp(uint16_t time);

  /**
   * Set the approx. time a transition will take
   */
  void setTimePerTransition(uint16_t time);

  // Customize indicator position and style

  // Frame settings

  /**
   * Configure what animation is used to transition from one frame to another
   */
  void setAppAnimation(AnimationDirection dir);

  /**
   * Add frame drawing functions
   */
  void setApps(const std::vector<std::pair<uint16_t, AppCallback>> &appPairs);

  // Overlay

  /**
   * Add overlays drawing functions that are draw independent of the Frames
   */
  void setOverlays(OverlayCallback *overlayFunctions, uint8_t overlayCount);

  // Manual Control
  void nextApp();
  void previousApp();

  /**
   * Switch without transition to frame `frame`.
   */
  void switchToApp(uint8_t frame);

  /**
   * Transition to frame `frame`, when the `frame` number is bigger than the current
   * frame the forward animation will be used, otherwise the backwards animation is used.
   */
  void transitionToApp(uint8_t frame);

  // State Info
  MatrixDisplayUiState *getUiState();

  int8_t update();
};
#endif
