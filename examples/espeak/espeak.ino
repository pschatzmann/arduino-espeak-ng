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
 */

#include "AudioTools.h" // https://github.com/pschatzmann/arduino-audio-tools
//#include "AudioLibs/AudioKit.h" // https://github.com/pschatzmann/arduino-audiokit
#include "FileSystems.h" // https://github.com/pschatzmann/arduino-posix-fs
#include "espeak.h"

I2SStream i2s; // or replace with AudioKitStream for AudioKit
file_systems::FileSystemMemory fsm("/mem"); // File system data in PROGMEM
espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
const char* path = "/mem/data"; 
void *user_data = nullptr;
unsigned int *identifier = nullptr;
int buflength = 500, options = 0;
unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
espeak_POSITION_TYPE position_type = POS_CHARACTER;

void setup() {
  Serial.begin(115200);
  // file_systems::FSLogger.begin(file_systems::FSInfo, Serial); 

  // setup min file system
  fsm.add("/mem/data/phontab", espeak_ng_data_phontab,espeak_ng_data_phontab_len);
  fsm.add("/mem/data/phonindex", espeak_ng_data_phonindex,espeak_ng_data_phonindex_len);
  fsm.add("/mem/data/phondata", espeak_ng_data_phondata,espeak_ng_data_phondata_len);
  fsm.add("/mem/data/intonations", espeak_ng_data_intonations,espeak_ng_data_intonations_len);
  fsm.add("/mem/data/en_dict", espeak_ng_data_en_dict,espeak_ng_data_en_dict_len);
  fsm.add("/mem/data/lang/en", espeak_ng_data_lang_gmw_en, espeak_ng_data_lang_gmw_en_len);

  // setup output
  audio_info espeak_info = espeak_get_audio_info();
  auto cfg = i2s.defaultConfig();
  cfg.channels = espeak_info.channels; // 1
  cfg.sample_rate = espeak_info.sample_rate; // 22050
  cfg.bits_per_sample = espeak_info.bits_per_sample; // 16
  i2s.begin(cfg);
  espeak_set_audio_output(&i2s);

  // setup espeak
  Serial.println("espeak_Initialize");
  espeak_Initialize(output, buflength, path, options);
  //const char* voicename = "English"; // Set voice by its name
  //espeak_SetVoiceByName(voicename);
  Serial.print("espeak_Synth ");
}

void loop() {
  char text[] = "Hello my name is Phil!";
  Serial.println(text);
  espeak_Synth(text, buflength, position, position_type, end_position, flags,
               identifier, user_data);
  Serial.println("Done");
  delay(5000);
}
