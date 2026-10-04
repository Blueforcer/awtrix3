/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 by Daniel Eichhorn
 * Copyright (c) 2016 by Fabrice Weinberg
 * Highly modified 2023 for AWTRIX 3 by Stephan Muehl (Blueforcer)
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
#include "GifPlayer.h"
#include "DisplayManager.h"

#ifndef DEBUG_MatrixDisplayUi
#define DEBUG_MatrixDisplayUi(...)
#endif

enum AnimationDirection
{
  SLIDE_UP,
  SLIDE_DOWN
};

enum TransitionType
{
  RANDOM,
  SLIDE,
  FADE,
  ZOOM,
  ROTATE,
  PIXELATE,
  CURTAIN,
  RIPPLE,
  BLINK,
  RELOAD,
  CROSSFADE,
  INSTANT
};

enum AppState
{
  IN_TRANSITION,
  FIXED
};

// Structure of the UiState
struct MatrixDisplayUiState
{
  u_int64_t lastUpdate = 0;
  long ticksSinceLastStateSwitch = 0;

  AppState appState = FIXED;
  uint8_t currentApp = 0;

  // Normal = 1, Inverse = -1;
  int8_t appTransitionDirection = 1;
  bool lastFrameShown = false;
  bool manualControl = false;

  // Custom data that can be used by the user
  void *userData = NULL;
};

typedef void (*AppCallback)(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
typedef void (*OverlayCallback)(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);
typedef void (*BackgroundCallback)(FastLED_NeoMatrix *matrix);

class MatrixDisplayUi
{
private:
  FastLED_NeoMatrix *matrix;
  CRGB ledsCopy[256];
  // Values for the Apps
  AnimationDirection appAnimationDirection = SLIDE_DOWN;
  int8_t lastTransitionDirection = 1;

  long ticksPerApp = 151;           // ~ 5000ms at 30 FPS
  uint16_t ticksPerTransition = 15; // ~  500ms at 30 FPS

  bool setAutoTransition = true;
  bool lastFrameShown;
  AppCallback *AppFunctions = nullptr;

  // Internally used to transition to a specific app
  int8_t nextAppNumber = -1;

  // Values for Overlays
  OverlayCallback *overlayFunctions;
  BackgroundCallback backgroundFunction;
  uint8_t overlayCount = 0;
  int BackgroundEffect;
  // UI State
  MatrixDisplayUiState state;

  // Bookkeeping for update
  long updateInterval = 33;

  void drawApp();
  void drawOverlays();
  void drawBackground();
  void tick();
  void resetState();
  bool isCurrentAppValid();
  void curtainTransition();
  void slideTransition();
  void fadeTransition();
  void zoomTransition();
  void rotateTransition();
  void pixelateTransition();
  void rippleTransition();
  void blinkTransition();
  void reloadTransition();
  void crossfadeTransition();
  void instantTransition();

public:
  MatrixDisplayUi(FastLED_NeoMatrix *matrix);
  uint32_t fadeColor(uint32_t color, uint32_t interval);
  uint8_t AppCount = 0;
  /**
   * Initialise the display
   */
  void init();

  uint8_t getnextAppNumber();
  /**
   * Configure the internal used target FPS
   */
  void setTargetFPS(uint8_t fps);

  void setBackgroundEffect(int effect);
  // Automatic Control
  /**
   * Enable automatic transition to next app after the some time can be configured with `setTimePerApp` and `setTimePerTransition`.
   */
  void enablesetAutoTransition();
  /**
   * Disable automatic transition to next app.
   */
  void disablesetAutoTransition();
  /**
   * Set the direction if the automatic transitioning
   */
  void setsetAutoTransitionForwards();
  void setsetAutoTransitionBackwards();

  /**
   *  Set the approx. time a app is displayed
   */
  void setTimePerApp(long time);

  /**
   * Set the approx. time a transition will take
   */
  void setTimePerTransition(uint16_t time);

  void setIndicator1Color(uint32_t color);
  void setIndicator1State(bool state);
  void setIndicator1Blink(int Blink);
  void setIndicator1Fade(int fade);

  void setIndicator2Color(uint32_t color);
  void setIndicator2State(bool state);
  void setIndicator2Blink(int Blink);
  void setIndicator2Fade(int fade);

  void setIndicator3Color(uint32_t color);
  void setIndicator3State(bool state);
  void setIndicator3Blink(int Blink);
  void setIndicator3Fade(int fade);
  void drawIndicators();
  // Customize indicator position and style

  // App settings

  /**
   * Configure what animation is used to transition from one app to another
   */
  void setAppAnimation(AnimationDirection dir);

  /**
   * Add app drawing functions
   */
  void setApps(const std::vector<std::pair<String, AppCallback>> &appPairs);

  // Overlay
  void forceResetState();
  /**
   * Add overlays drawing functions that are draw independent of the Apps
   */
  void setOverlays(OverlayCallback *overlayFunctions, uint8_t overlayCount);
  void setBackground(BackgroundCallback backgroundfunction);
  // Manual Control
  void nextApp();
  void previousApp();

  /**
   * Switch without transition to app `app`.
   */
  bool switchToApp(uint8_t app);

  /**
   * Transition to app `app`, when the `app` number is bigger than the current
   * app the forward animation will be used, otherwise the backwards animation is used.
   */
  void transitionToApp(uint8_t app);

  // State Info
  MatrixDisplayUiState *getUiState();

  int8_t update();

  uint32_t indicator1Color = 0xFF0000;
  uint32_t indicator2Color = 0x00FF00;
  uint32_t indicator3Color = 0x0000FF;

  bool indicator1State = false;
  bool indicator2State = false;
  bool indicator3State = false;

  int indicator1Blink = 0;
  int indicator2Blink = 0;
  int indicator3Blink = 0;

  int indicator1Fade = 0;
  int indicator2Fade = 0;
  int indicator3Fade = 0;
};
#endif
