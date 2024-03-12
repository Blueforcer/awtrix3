/***************************************************************************
 *   This file is part of Melody Player, a library for Arduino             *
 *   to play notes on piezoelectric buzzers.                               *
 *                                                                         *
 *   Copyright (C) 2020-2022  Fabiano Riccardi                             *
 *                                                                         *
 *   This library is free software; you can redistribute                   *
 *   it and/or modify it under the terms of the GNU Lesser General Public  *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this library; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/
#ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#include "melody.h"
#include <Ticker.h>
#include <memory>

class MelodyPlayer {
public:
#ifdef ESP32
  /**
   * pwmChannel is optional and you have to configure it only if you play will
   * simultaneous melodies.
   */
  MelodyPlayer(unsigned char pin, unsigned char pwmChannel = 0, bool offLevel = HIGH);
#else
  MelodyPlayer(unsigned char pin, bool offLevel = HIGH);
#endif

  /**
   * Play the last melody in a synchrounus (blocking) way.
   * If the melody is not valid, this call has no effect.
   */
  void play();

  /**
   * Play the given melody in a synchronous (blocking) way.
   * If the melody is not valid, this call has no effect.
   */
  void play(Melody& melody);

  /**
   * Play the last melody in asynchronous way (return immediately).
   * If the melody is not valid, this call has no effect.
   */
  void playAsync();

  /**
   * Play the given melody in asynchronous way (return immediately).
   * If the melody is not valid, this call has no effect.
   * Set loop to true if you want the melody to start over after at the end.
   * A call back can be provided to be called at the end of the melody (only used if loop is false)
   */
  void playAsync(Melody& melody, bool loop = false, void(*stopCallback)(void) = NULL);

  /**
   * Stop the current melody.
   * Then, if you will call play() or playAsync(), the melody restarts from the begin.
   */
  void stop();

  /**
   * Pause the current melody.
   * Then, if you will call play() or playAsync(), the melody continues from
   * where it was paused.
   */
  void pause();

  /**
   * Mute the sound of the current melody without stoppig it
   * When unmute will be called the melody be resumed
  */
  void mute();

  /**
   * Unmute the current melody
  */
  void unmute();

  /**
   * Tell if playing.
   */
  bool isPlaying() const {
    return state == State::PLAY;
  }

  /**
   * Change the tempo of the current melody to the proided value
   */
  void changeTempo(int newTempo);

  /**
   * Set the volume (0-255 value) of the player (only works on ESP32 platform)
   * The volume is changed by adjusting the duty-cycle of the pwm signal sent to the piezo
   * A value of 0 will produce no sound while a value of 255 will set the duty cycle to 50%,
   * wich will produce the highest possible volume for the piezo.
   */
  void setVolume(byte volume);

  /**
   * Move the current melody and player's state to the given destination Player.
   * The source player stops and lose the reference to the actual melody (i.e. you have to call
   * play(Melody) to make the source player play again).
   */
  void transferMelodyTo(MelodyPlayer& destination);

  /**
   * Duplicate the current melody and player's state to the given destination Player.
   * Both players remains indipendent from each other (e.g. the melody can be independently
   * stopped/paused/played).
   */
  void duplicateMelodyTo(MelodyPlayer& destination);

private:
  unsigned char pin;
  byte volume = 125;
  bool loop = false;
  bool muted = false;
  void (*stopCallback)(void) = NULL;

#ifdef ESP32
  unsigned char pwmChannel;
#endif

  /**
   * The voltage to turn off the buzzer.
   *
   * NOTE: Passive buzzers have 2 states: the "rest" state (no power consumption) and the "active"
   * state (high power consumption). To emit sound, it have to oscillate between these 2 states. If
   * it stops in the active state, it doesn't emit sound, but it continues to consume energy,
   * heating the buzzer and possibly damaging itself.
   */
  bool offLevel;

  /**
   * Store the playback state of a melody and provide the methods to control it.
   */
  class MelodyState {
  public:
    MelodyState() : first(true), index(0), remainingNoteTime(0), timeUnit(0) {};
    MelodyState(const Melody& melody)
      : melody(melody), first(true), silence(false), index(0), remainingNoteTime(0), timeUnit(melody.getTimeUnit()){};
    Melody melody;

    unsigned short getIndex() const {
      return index;
    }

    bool isSilence() const {
      return silence;
    }

    void changeTempo(int newTempo) {
      timeUnit = (60 * 1000 * 4 / newTempo / 32);
    }

    /**
     * Advance the melody index by one step. If there is a pending partial note it hasn't any
     * effect.
     */
    void advance() {
      if (first) {
        first = false;
        return;
      }
      if (remainingNoteTime != 0) { return; }

      if (melody.getAutomaticSilence()) {
        if (silence) {
          index++;
          silence = false;
        } else {
          silence = true;
        }
      } else {
        index++;
      }
    }

    /**
     * Reset the state of the melody (i.e. a melody just loaded).
     */
    void reset() {
      first = true;
      index = 0;
      remainingNoteTime = 0;
      silence = false;
    }

    /**
     * Save the time to finish the current note.
     */
    void saveRemainingNoteDuration(unsigned long supportSemiNote) {
      remainingNoteTime = supportSemiNote - millis();
      // Ignore partial reproduction if the current value is below the threshold. This is needed
      // since Ticker may struggle with tight timings.
      if (remainingNoteTime < 10) { remainingNoteTime = 0; }
    }

    /**
     * Clear the partial note duration. It should be called after reproduction of the partial note
     * and it is propedeutic to advance() method.
     */
    void resetRemainingNoteDuration() {
      remainingNoteTime = 0;
    }

    /**
     * Get the remaining duration of the latest "saved" note.
     */
    unsigned short getRemainingNoteDuration() const {
      return remainingNoteTime;
    }

    /**
     * Get the current note. The duration is absolute and expressed in milliseconds.
     */
    NoteDuration getCurrentComputedNote() const {
      NoteDuration note = melody.getNote(getIndex());
      note.duration = timeUnit * note.duration;
      // because the fixed point notation
      note.duration /= 2;
      return note;
    }

  private:
    bool first;
    bool silence;
    unsigned short index;
    unsigned short timeUnit;

    /**
     * Variable to support precise pauses and move/duplicate melodies between Players.
     * Value are expressed in milliseconds.
     */
    unsigned short remainingNoteTime;
  };

  enum class State { STOP, PLAY, PAUSE };

  State state;

  std::unique_ptr<MelodyState> melodyState;

  unsigned long supportSemiNote;

  Ticker ticker;

  const static bool debug = false;

  /**
   * Change the current note with the next one.
   */
  friend void changeTone(MelodyPlayer* melody);

  /**
   * Halt the advancement of the melody reproduction.
   * This is the shared method for pause and stop.
   */
  void haltPlay();

  /**
   * Configure pin to emit PWM.
   */
  void turnOn();

  /**
   * Disable PWM and put the buzzer is low-power state.
   * This calls will fails if PWM is not initialized!
   */
  void turnOff();
};

#endif  // END MELODY_PLAYER_H