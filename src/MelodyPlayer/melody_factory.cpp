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
#include "notes_array.h"
#include <LittleFS.h>
#include <algorithm>

static void removeCarriageReturn(String& s) {
  if (s.charAt(s.length() - 1) == '\r') { s = s.substring(0, s.length() - 1); }
}

Melody MelodyFactoryClass::load(String filepath, FS& fs) {
  File f = LittleFS.open(filepath, "r");
  f.setTimeout(0);

  if (!f) {
    if (debug) Serial.println("Opening file error");
    return Melody();
  }

  // Skip multi-line comments at the begin of the file
  String line = f.readStringUntil('\n');
  while (line.charAt(0) == '#') { line = f.readStringUntil('\n'); }

  bool success = false;
  success = loadTitle(line);
  if (!success) { return Melody(); }

  success = loadTimeUnit(f.readStringUntil('\n'));
  if (!success) { return Melody(); }

  success = loadNumberOfNotes(f.readStringUntil('\n'));
  if (!success) { return Melody(); }

  NoteFormat noteFormat = loadNoteFormat(f.readStringUntil('\n'));
  if (noteFormat == NoteFormat::ERROR) {
    return Melody();
  } else {
    this->noteFormat = noteFormat;
  }

  if (debug)
    Serial.println(String("This melody object will take at least: ") + (sizeof(NoteDuration) * nNotes) + "bytes");
  if (nNotes < maxLength) {
    notes = std::make_shared<std::vector<NoteDuration>>();
    notes->reserve(nNotes);
    bool error = false;
    while (f.available() && notes->size() < nNotes && !error) {
      // get a token
      String noteDuration = f.readStringUntil('|');
      error = !loadNote(noteDuration);
    }

    if (error) {
      if (debug) Serial.println("error during the tokens loading!");
      return Melody();
    }
  }

  return Melody(title, timeUnit, notes, true);
}

Melody MelodyFactoryClass::load(String title, unsigned short timeUnit, String notesToLoad[],
                                unsigned short nNotesToLoad, bool autoSilence) {
  if (title.length() == 0 && timeUnit <= 20) { return Melody(); }
  if (nNotesToLoad == 0 || nNotesToLoad > maxLength) { return Melody(); }

  if (notesToLoad == nullptr) { return Melody(); }

  notes = std::make_shared<std::vector<NoteDuration>>();
  notes->reserve(nNotesToLoad);
  noteFormat = NoteFormat::STRING;
  bool error = false;
  while (this->notes->size() < nNotesToLoad && !error) {
    String noteDuration = notesToLoad[notes->size()] + ",1";
    error = !loadNote(noteDuration);
  }
  if (error) { return Melody(); }

  return Melody(title, timeUnit, notes, autoSilence);
}

Melody MelodyFactoryClass::load(String title, unsigned short timeUnit, int frequenciesToLoad[],
                                unsigned short nFrequenciesToLoad, bool autoSilence) {
  if (title.length() == 0 && timeUnit <= 20) { return Melody(); }
  if (nFrequenciesToLoad == 0 || nFrequenciesToLoad > maxLength) { return Melody(); }

  if (frequenciesToLoad == nullptr) { return Melody(); }

  notes = std::make_shared<std::vector<NoteDuration>>();
  notes->reserve(nFrequenciesToLoad);
  noteFormat = NoteFormat::INTEGER;
  bool error = false;
  while (this->notes->size() < nFrequenciesToLoad && !error) {
    String noteDuration = String(frequenciesToLoad[notes->size()]) + ",1";
    error = !loadNote(noteDuration);
  }
  if (error) { return Melody(); }

  return Melody(title, timeUnit, notes, autoSilence);
}

bool MelodyFactoryClass::loadTitle(String line) {
  removeCarriageReturn(line);
  if (debug) Serial.println(String("Reading line:--") + line + "-- Len:" + line.length());
  if (line.startsWith("title")) {
    // Skip also '='
    String title = line.substring(6);
    this->title = title;
    return true;
  }
  return false;
}

bool MelodyFactoryClass::loadTimeUnit(String line) {
  removeCarriageReturn(line);
  if (debug) Serial.println(String("Reading line:--") + line + "-- Len:" + line.length());
  if (line.startsWith("timeUnit")) {
    // Skip '='
    String t = line.substring(9);
    this->timeUnit = t.toInt();
    if (debug) Serial.println(this->timeUnit);
    if (this->timeUnit > 20) { return true; }
  }
  return false;
}

bool MelodyFactoryClass::loadNumberOfNotes(String line) {
  removeCarriageReturn(line);
  if (debug) Serial.println(String("Reading line:--") + line + "-- Len:" + line.length());
  if (line.startsWith("length")) {
    // Skip also '='
    String len = line.substring(7);
    this->nNotes = len.toInt();
    if (debug) Serial.println(this->nNotes);
    return true;
  }
  return false;
}

MelodyFactoryClass::NoteFormat MelodyFactoryClass::loadNoteFormat(String line) {
  removeCarriageReturn(line);
  if (debug) Serial.println(String("Reading line:--") + line + "-- Len:" + line.length());
  String format;
  if (line.startsWith("format")) {
    // Skip also '='
    format = line.substring(7);
    if (debug) Serial.println(format);
  }

  NoteFormat noteFormat = NoteFormat::ERROR;
  if (format == "string") {
    noteFormat = NoteFormat::STRING;
  } else if (format == "integer") {
    noteFormat = NoteFormat::INTEGER;
  }

  return noteFormat;
}

bool MelodyFactoryClass::loadNote(String token) {
  token.trim();
  NoteDuration note;

  if (debug) Serial.println(String("note+duration: ") + token);

  String aux;
  unsigned int j = 0;

  // Get the frequency
  while (j < token.length() && token.charAt(j) != ',') {
    aux += token.charAt(j);
    j++;
  }

  if (noteFormat == NoteFormat::STRING) {
    auto n = std::find_if(noteMapping.cbegin(), noteMapping.cend(),
                          [&aux](std::pair<StringView, unsigned short> e) {
                            return e.first == aux.c_str();
                          });

    if (n != noteMapping.cend()) {
      note.frequency = n->second;
    } else {
      if (debug) Serial.println(String("This note doesn't exist: ") + aux);
      return false;
    }

  } else if (noteFormat == NoteFormat::INTEGER) {
    note.frequency = aux.toInt();
  }
  if (debug) Serial.println(String("freq: ") + note.frequency);

  j++;
  aux = "";
  while (j < token.length()) {
    aux += token.charAt(j);
    j++;
  }

  note.duration = aux.toInt();

  if (debug) Serial.println(String("duration: ") + note.duration);
  // The representation of relative note duration is fixed-point with decimal part length = 1bit
  note.duration *= 2;

  notes->push_back(note);

  return true;
}

MelodyFactoryClass MelodyFactory;
