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
#include "melody_factory.h"
#include "notes.h"
#include <LittleFS.h>
// clang-format off
const uint16_t sourceNotes[] = {
  0,
  NOTE_C4,
  NOTE_CS4,
  NOTE_D4,
  NOTE_DS4,
  NOTE_E4,
  NOTE_F4,
  NOTE_FS4,
  NOTE_G4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_AS4,
  NOTE_B4,

  NOTE_C5,
  NOTE_CS5,
  NOTE_D5,
  NOTE_DS5,
  NOTE_E5,
  NOTE_F5,
  NOTE_FS5,
  NOTE_G5,
  NOTE_GS5,
  NOTE_A5,
  NOTE_AS5,
  NOTE_B5,

  NOTE_C6,
  NOTE_CS6,
  NOTE_D6,
  NOTE_DS6,
  NOTE_E6,
  NOTE_F6,
  NOTE_FS6,
  NOTE_G6,
  NOTE_GS6,
  NOTE_A6,
  NOTE_AS6,
  NOTE_B6,

  NOTE_C7,
  NOTE_CS7,
  NOTE_D7,
  NOTE_DS7,
  NOTE_E7,
  NOTE_F7,
  NOTE_FS7,
  NOTE_G7,
  NOTE_GS7,
  NOTE_A7,
  NOTE_AS7,
  NOTE_B7,

  2 * NOTE_C7,
  2 * NOTE_CS7,
  2 * NOTE_D7,
  2 * NOTE_DS7,
  2 * NOTE_E7,
  2 * NOTE_F7,
  2 * NOTE_FS7,
  2 * NOTE_G7,
  2 * NOTE_GS7,
  2 * NOTE_A7,
  2 * NOTE_AS7,
  2 * NOTE_B7,
};
// clang-format on

Melody MelodyFactoryClass::loadRtttlFile(String filepath, FS& fs) {
  File f = LittleFS.open(filepath, "r");
  f.setTimeout(0);

  if (!f) {
    if (debug) Serial.println("Opening file error");
    return Melody();
  }

  String title = f.readStringUntil(':');
  title.trim();
  if (debug) Serial.println(String("Title:") + title);
  if (title.length() == 0) { return Melody(); }

  String values = f.readStringUntil(':');
  values.trim();
  if (debug) Serial.println(String("Default values:") + values);
  if (values.length() == 0) { return Melody(); }

  parseDefaultValues(values);

  // 32 because it is the shortest note!
  int timeUnit = 60 * 1000 * 4 / beat / 32;

  notes = std::make_shared<std::vector<NoteDuration>>();
  bool result = true;
  while (f.available() && notes->size() < maxLength && result) {
    String s = f.readStringUntil(',');
    s.trim();
    result = parseRtttlNote(s);
  }
  if (result && notes->size() > 0) { return Melody(title, timeUnit, notes, false); }

  return Melody();
}

Melody MelodyFactoryClass::loadRtttlDB(String filepath, String title, FS& fs) {
  File f = LittleFS.open(filepath, "r");
  f.setTimeout(0);

  if (!f) {
    if (debug) Serial.println("Opening file error");
    return Melody();
  }

  if (title.length() == 0) {
    if (debug) Serial.println("Title length = 0");
    return Melody();
  }

  if (!f.find(title.c_str())) {
    if (debug) Serial.println("Unable to find melody with title: " + String(title));
    return Melody();
  }
  f.readStringUntil(':');

  String values = f.readStringUntil(':');
  values.trim();
  if (debug) Serial.println(String("Default values:") + values);
  if (values.length() == 0) { return Melody(); }

  parseDefaultValues(values);

  // 32 because it is the shortest note!
  int timeUnit = 60 * 1000 * 4 / beat / 32;

  size_t position = f.position();
  int bytesUntilNewLine = f.readStringUntil('\n').length();
  f.seek(position);

  notes = std::make_shared<std::vector<NoteDuration>>();
  bool result = true;
  while (f.available() && notes->size() < maxLength && result && bytesUntilNewLine > 0) {
    String s = f.readStringUntil(',');
    if (s.length() > bytesUntilNewLine) { s = s.substring(0, bytesUntilNewLine); }
    bytesUntilNewLine -= s.length() + 1;
    s.trim();
    result = parseRtttlNote(s);
  }
  if (result && notes->size() > 0) { return Melody(title, timeUnit, notes, false); }

  return Melody();
}

Melody MelodyFactoryClass::loadRtttlString(const char rtttlMelody[]) {
  String title;
  int i = 0;
  while (rtttlMelody[i] != 0 && rtttlMelody[i] != ':') {
    title.concat(rtttlMelody[i]);
    i++;
  }

  if (title.length() == 0 || rtttlMelody[i] == 0) { return Melody(); }

  // skip ':'
  i++;

  String defaultParameters;
  while (rtttlMelody[i] != 0 && rtttlMelody[i] != ':') {
    defaultParameters.concat(rtttlMelody[i]);
    i++;
  }

  if (rtttlMelody[i] == 0) { return Melody(); }

  defaultParameters.trim();
  parseDefaultValues(defaultParameters);

  // 32 because it is the shortest note!
  int timeUnit = 60 * 1000 * 4 / beat / 32;

  // skip ':'
  i++;

  notes = std::make_shared<std::vector<NoteDuration>>();
  // Read notes
  while (rtttlMelody[i] != 0) {
    String note;
    while (rtttlMelody[i] != 0 && rtttlMelody[i] != ',') {
      note.concat(rtttlMelody[i]);
      i++;
    }
    note.trim();
    parseRtttlNote(note);
    if (rtttlMelody[i] == ',') { i++; }
  }
  if (notes->size() > 0) { return Melody(title, timeUnit, notes, false); }

  return Melody();
}

/**
 * Parse an unsigned integer starting from the given startFrom to the first non-digit char.
 * Return zero if it cannot parse a number. *startFrom* will point to the first non-digit char.
 */
unsigned int getUnsignedInt(const String& s, int& startFrom) {
  unsigned int temp = 0;
  while (isDigit(s.charAt(startFrom))) {
    temp = (temp * 10) + s.charAt(startFrom) - '0';
    startFrom++;
  }
  return temp;
}

unsigned int MelodyFactoryClass::parseDuration(const String& s, int& startFrom) {
  // Skip '='
  startFrom++;
  unsigned int temp = getUnsignedInt(s, startFrom);
  if (temp != 1 && temp != 2 && temp != 4 && temp != 8 && temp != 16 && temp != 32) { return 0; }
  // Discard ','
  startFrom++;
  return temp;
}

unsigned int MelodyFactoryClass::parseOctave(const String& s, int& startFrom) {
  // Skip '='
  startFrom++;
  unsigned int temp = getUnsignedInt(s, startFrom);
  if (temp < 4 || temp > 7) { return 0; }
  // Discard ','
  startFrom++;
  return temp;
}

unsigned int MelodyFactoryClass::parseBeat(const String& s, int& startFrom) {
  // Skip '='
  startFrom++;
  unsigned int temp = getUnsignedInt(s, startFrom);

  // BPM is arbitrarily limited to 300. You may try to increase it, but remember that
  // actually, the minimum note length is 60(seconds)/300(bpm)/32(minimum note length) = 6.25ms.
  // If you reduce this duration, you may not be able to keep up the pace to play a smooth
  // async playback while doing other operations.
  if (!(10 <= temp && temp <= 300)) { return 0; }
  // Discard ','
  startFrom++;
  return temp;
}

bool MelodyFactoryClass::parseRtttlNote(const String& s) {
  int i = 0;

  unsigned short relativeDuration = this->duration;
  // Optional number: note duration (e.g 4=quarter note, ...)
  if (isdigit(s.charAt(i))) {
    unsigned int temp = getUnsignedInt(s, i);
    if (temp) { relativeDuration = temp; }
  }

  // To match struct NoteDuration format, I need the direct
  // note length, instead RTTTL provides the denominator
  // of the whole note
  if (relativeDuration == 32) {
    relativeDuration = 1;
  } else if (relativeDuration == 16) {
    relativeDuration = 2;
  } else if (relativeDuration == 8) {
    relativeDuration = 4;
  } else if (relativeDuration == 4) {
    relativeDuration = 8;
  } else if (relativeDuration == 2) {
    relativeDuration = 16;
  } else if (relativeDuration == 1) {
    relativeDuration = 32;
  } else {
    relativeDuration = 0;
  }

  // note (p is silence)
  int note = 0;
  switch (s.charAt(i)) {
    case 'c': note = 1; break;
    case 'd': note = 3; break;
    case 'e': note = 5; break;
    case 'f': note = 6; break;
    case 'g': note = 8; break;
    case 'a': note = 10; break;
    case 'b': note = 12; break;
    case 'p':
    default: note = 0;
  }

  i++;

  // Optional #
  if (s.charAt(i) == '#') {
    note++;
    i++;
  }

  // The representation of relative note duration is fixed-point with decimal part length = 1bit
  relativeDuration *= 2;
  // get optional '.' dotted note
  // This note will last 50% more
  if (s.charAt(i) == '.') {
    relativeDuration += relativeDuration / 2;
    i++;
  }

  int scale;
  // now, get scale
  if (isdigit(s.charAt(i))) {
    scale = s.charAt(i) - '0';
    i++;
  } else {
    scale = octave;
  }

  unsigned short freq;
  if (note) {
    freq = sourceNotes[(scale - 4) * 12 + note];
  } else {
    freq = 0;
  }

  notes->push_back({ .frequency = freq, .duration = relativeDuration });
  return true;
}

void MelodyFactoryClass::parseDefaultValues(String values) {
  int i = 0;

  if (values.charAt(i) == 'd') { i++; }
  duration = parseDuration(values, i);
  if (duration == 0) { duration = defaultDuration; }

  if (values.charAt(i) == 'o') { i++; }
  octave = parseOctave(values, i);
  if (octave == 0) { octave = defaultOctave; }

  if (values.charAt(i) == 'b') {
    i++;
    beat = parseBeat(values, i);
  }
  if (beat == 0) { beat = defaultBeat; }
}
