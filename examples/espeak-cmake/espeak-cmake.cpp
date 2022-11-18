/**
 * @file espeak-min.ino
 * @author Phil Schatzmann
 * @brief Compile with cmake to run on desktop, e.g. to use a debugger
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "Arduino.h"
#include "AudioTools.h" // https://github.com/pschatzmann/arduino-audio-tools
#include "AudioLibs/PortAudioStream.h"
#include "espeak.h"

PortAudioStream out; // or replace with AudioKitStream for AudioKit
//ESpeak espeak(out,"/Users/pschatzmann/Documents/Arduino/libraries/arduino-espeak-ng/espeak-ng-data");
ESpeak espeak(out);

void i2sStart(){
}

void i2sStop(){
}


void setup() {
  Serial.begin(115200);

  // setup espeak
  espeak.begin();
  espeak.setOutputBeginCallback(i2sStart);
  espeak.setOutputEndCallback(i2sStop);  

  // setup output
  audio_info info = espeak.audioInfo();
  auto cfg = out.defaultConfig();
  cfg.channels = info.channels; // 1
  cfg.sample_rate = info.sample_rate; //22050;
  out.begin(cfg);


}

void loop() {
  espeak.say("hello my name is Phil");
  delay(5000);
}