/**
 * @file espeak-min.ino
 * @author Phil Schatzmann
 * @brief Arduino C++ API - minimum example. The espeak-ng-data is stored on in
 * progmem with the arduino-posix-fs library and we output audio to I2S with the
 * help of the AudioTools library
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 */

#include "AudioTools.h" // https://github.com/pschatzmann/arduino-audio-tools
//#include "AudioLibs/AudioKit.h" // https://github.com/pschatzmann/arduino-audiokit
#include "FileSystems.h" // https://github.com/pschatzmann/arduino-posix-fs
#include "espeak.h"

I2SStream i2s; // or replace with AudioKitStream for AudioKit
ESpeak espeak(i2s);

void setup() {
  Serial.begin(115200);
  //file_systems::FSLogger.begin(file_systems::FSInfo, Serial); 
  // add voice option
  espeak.add("/mem/data/voices/!v/Alicia", espeak_ng_data_voices__v_Alicia, espeak_ng_data_voices__v_Alicia_len);

  // setup espeak
  espeak.begin();
  // Set voice and voice option
  espeak.setVoice("en+Alicia");

  // setup output
  audio_info espeak_info = espeak.audioInfo();
  auto cfg = i2s.defaultConfig();
  cfg.channels = espeak_info.channels; // 1
  cfg.sample_rate = espeak_info.sample_rate; // 22050
  cfg.bits_per_sample = espeak_info.bits_per_sample; // 16
  i2s.begin(cfg);

}

void loop() {
  espeak.say("Hello world!");
  delay(5000);
}
