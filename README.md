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

## Examples

- [espeak](examples/espeak/espeak.ino): Recommended example
- [espeak-sd](examples/espeak-sd/espeak-sd.ino): Example using SD drive.
- [espeak-cmake](examples/espeak-cmake/espeak-cmake.ino): Example compiled and running on desktop.


## Dependencies

This library requires the installation of the following additinal libraries:

- https://github.com/pschatzmann/arduino-posix-fs (Reading of data)
- https://github.com/pschatzmann/arduino-audio-tools (Output of Audio)

## Installation in Arduino

You can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with

```
cd  ~/Documents/Arduino/libraries
git clone pschatzmann/arduino-espeak-ng.git
```

I recommend to use git because you can easily update to the latest version just by executing the git pull command in the project folder.





