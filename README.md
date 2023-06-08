# eSpeak NG Text-to-Speech for Arduino

I converted [eSpeak NG](https://github.com/espeak-ng/espeak-ng) to an Arduino Library.

The eSpeak NG is a compact open source software text-to-speech synthesizer for 
Linux, Windows, Android and other operating systems. It supports 
[more than 100 languages and accents](docs/languages.md). It is based on the eSpeak engine
created by Jonathan Duddington.

eSpeak NG uses a "formant synthesis" method. This allows many languages to be
provided in a small size. The speech is clear, and can be used at high speeds,
but is not as natural or smooth as larger synthesizers which are based on human
speech recordings. It also supports Klatt formant synthesis, and the ability
to use MBROLA as backend speech synthesizer.

## Configuration Data

eSpeak needs to load configuration data: This can be found in the espeak-ng-data directory. It is also made available as include header files: see the src/data directory.

You could load the data from an SD drive, however this is using an excessive amount of RAM and will only work if you have additinal PSRAM. The recommended way is to load the data from PROGMEM.

### Memory Requirements

With the miminum configuration we get the following on an ESP32:

- Sketch uses 1638633 bytes (52%) of program storage space. Maximum is 3145728 bytes.
- Global variables use 120924 bytes (36%) of dynamic memory, leaving 206756 bytes 

If you use an ESP32, don't forget to select the partition scheme Huge APP!

## Examples

- [espeak-arduino](examples/espeak-arduino/espeak-arduino.ino): Recommended example
- [espeak-arduino-voice](examples/espeak-arduino-voice/espeak-arduino-voice.ino): Example defining a voice.
- [espeak](examples/espeak/espeak.ino): functional example
- [espeak-cmake](examples/espeak-cmake/espeak-cmake.ino): Example compiled and running on desktop.


## Dependencies

This library requires the installation of the following additinal libraries:

- https://github.com/pschatzmann/arduino-posix-fs (Reading of data)
- https://github.com/pschatzmann/arduino-audio-tools (Output of Audio)


## Documentation

- [C++ API](https://pschatzmann.github.io/arduino-espeak-ng/docs/html/classESpeak.html)
- [Functional API](https://pschatzmann.github.io/arduino-espeak-ng/docs/html/speak__lib_8h.html)

You can also find further information in the [Wiki](https://github.com/pschatzmann/arduino-espeak-ng/wiki)...

## Installation in Arduino

You can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with

```
cd  ~/Documents/Arduino/libraries
git clone pschatzmann/arduino-espeak-ng.git
```

I recommend to use git because you can easily update to the latest version just by executing the git pull command in the project folder.





