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
char *path = nullptr;
void* user_data = nullptr;
unsigned int *identifier = nullptr;

// callback method to setup output
AudioStream *setupStream(AudioBaseInfo &info){
    auto cfg = kit.defaultConfig();
    cfg.copyFrom(info);
    kit.begin(cfg);
    return &kit;
}

void setup() {
    Serial.begin(115200);
    // assign callback method
    audio_stream_factory_callback = setupStream;
    char voicename[] = {"English"}; // Set voice by its name
    char text[] = {"Hello world!"};
    int buflength = 500, options = 0;
    unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
    espeak_POSITION_TYPE position_type=POS_CHARACTER;
    espeak_Initialize(output, buflength, path, options);
    espeak_SetVoiceByName(voicename);
    Serial.println(text);
    espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
    Serial.println("Done");
}

void loop(){}