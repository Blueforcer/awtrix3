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
#include "melody_player.h"

/**
 * https://stackoverflow.com/questions/24609271/errormake-unique-is-not-a-member-of-std
 */
template<typename T, typename... Args> std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

void MelodyPlayer::play() {
  if (melodyState == nullptr) { return; }

  turnOn();
  state = State::PLAY;

  melodyState->advance();
  while (melodyState->getIndex() + melodyState->isSilence() < melodyState->melody.getLength()) {
    NoteDuration computedNote = melodyState->getCurrentComputedNote();
    if (debug)
      Serial.println(String("Playing: frequency:") + computedNote.frequency
                     + " duration:" + computedNote.duration);
    if (melodyState->isSilence()) {
#ifdef ESP32
      ledcWriteTone(pwmChannel, 0);
#else
      noTone(pin);
#endif
      delay(0.3f * computedNote.duration);
    } else {
#ifdef ESP32
      ledcWriteTone(pwmChannel, computedNote.frequency);
#else
      tone(pin, computedNote.frequency);
#endif
      delay(computedNote.duration);
    }
    melodyState->advance();
  }
  stop();
}

void MelodyPlayer::play(Melody& melody) {
  if (!melody) { return; }
  melodyState = make_unique<MelodyState>(melody);
  play();
}

void changeTone(MelodyPlayer* player) {
  // The last silence is not reproduced
  player->melodyState->advance();
  if (player->melodyState->getIndex() + player->melodyState->isSilence()
      < player->melodyState->melody.getLength()) {
    NoteDuration computedNote(player->melodyState->getCurrentComputedNote());

    float duration = player->melodyState->getRemainingNoteDuration();
    if (duration > 0) {
      player->melodyState->resetRemainingNoteDuration();
    } else {
      if (player->melodyState->isSilence()) {
        duration = 0.3f * computedNote.duration;
      } else {
        duration = computedNote.duration;
      }
    }
    if (player->debug)
      Serial.println(String("Playing async: freq=") + computedNote.frequency + " dur=" + duration
                     + " iteration=" + player->melodyState->getIndex());

    if (player->melodyState->isSilence()) {
#ifdef ESP32
      ledcWriteTone(player->pwmChannel, 0);
#else
      tone(player->pin, 0);
#endif

#ifdef ESP32
      player->ticker.once_ms(duration, changeTone, player);
#else
      player->ticker.once_ms_scheduled(duration, std::bind(changeTone, player));
#endif
    } else {
#ifdef ESP32
      ledcWriteTone(player->pwmChannel, computedNote.frequency);
#else
      tone(player->pin, computedNote.frequency);
#endif

#ifdef ESP32
      player->ticker.once_ms(duration, changeTone, player);
#else
      player->ticker.once_ms_scheduled(duration, std::bind(changeTone, player));
#endif
    }
    player->supportSemiNote = millis() + duration;
  } else {
    player->stop();
  }
}

void MelodyPlayer::playAsync() {
  if (melodyState == nullptr) { return; }

  turnOn();
  state = State::PLAY;

  // Start immediately
#ifdef ESP32
  ticker.once(0, changeTone, this);
#else
  ticker.once_scheduled(0, std::bind(changeTone, this));
#endif
}

void MelodyPlayer::playAsync(Melody& melody) {
  if (!melody) { return; }
  melodyState = make_unique<MelodyState>(melody);
  playAsync();
}

void MelodyPlayer::stop() {
  if (melodyState == nullptr) { return; }

  haltPlay();
  state = State::STOP;
  melodyState->reset();
}

void MelodyPlayer::pause() {
  if (melodyState == nullptr) { return; }

  haltPlay();
  state = State::PAUSE;
  melodyState->saveRemainingNoteDuration(supportSemiNote);
}

void MelodyPlayer::transferMelodyTo(MelodyPlayer& destPlayer) {
  if (melodyState == nullptr) { return; }

  destPlayer.stop();

  bool playing = isPlaying();

  haltPlay();
  state = State::STOP;
  melodyState->saveRemainingNoteDuration(supportSemiNote);
  destPlayer.melodyState = std::move(melodyState);

  if (playing) {
    destPlayer.playAsync();
  } else {
    destPlayer.state = state;
  }
}

void MelodyPlayer::duplicateMelodyTo(MelodyPlayer& destPlayer) {
  if (melodyState == nullptr) { return; }

  destPlayer.stop();
  destPlayer.melodyState = make_unique<MelodyState>(*(this->melodyState));
  destPlayer.melodyState->saveRemainingNoteDuration(supportSemiNote);

  if (isPlaying()) {
    destPlayer.playAsync();
  } else {
    destPlayer.state = state;
  }
}

#ifdef ESP32
MelodyPlayer::MelodyPlayer(unsigned char pin, unsigned char pwmChannel, bool offLevel)
  : pin(pin), pwmChannel(pwmChannel), offLevel(offLevel), state(State::STOP), melodyState(nullptr) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, offLevel);
};
#else
MelodyPlayer::MelodyPlayer(unsigned char pin, bool offLevel)
  : pin(pin), offLevel(offLevel), state(State::STOP), melodyState(nullptr) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, offLevel);
};
#endif

void MelodyPlayer::haltPlay() {
  // Stop player, but do not reset the melodyState
  ticker.detach();
  turnOff();
}

void MelodyPlayer::turnOn() {
#ifdef ESP32
  const int resolution = 8;
  // 2000 is a frequency, it will be changed at the first play
  ledcSetup(pwmChannel, 2000, resolution);
  ledcAttachPin(pin, pwmChannel);
  ledcWrite(pwmChannel, 125);
#endif
}

void MelodyPlayer::turnOff() {
#ifdef ESP32
  ledcWrite(pwmChannel, 0);
  ledcDetachPin(pin);
#else
  // Remember that this will set LOW output, it doesn't mean that buzzer is off (look at offLevel
  // for more info).
  noTone(pin);
#endif

  pinMode(pin, LOW);
  digitalWrite(pin, offLevel);
}
