/**
 * @file espeak-min.ino
 * @author Phil Schatzmann
 * @brief Functional API - minimum example. The espeak-ng-data is stored on in
 * progmem with the arduino-posix-fs library and we output audio to I2S with the
 * help of the AudioTools library
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "AudioTools.h" // https://github.com/pschatzmann/arduino-audio-tools
//#include "AudioLibs/AudioKit.h"
#include "FileSystems.h" // https://github.com/pschatzmann/arduino-posix-fs
#include "espeak.h"

I2SStream i2s; // or replace with AudioKitStream for AudioKit
file_systems::FileSystemMemory fsm("/mem");
espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
char *path = "/mem/data"; 
void *user_data = nullptr;
unsigned int *identifier = nullptr;

void setup() {
  Serial.begin(115200);
  // setup file system
  fsm.add("/mem/data/en_dict", espeak_ng_data_en_dict,espeak_ng_data_en_dict_len);
  fsm.add("/mem/data/phonindex", espeak_ng_data_phonindex,espeak_ng_data_phonindex_len);
  fsm.add("/mem/data/phontab", espeak_ng_data_phontab,espeak_ng_data_phontab_len);
  fsm.add("/mem/data/phondata", espeak_ng_data_phondata,espeak_ng_data_phondata_len);
  fsm.add("/mem/data/phondata-manifest", espeak_ng_data_phondata_manifest,espeak_ng_data_phondata_manifest_len);

  // setup output
  auto cfg = i2s.defaultConfig();
  i2s.begin(cfg);
  espeak_set_audio_output(&i2s);

  // setup espeak
  char voicename[] = {"English"}; // Set voice by its name
  char text[] = {"Hello world!"};
  int buflength = 500, options = 0;
  unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
  espeak_POSITION_TYPE position_type = POS_CHARACTER;
  Serial.println("espeak_Initialize");
  espeak_Initialize(output, buflength, path, options);
  espeak_SetVoiceByName(voicename);
  Serial.print("espeak_Synth ");
  Serial.println(text);
  espeak_Synth(text, buflength, position, position_type, end_position, flags,
               identifier, user_data);
  Serial.println("Done");
}

void loop() {}