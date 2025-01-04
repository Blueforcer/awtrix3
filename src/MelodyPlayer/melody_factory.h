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
#ifndef MELODY_FACTORY_H
#define MELODY_FACTORY_H

#include "melody.h"

#include <FS.h>
#ifdef ESP32
#include <SPIFFS.h>
#endif

class MelodyFactoryClass {
public:
  /**
   * Load the melody from file in MelodyPlayer format.
   */
  Melody load(String filePath, FS& fs = SPIFFS);

  /**
   * Load melody from file in RTTTL format. The file must contain only one melody.
   */
  Melody loadRtttlFile(String filePath, FS& fs = SPIFFS);

  /**
   * Load melody with the given title from a file containing multiple RTTTL melody (one Melody per
   * line).
   */
  Melody loadRtttlDB(String filepath, String title, FS& fs = SPIFFS);

  /**
   * Load melody from string in RTTTL format.
   */
  Melody loadRtttlString(const char rtttlMelody[]);

  /**
   * Create a melody with the given parameters.
   * Notes are represented as string accordigly to english notation (i.e. "C4", "G3", "G6").
   * This method assumes that each note lasts 1 beat.
   * frequenciesToLoad are integer numbers expressing the real reproduced frequency.
   * automaticSilence, if true, automatically inserts a small silence between 2 consecutive notes.
   */
  Melody load(String title, unsigned short timeUnit, String notesToLoad[],
              unsigned short nNotesToLoad, bool autoSilence = true);

  /**
   * Create a melody with the given parameters.
   * This method assumes that each note lasts 1 beat.
   * frequenciesToLoad are integer numbers expressing the real reproduced frequency.
   * The last parameter, automaticSilence, if true, automatically inserts a small silence between 2
   * consecutive notes.
   */
  Melody load(String title, unsigned short timeUnit, int frequenciesToLoad[],
              unsigned short nFrequenciesToLoad, bool autoSilence = true);

private:
  enum class NoteFormat { ERROR, STRING, INTEGER };

  String title;
  unsigned short timeUnit;
  NoteFormat noteFormat;
  std::shared_ptr<std::vector<NoteDuration>> notes;
  // Used to check how many notes are stored in a file.
  unsigned short nNotes;
  const unsigned short maxLength = 1000;

  ///////////// RTTTL helpers
  /**
   * The default duration of a note. For example,
   * "4" means that each note with no duration specifier
   * is by default considered a quarter note. Possibile values:
   * 1 - whole note
   * 2 - half note
   * 4 - quarter note
   * 8 - eighth note
   * 16 - sixteenth note
   * 32 - thirty-second note
   */
  const unsigned short defaultDuration = 4;
  unsigned short duration;

  /**
   * The default octave. There are four octaves in the RTTTL format [4-7].
   */
  const unsigned short defaultOctave = 6;
  unsigned short octave;

  /**
   * The default BPM (beats per minute) value. BPM is arbitrarily limited between 10 and 300. Look
   * at the implementation of parseBeat for more info.
   */
  const unsigned short defaultBeat = 63;
  unsigned short beat;

  /**
   * Try to parse the default parameters of RTTTL melody.
   * If user-defined defaults are not found it sets the default values as prescribed by RTTTL
   * specification.
   */
  void parseDefaultValues(String values);

  unsigned int parseDuration(const String& s, int& startFrom);
  unsigned int parseOctave(const String& s, int& startFrom);
  unsigned int parseBeat(const String& s, int& startFrom);
  bool parseRtttlNote(const String& s);

  //////////// END RTTTL helpers

  /**
   * Parse the title from the given string.
   * Return true on success.
   */
  bool loadTitle(String line);

  /**
   * Parse the time unit from the given string.
   * Return true on success.
   */
  bool loadTimeUnit(String line);

  /**
   * Parse the number of notes from the given string.
   * Return true on success.
   */
  bool loadNumberOfNotes(String line);

  /**
   * Parse the note's format from the given string.
   */
  NoteFormat loadNoteFormat(String line);

  /**
   * Parse a token (a note and its duration) from the given string.
   * The format of this token is:
   * <note> + ',' + <duration>.
   * Return true if the parsing succeeds, false otherwise.
   */
  bool loadNote(String token);

  // Enable debug messages over serial port
  static const bool debug = false;
};

extern MelodyFactoryClass MelodyFactory;

#endif  // END MELODY_FACTORY_H
