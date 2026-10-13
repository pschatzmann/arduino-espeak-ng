// Regression test for github.com/pschatzmann/arduino-espeak-ng/issues/37:
// 'Words containing "fi"/"st"/"ff"/"fl" are spoken as "letter FB0x" on
// ESP32'
//
// LoadDictionary() (dictionary.c) located the .replace table by rounding
// up the absolute memory address to a 4-byte boundary, while compiledict
// pads it relative to the start of the dictionary file. When the
// dictionary is memory-mapped in place from a PROGMEM byte array that is
// not 4-byte aligned, the table was read from the wrong byte and the
// replacements ran backwards ("st" -> U+FB06 etc.).
//
// This test places en_dict at each of the offsets 0..3 from an aligned
// buffer, maps it through the in-memory filesystem (which returns the data
// in place, like on the microcontroller) and checks that "test" and "five"
// are not turned into ligatures.
#define PROGMEM
#include "FileSystems.h"
#include "data/en_dict.h"
#include "data/phontab.h"
#include "data/phonindex.h"
#include "data/phondata.h"
#include "data/intonations.h"
#include "data/lang/gmw/en.h"
extern "C" {
#include "speak_lib.h"
}
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <string>
#include "test_util.h"

int main() {
  // aligned buffer with room for the largest offset
  unsigned char *buffer =
      (unsigned char *)aligned_alloc(4, ((espeak_ng_data_en_dict_len + 3 + 3) / 4) * 4);
  CHECK(buffer != nullptr, "allocating the dictionary buffer should succeed");
  if (buffer == nullptr) TEST_EXIT();

  file_systems::FileSystemMemory fsm("/mem");
  fsm.add("/mem/data/phontab", espeak_ng_data_phontab, espeak_ng_data_phontab_len);
  fsm.add("/mem/data/phonindex", espeak_ng_data_phonindex, espeak_ng_data_phonindex_len);
  fsm.add("/mem/data/phondata", espeak_ng_data_phondata, espeak_ng_data_phondata_len);
  fsm.add("/mem/data/intonations", espeak_ng_data_intonations, espeak_ng_data_intonations_len);
  fsm.add("/mem/data/lang/en", espeak_ng_data_lang_gmw_en, espeak_ng_data_lang_gmw_en_len);

  int rate = espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK, 500, "/mem/data", 0);
  CHECK(rate > 0, "espeak_Initialize should succeed");

  std::setlocale(LC_CTYPE, "C");

  for (int offset = 0; offset < 4; offset++) {
    unsigned char *dict = buffer + offset;
    memcpy(dict, espeak_ng_data_en_dict, espeak_ng_data_en_dict_len);
    fsm.add("/mem/data/en_dict", dict, espeak_ng_data_en_dict_len);

    // selecting the voice (re)loads the dictionary from the new location
    espeak_ERROR vrc = espeak_SetVoiceByName("en");
    CHECK(vrc == EE_OK, "espeak_SetVoiceByName should succeed");

    const void *tp = "test five";
    const char *ph = espeak_TextToPhonemes(&tp, espeakCHARS_UTF8, 0);
    CHECK(ph != nullptr, "espeak_TextToPhonemes should return phonemes");
    if (ph == nullptr) continue;

    std::string phonemes = ph;
    printf("offset %d: %s\n", offset, ph);
    CHECK(phonemes.find("t'Est") != std::string::npos, "\"test\" should be pronounced normally");
    CHECK(phonemes.find("f'aIv") != std::string::npos, "\"five\" should be pronounced normally");
  }

  free(buffer);
  TEST_EXIT();
}
