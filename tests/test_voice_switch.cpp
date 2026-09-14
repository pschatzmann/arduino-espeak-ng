// Regression test for github.com/pschatzmann/arduino-espeak-ng/issues/35:
// "When I change voice in runtime, it causes exception on ESP32-C6"
//
// The reporter set up English and Vietnamese voices and found that saying
// text with one voice, then switching to the other (or even re-selecting
// the same voice) crashed. The root cause was that SelectTranslator()
// (tr_languages.c) dereferenced the pointer returned by NewTranslator()
// without checking for allocation failure, and LoadVoice() (voices.c) did
// the same with the pointer returned by SelectTranslator(). Both only
// ever return NULL when the allocator is out of memory, but on a
// constrained/fragmented heap (as on a microcontroller) that is exactly
// when repeatedly switching voices is most likely to happen.
//
// This test drives the real English and Vietnamese dictionary/language
// data through the same in-memory filesystem the ESpeak Arduino wrapper
// uses, linked against the arduino-espeak-ng-arduinosim library, and
// switches voices back and forth the way the issue describes.
#define PROGMEM
#include "FileSystems.h"
#include "data/en_dict.h"
#include "data/vi_dict.h"
#include "data/phontab.h"
#include "data/phonindex.h"
#include "data/phondata.h"
#include "data/intonations.h"
#include "data/lang/gmw/en.h"
#include "data/lang/aav/vi.h"
extern "C" {
#include "speak_lib.h"
}
#include <clocale>
#include "test_util.h"

static void say(const char *voice, const char *text) {
  espeak_ERROR vrc = espeak_SetVoiceByName(voice);
  CHECK(vrc == EE_OK, "espeak_SetVoiceByName should succeed");

  const void *tp = text;
  const char *ph = espeak_TextToPhonemes(&tp, espeakCHARS_UTF8, 0);
  CHECK(ph != nullptr, "espeak_TextToPhonemes should return phonemes");
}

int main() {
  file_systems::FileSystemMemory fsm("/mem");
  fsm.add("/mem/data/phontab", espeak_ng_data_phontab, espeak_ng_data_phontab_len);
  fsm.add("/mem/data/phonindex", espeak_ng_data_phonindex, espeak_ng_data_phonindex_len);
  fsm.add("/mem/data/phondata", espeak_ng_data_phondata, espeak_ng_data_phondata_len);
  fsm.add("/mem/data/intonations", espeak_ng_data_intonations, espeak_ng_data_intonations_len);
  fsm.add("/mem/data/en_dict", espeak_ng_data_en_dict, espeak_ng_data_en_dict_len);
  fsm.add("/mem/data/lang/en", espeak_ng_data_lang_gmw_en, espeak_ng_data_lang_gmw_en_len);
  fsm.add("/mem/data/vi_dict", espeak_ng_data_vi_dict, espeak_ng_data_vi_dict_len);
  fsm.add("/mem/data/lang/vi", espeak_ng_data_lang_aav_vi, espeak_ng_data_lang_aav_vi_len);

  int rate = espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK, 500, "/mem/data", 0);
  CHECK(rate > 0, "espeak_Initialize should succeed");

  std::setlocale(LC_CTYPE, "C");

  // Same sequence the issue reports: set a voice and say something, then
  // switch to the other voice (and back, and to the same voice again).
  say("en", "Hello, World!");
  say("vi", "Xin ch\xc3\xa0o");
  say("en", "Hello again!");
  say("en", "Hello again!"); // re-selecting the same voice
  say("vi", "Xin ch\xc3\xa0o");

  TEST_EXIT();
}
