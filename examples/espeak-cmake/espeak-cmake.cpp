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
espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
const char *path = "../../../espeak-ng-data-min"; 
void *user_data = nullptr;
unsigned int *identifier = nullptr;
int buflength = 500, options = 0;
unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
espeak_POSITION_TYPE position_type = POS_CHARACTER;

void setup() {
  Serial.begin(115200);

  // setup output
  auto cfg = out.defaultConfig();
  cfg.channels = 1;
  cfg.sample_rate = 22050;
  out.begin(cfg);
  espeak_set_audio_output(&out);

  // setup espeak
  Serial.println("espeak_Initialize");
  espeak_Initialize(output, buflength, path, options);
}

void loop() {
  Serial.print("espeak_Synth ");
  char text[] = "Hello world!";
  Serial.println(text);
  espeak_Synth(text, buflength, position, position_type, end_position, flags,
               identifier, user_data);
  Serial.println("Done");
  delay(5000);
}