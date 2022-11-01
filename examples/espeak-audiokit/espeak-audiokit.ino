/**
 * @file espeak-audiokit.ino
 * @author Phil Schatzmann
 * @brief Functional API with custom output stream
 * @version 0.1
 * @date 2022-10-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "AudioTools.h"
#include "AudioLibs/AudioKit.h"
#include "espeak.h"

AudioKitStream kit;
espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
char *path = "/sd/espeak-ng-data";
void* user_data = nullptr;
unsigned int *identifier = nullptr;

void setup() {
    Serial.begin(115200);

    // setup SD and output
    auto cfg = kit.defaultConfig();
    cfg.sd_active = true;
    kit.begin(cfg);
    espeak_set_audio_output(&kit);

    // enable allocates in psram
    const int limit = 1000;
    heap_caps_malloc_extmem_enable(limit);

    // setup espeak
    char voicename[] = "English"; // Set voice by its name
    espeak_Initialize(output, buflength, path, options);
    espeak_SetVoiceByName(voicename);
}

void loop(){
    char text[] = "Hello world!";
    Serial.println(text);
    espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
    Serial.println("Done");
    delay(5000);
}