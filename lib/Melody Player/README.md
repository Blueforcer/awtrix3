# Melody Player

[![arduino-library-badge](https://www.ardu-badge.com/badge/Melody%20Player.svg)](https://www.ardu-badge.com/Melody%20Player) ![Compile Library Examples](https://github.com/fabianoriccardi/melody-player/actions/workflows/LibraryBuild.yml/badge.svg)

Melody Player is an Arduino library to play melodies on buzzers on ESP8266 and ESP32 in a non-blocking manner. Melodies can be written directly in code or loaded from file. It supports Ring Tone Text Transfer Language (RTTTL) format and a custom format developed specifically to enjoy all the benefits of this library.

## Motivation

Arduino cores provide tone() function to emit a PWM signal, and it is often used to play beeps on a buzzer. Modulating the PWM frequency, you can play sounds at a given frequency, i.e., you can play a note, and it is relatively easy to play a monophonic melody. However, the Arduino ecosystem lacks of a structured and easy way to accomplish this task (i.e., each developer have to write bloating code for it). So, I started to write a simple snippet to asynchronously play sequences of notes on a buzzer without bloating user-code with long parsing and playback methods. Moreover, since ESP8266 and ESP32 cores provide a file system for the embedded flash melody, I wanted a melody format easy to remember, human-readable and editable with a simple text editor. From this context Melody Player was born, improved, and extended over time.

## Features

* Non-blocking playback
* Support RTTTL format (allow reuse of ringtones popular on old mobile phones)
* Support custom format to allow finer control of frequencies
* For ESP8266 and ESP32
* Load melodies from file (support both LittleFS and SPIFFS file systems)
* Control the melody playback through trivial *play*, *pause*, *stop* methods
* Support multiple playing buzzers
* *Migration* and *duplication* of melodies among buzzers

## Installation

You can find Melody Player on Arduino and PlatformIO library registries. You can install it through  Arduino IDE, or you can use the respective command-line tools running:

    arduino-cli lib install "Melody Player"

or:

    pio lib install "fabianoriccardi/Melody Player"

## Usage

Here a quick overview of the main methods to use this library. Initialize MelodyPlayer by specifying the pin where the buzzer is attached to:

    MelodyPlayer player(4);

Load the RTTTL melody from file (remember to initialize the file system before this call):

    Melody melody = MelodyFactory.loadRtttlFile("/the-anthem.rtttl");

Check if the melody is loaded correctly:

    if(!melody) {
        Serial.println("Cannot play this melody");
    }

Play it using blocking or non-blocking methods:

    player.play(melody);

or

    player.playAsync(melody);

In case of non-blocking playback, you can check if the melody is running:

    if(player.isPlaying()){
        Serial.println("Playing...");
    }

and pause/continue to play/stop the melody through:

    player.pause();
    player.play();
    player.stop();

## Details about the custom format

You can write a melody in a text file, accordingly to the following specifications:

    title={Name of the melody}
    timeUnit={Base time in millisecond}
    length={Array length}
    format={This value can be "integer" or "string", and it specifies how the tone frequency is represented in the following array}
    {Array composed by pair <frequency; duration> and spit by '|' (pipe character)}

where:

* *frequency* can be either an integer number or a string, depending on what specified in "format" parameter. If format type is "string", it represents a note in English convention e.g. E5, F1
* *duration* is an integer expressed in "timeUnits"

A small pause is automatically added between 2 consecutive pairs. You can add comments using '#' at the begin of the line.

Example 1: this melody codifies 2 "beeps" using the "string" codification:

    title=Beep
    timeUnit=200
    length=1
    format=string
    G7,3|SILENCE,1|G7,3

Example 2: the same melody using the "integer" codification:

    title=Beep
    timeUnit=200
    length=1
    format=integer
    3136,3|0,1|3136,3

## Useful links

* <https://panuworld.net/nuukiaworld/download/nokix/rtttl.htm>: RTTTL specification
* <https://adamonsoon.github.io/rtttl-play/>: Test and listen RTTTL melodies
