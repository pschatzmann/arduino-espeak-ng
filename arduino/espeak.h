#pragma once
#include "config.h"
#include "speak_lib.h"
#include "pcaudiolib/audio_object.h"
#include "espeak-ng-data.h"

#if USE_CPP_API
#include "FileSystems.h" // https://github.com/pschatzmann/arduino-posix-fs
#include "Print.h"

/**
 * @brief Simple Arduino C++ class API for ESpeak
 */
class ESpeak {
public:
    /// Constructor
    ESpeak(Print &out, bool setupEnglish=true){
        // setup min file system
        espeak_set_audio_output(&out);
        add("/mem/data/phontab", espeak_ng_data_phontab,espeak_ng_data_phontab_len);
        add("/mem/data/phonindex", espeak_ng_data_phonindex,espeak_ng_data_phonindex_len);
        add("/mem/data/phondata", espeak_ng_data_phondata,espeak_ng_data_phondata_len);
        add("/mem/data/intonations", espeak_ng_data_intonations,espeak_ng_data_intonations_len);
        if (setupEnglish){
            add("/mem/data/en_dict", espeak_ng_data_en_dict,espeak_ng_data_en_dict_len);
            add("/mem/data/lang/en", espeak_ng_data_lang_gmw_en, espeak_ng_data_lang_gmw_en_len);
        }
    }

    /// Provides information about the sample rate, channels ....
    audio_info audioInfo() { return espeak_get_audio_info();}

    /// Adds a configuration file (e.g dictionary or lang). Use the prefix /mem/data/ for the name!
    void add(const char* fileName, const void* fileContent, size_t len){
        fsm.add(fileName, fileContent, len);
    }

    /// Starts the processing. Optionally add all relevant configuration files be fore calling this method.
    bool begin(int buflength=0) {
        int sample_rate = espeak_Initialize(output, buflength, path, options);
        return sample_rate!=-1;
    }

    /// Defines the voice by i'ts name
    bool setVoice(const char *voiceName){
        espeak_ERROR rc = espeak_SetVoiceByName(voiceName);
        return rc==EE_OK;
    }

    /// Defines the voice by it's properties
    bool setVoice(espeak_VOICE *voice_spec){
        espeak_ERROR rc = espeak_SetVoiceByProperties (voice_spec);
        return rc==EE_OK;
    }

    /// Determiens the current voice
    espeak_VOICE *voice() {
        return espeak_GetCurrentVoice ();
    }

    /// Stops the processing
    void end() {
        espeak_Terminate();
    }

    /// Outputs the string as sound
    bool say(const char* str){
        espeak_ERROR rc = espeak_Synth(str, strlen(str), position, position_type, end_position, flags,
                    identifier, user_data);
        return rc==EE_OK;
    }

protected:
    file_systems::FileSystemMemory fsm = file_systems::FileSystemMemory("/mem"); // File system data in PROGMEM
    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
    const char* path = "/mem/data"; 
    void *user_data = nullptr;
    unsigned int *identifier = nullptr;
    int options = 0;
    unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
    espeak_POSITION_TYPE position_type = POS_CHARACTER;
};

#endif