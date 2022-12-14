/**
 * @file espeak.ino
 * @author Phil Schatzmann
 * @brief Functional API example with additional information
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "AudioTools.h"
//#include "FileSystems.h" // https://github.com/pschatzmann/arduino-posix-fs
//#include "AudioLibs/AudioKit.h" // https://github.com/pschatzmann/arduino-audiokit
#include "espeak.h"
#include "SD.h"
#include "SPI.h"

#define PIN_SD_CARD_CS 13
#define PIN_SD_CARD_MISO 2
#define PIN_SD_CARD_MOSI 15
#define PIN_SD_CARD_CLK 14

I2SStream i2s; // or replace with AudioKitStream for AudioKit
const char* path = "/sd/espeak-ng-data";
//file_systems::FileSystemSD efs(path, SD); // dummy implementation on ESP32
espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
void *user_data = nullptr;
unsigned int *identifier = nullptr;
int buflength = 500, options = 0;
unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
espeak_POSITION_TYPE position_type = POS_CHARACTER;

void setup() {
  Serial.begin(115200);
  // we load the config data from SD: "sd" is default mount point
  Serial.println("starting SD");
  SPI.begin(PIN_SD_CARD_CLK, PIN_SD_CARD_MISO, PIN_SD_CARD_MOSI,
            PIN_SD_CARD_CS);
  while (!SD.begin(PIN_SD_CARD_CS)) {
    delay(500);
  }

  // setup audio
  Serial.println("starting i2s");
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
  espeak_VOICE voice;
  memset(&voice, 0, sizeof(espeak_VOICE)); // Zero out the voice first
  const char *langNativeString = "en";     // Set voice by properties
  voice.languages = langNativeString;
  voice.name = "US";
  voice.variant = 2;
  voice.gender = 2;
  Serial.println("espeak_SetVoiceByProperties");
  espeak_SetVoiceByProperties(&voice);
}

void loop() {
  char text[] = "Hello world!";
  Serial.println(text);
  espeak_Synth(text, buflength, position, position_type, end_position, flags,
                identifier, user_data);
  Serial.println("Done");
  delay(5000);
}