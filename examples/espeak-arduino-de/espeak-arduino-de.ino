/**
 * @file espeak-min.ino
 * @author Phil Schatzmann
 * @brief Arduino C++ API - minimum example. The espeak-ng-data is stored on in
 * progmem with the arduino-posix-fs library and we output audio to I2S with the
 * help of the AudioTools library. 
 * We use German instead of English!
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
const bool load_english = false;
ESpeak espeak(i2s, load_english);

void setup() {
  Serial.begin(115200);
  // file_systems::FSLogger.begin(file_systems::FSInfo, Serial); 

  // add foreign language configuration files
  espeak.add("/mem/data/de_dict", espeak_ng_data_de_dict,espeak_ng_data_de_dict_len);
  espeak.add("/mem/data/lang/de", espeak_ng_data_lang_gmw_de, espeak_ng_data_lang_gmw_de_len);

  // setup espeak
  espeak.begin();
  espeak.setVoice("de");

  // setup output
  auto espeak_info = espeak.audioInfo();
  auto cfg = i2s.defaultConfig();
  cfg.channels = espeak_info.channels; // 1
  cfg.sample_rate = espeak_info.sample_rate; // 22050
  cfg.bits_per_sample = espeak_info.bits_per_sample; // 16
  i2s.begin(cfg);
}

void loop() {
  espeak.say("Hallo Welt!");
  delay(5000);
}
