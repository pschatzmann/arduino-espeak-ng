#pragma once
#include "config-espk.h"
#include "speak_lib.h"
#include "pcaudiolib/audio_object.h"
#include "espeak-ng-data.h"
#include "libespeak-ng/mem_alloc.h"

#if USE_CPP_API
#include "FileSystems.h" // https://github.com/pschatzmann/arduino-posix-fs
#include "Print.h"
#define GUARD_VALUE 23
/**
 * @brief Simple Arduino C++ class API for ESpeak using files
 */
class ESpeakFiles {
public:
    /// Default constructor: define the output later via setOutput()
    ESpeakFiles(const char* path="../../../espeak-ng-data"){
        if (path!=nullptr){
            this->path = path;
        }
    }

    ESpeakFiles(Print &out, const char* path="../../../espeak-ng-data") : ESpeakFiles(path){
        setOutput(out);
    }

    /// Defines the output to which the generated audio is written
    void setOutput(Print &out){
        espeak_set_audio_output(&out);
    }

    /// Provides information about the sample rate, channels ....
    audio_info audioInfo() { return espeak_get_audio_info();}

    /// Starts the processing. Optionally add all relevant configuration files be fore calling this method.
    bool begin(int buflength=5) {
        int sample_rate = espeak_Initialize(output, buflength, path, options);
        return sample_rate!=-1;
    }

    /// PSRAM is used automatically by default for the library's heap allocations
    /// whenever it is physically present on the board -- no call needed. Use this
    /// to opt out (false) and force everything into internal RAM, or to opt back in
    /// (true) after doing so. Call before begin() so it applies to the buffers set
    /// up during initialization. Has no effect on platforms without PSRAM support
    /// (e.g. desktop builds).
    void setUsePSRAM(bool enable) {
        espeak_SetUsePsram(enable ? 1 : 0);
    }

    /// Determines if the use of PSRAM is currently active (true by default when supported)
    bool isUsePSRAM() {
        return espeak_GetUsePsram() != 0;
    }

    /// Determines if PSRAM is physically present and usable on this device
    bool isPSRAMAvailable() {
        return espeak_PsramAvailable() != 0;
    }

    // defines a callback which is executed at the begin of the audio output
    void setOutputBeginCallback(void(*cb)()){
        audio_stream_start_callback=cb;
    }

    // defines a callback which is executed at the end of the audio output
    void setOutputEndCallback(void(*cb)()){
        audio_stream_stop_callback=cb;
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
        if (str==nullptr) return false;
        assert(memory_guard==GUARD_VALUE);
        espeak_ERROR rc = espeak_Synth(str, strlen(str)+1, position, position_type, end_position, flags,
                    identifier, user_data);
        assert(memory_guard==GUARD_VALUE);
        return rc==EE_OK;
    }

    /// speaking speed in word per minute. Values 80 to 450.
    bool setRate(int rate) {
        return setParameter(espeakRATE, rate);
    }

    /// volume in range 0-200 or more. 0=silence, 100=normal full volume, greater values may produce amplitude compression or distortion
    bool setVolume(int vol){
        return setParameter(espeakVOLUME, vol);
    }

    /// PITCH: base pitch, range 0-100. 50=normal
    bool setPitch(int pitch){
        return setParameter(espeakPITCH, pitch);
    }

    /// RANGE: pitch range, range 0-100. 0-monotone, 50=normal
    bool setPitchRange(int range){
        return setParameter(espeakRANGE, range);
    }

    /// PUNCTUATION: which punctuation characters to announce: value in espeak_PUNCT_TYPE (none, all, some), see espeak_GetParameter() to specify which characters are announced.
    bool setPunctuation(espeak_PUNCT_TYPE punct){
        return setParameter(espeakPUNCTUATION, punct);
    }

    /// CAPITALS: announce capital letters by: 0=none, 1=sound icon, 2=spelling, 3 or higher, by raising pitch. This values gives the amount in Hz by which the pitch of a word raised to indicate it has a capital letter.
    bool setAnnounceCapitals(int caps){
        return setParameter(espeakCAPITALS, caps);
    }

    /// WORDGAP: pause between words, units of 10mS (at the default speed)
    bool setWordGap(int gap){
        return setParameter(espeakWORDGAP, gap);
    }

    /// Sets Parameter: espeakRATE, espeakVOLUME etc
    bool setParameter(espeak_PARAMETER parameter, int value, int relative=0){
        return espeak_SetParameter(parameter, value, relative)==EE_OK;
    }

    /// sets the flags espeakCHARS_UTF8,espeakCHARS_8BIT,espeakCHARS_WCHAR (by default we use espeakCHARS_AUTO)
    bool setFlags(int flag){
        flags = flag;
        return true;
    }

    /// Converts a string to phonemes
    const char* toPhonemes(const char* str, int charset=espeakCHARS_UTF8, int phonememode=espeakPHONEMES){
        if (str==nullptr) return "";
        return espeak_TextToPhonemes((const void**)&str, charset, phonememode);
    }

protected:
    const short memory_guard = GUARD_VALUE; // check that memory was not overwritten by stack overflow
    const espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
    const char* path = "../../../espeak-ng-data-min"; 
    void *user_data = nullptr;
    int options = 0;
    unsigned int *identifier = nullptr;
    unsigned int position = 0;
    unsigned int  end_position = 0;
    unsigned int  flags = espeakCHARS_AUTO;
    espeak_POSITION_TYPE position_type = POS_CHARACTER;
};
/**
 * @brief Simple Arduino C++ class API for ESpeak using PROGMEM
 * 
 */

class ESpeak :  public ESpeakFiles {
public:
    /// Default constructor: define the output later via setOutput()
    ESpeak(bool setupEnglish=true) : ESpeakFiles("/mem/data"){
        is_setup_english = setupEnglish;
    }

    ESpeak(Print &out, bool setupEnglish=true) : ESpeakFiles(out, "/mem/data"){
        // setup min file system
        is_setup_english = setupEnglish;
    }
    /// Adds a configuration file (e.g dictionary or lang). Use the prefix /mem/data/ for the name!
    void add(const char* fileName, const void* fileContent, size_t len){
        fsm.add(fileName, fileContent, len);
    }

    bool begin(int buflength=500) {
        if (!is_fs_setup){
            add("/mem/data/phontab", espeak_ng_data_phontab,espeak_ng_data_phontab_len);
            add("/mem/data/phonindex", espeak_ng_data_phonindex,espeak_ng_data_phonindex_len);
            add("/mem/data/phondata", espeak_ng_data_phondata,espeak_ng_data_phondata_len);
            add("/mem/data/intonations", espeak_ng_data_intonations,espeak_ng_data_intonations_len);
            if (is_setup_english){
                add("/mem/data/en_dict", espeak_ng_data_en_dict,espeak_ng_data_en_dict_len);
                add("/mem/data/lang/en", espeak_ng_data_lang_gmw_en, espeak_ng_data_lang_gmw_en_len);
            }
            is_fs_setup = true;
        }
        return ESpeakFiles::begin(buflength);
    }

protected:
    file_systems::FileSystemMemory fsm{"/mem"}; // File system data in PROGMEM
    bool is_setup_english;
    bool is_fs_setup = false;
};

#endif