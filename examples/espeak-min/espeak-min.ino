/**
 * @file espeak-min.ino
 * @author Phil Schatzmann
 * @brief Functional API - minimum example. The espeak-ng-data is stored on an sd drive
 * in the /espeak-ng-data directory
 * @version 0.1
 * @date 2022-10-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "SPI.h"
#include "SD.h"
#include "espeak.h"

#define PIN_SD_CARD_CS 13  
#define PIN_SD_CARD_MISO 2
#define PIN_SD_CARD_MOSI 15
#define PIN_SD_CARD_CLK  14

espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
char *path = "/sd/espeak-ng-data";
void* user_data = nullptr;
unsigned int *identifier = nullptr;

void setupSD() {
// setup SD
  SPI.begin(PIN_SD_CARD_CLK, PIN_SD_CARD_MISO, PIN_SD_CARD_MOSI, PIN_SD_CARD_CS);
  // "sd" is default mount point
  while(!SD.begin(PIN_SD_CARD_CS)){
    delay(500);
  }
  
  // list existing directory content
  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir(path)) == NULL)
    Serial.println("opendir() error");
  else {
    Serial.println("contents:");
    while ((entry = readdir(dir)) != NULL)
      Serial.println(entry->d_name);
    closedir(dir);
  }
}

void setup() {
    Serial.begin(115200);

    setupSD();

    // enable allocates in psram
    const int limit = 1000;
    heap_caps_malloc_extmem_enable(limit);

    // setup espeak
    char voicename[] = {"English"}; // Set voice by its name
    char text[] = {"Hello world!"};
    int buflength = 500, options = 0;
    unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
    espeak_POSITION_TYPE position_type=POS_CHARACTER;
    Serial.println("espeak_Initialize");
    espeak_Initialize(output, buflength, path, options);
    espeak_SetVoiceByName(voicename);
    Serial.print("espeak_Synth ");
    Serial.println(text);
    espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
    Serial.println("Done");
}

void loop(){}