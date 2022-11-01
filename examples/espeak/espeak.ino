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
#include <SD.h>
#include "espeak.h"
#include "AudioKitHAL.h"

char *path = "/data";
FileSystem efs(path, SD); // set file system
espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
void* user_data = nullptr;
unsigned int *identifier = nullptr;

void setupSD(){
  SPI.begin(PIN_AUDIO_KIT_SD_CARD_CLK, PIN_AUDIO_KIT_SD_CARD_MISO,
            PIN_AUDIO_KIT_SD_CARD_MOSI, PIN_AUDIO_KIT_SD_CARD_CS);
  while (!SD.begin(PIN_AUDIO_KIT_SD_CARD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
}

void setup() {
  Serial.begin(115200);
  setupSD();

  char text[] = {"Hello world!"};
  int buflength = 500, options = 0;
  unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
  espeak_POSITION_TYPE position_type=POS_CHARACTER;
  espeak_Initialize(output, buflength, path, options );
  espeak_VOICE voice;
  memset(&voice, 0, sizeof(espeak_VOICE)); // Zero out the voice first
  const char *langNativeString = "en"; // Set voice by properties
  voice.languages = langNativeString;
  voice.name = "US";
  voice.variant = 2;
  voice.gender = 2;
  espeak_SetVoiceByProperties(&voice);
  Serial.println(text);
  espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
  Serial.println("Done");
}

void loop(){
}