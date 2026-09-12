// End-to-end regression test for github.com/pschatzmann/arduino-espeak-ng/issues/34:
// "espeak can't say the whole word for Vietnamese language" -- accented words
// like "chào" were spoken as separate letters ("c", "h", "à", "o") instead of
// as one word, because IsAlpha() misclassified the accented letter as
// non-alphabetic (see tests/test_ctype_arduino_sim.c for the root cause).
//
// This test drives the real Vietnamese dictionary/language data through the
// same in-memory filesystem the ESpeak Arduino wrapper uses, linked against
// the arduino-espeak-ng-arduinosim library (compiled with -DARDUINO) so it
// exercises the exact code path embedded targets use, and checks that
// "Xin chào" translates into phonemes for two words, not four+ spelled-out
// letters.
#define PROGMEM
#include "FileSystems.h"
#include "data/vi_dict.h"
#include "data/phontab.h"
#include "data/phonindex.h"
#include "data/phondata.h"
#include "data/intonations.h"
#include "data/lang/aav/vi.h"
extern "C" {
#include "speak_lib.h"
}
#include <clocale>
#include <cstring>
#include "test_util.h"

int main() {
  file_systems::FileSystemMemory fsm("/mem");
  fsm.add("/mem/data/phontab", espeak_ng_data_phontab, espeak_ng_data_phontab_len);
  fsm.add("/mem/data/phonindex", espeak_ng_data_phonindex, espeak_ng_data_phonindex_len);
  fsm.add("/mem/data/phondata", espeak_ng_data_phondata, espeak_ng_data_phondata_len);
  fsm.add("/mem/data/intonations", espeak_ng_data_intonations, espeak_ng_data_intonations_len);
  fsm.add("/mem/data/vi_dict", espeak_ng_data_vi_dict, espeak_ng_data_vi_dict_len);
  fsm.add("/mem/data/lang/vi", espeak_ng_data_lang_aav_vi, espeak_ng_data_lang_aav_vi_len);

  int rate = espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK, 500, "/mem/data", 0);
  CHECK(rate > 0, "espeak_Initialize should succeed");

  // espeak_Initialize() calls setlocale(LC_CTYPE, "C.UTF-8") (speech.c), which
  // succeeds on desktop glibc and would mask this bug: it would make the
  // libc-based IsAlpha() fallback (the pre-fix, non-ARDUINO code path)
  // classify accented letters correctly even without today's fix, since that
  // fallback is what's being tested here. Embedded targets have no such
  // locale to fall back on -- setlocale() there is a no-op and iswalpha()
  // never sees anything but the "C" locale. Resetting to "C" here reproduces
  // that on desktop, so this test actually exercises the fixed
  // (ucd_isalpha()-based) ARDUINO path instead of accidentally passing
  // because glibc's locale support papered over the bug.
  std::setlocale(LC_CTYPE, "C");

  espeak_ERROR vrc = espeak_SetVoiceByName("vi");
  CHECK(vrc == EE_OK, "espeak_SetVoiceByName(\"vi\") should succeed");

  const char *text = "Xin ch\xc3\xa0o"; // "Xin chào"
  const void *tp = text;
  const char *ph = espeak_TextToPhonemes(&tp, espeakCHARS_UTF8, 0);
  CHECK(ph != nullptr, "espeak_TextToPhonemes should return phonemes");

  printf("phonemes: %s\n", ph ? ph : "(null)");

  // Two words ("Xin" and "chào") means one word-separator ("_|") followed by
  // the final one. If "chào" gets spelled out letter by letter, each letter
  // becomes its own phoneme chunk and this count goes well above 2.
  int word_pauses = 0;
  if (ph != nullptr) {
    const char *p = ph;
    while ((p = strstr(p, "_|")) != nullptr) {
      word_pauses++;
      p += 2;
    }
  }
  CHECK(word_pauses == 2, "\"Xin chào\" should translate as exactly two words");

  TEST_EXIT();
}
