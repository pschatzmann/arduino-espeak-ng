// Regression test for the desktop (non-ARDUINO) build of IsAlpha()/IsDigit()/
// IsSpace() in src/libespeak-ng/common.c. On desktop these fall back to the
// libc wide-char classifiers (iswalpha/iswdigit/iswspace), which only
// classify non-ASCII codepoints correctly once a UTF-8 locale is active --
// exactly what espeak_Initialize() sets up via setlocale() in speech.c. This
// test mirrors that by calling setlocale() itself, so it validates normal
// desktop behaviour keeps working, without needing the rest of espeak-ng.
#include <locale.h>
#include "libespeak-ng/common.h"
#include "test_util.h"

int main(void) {
  // Match what espeak_Initialize() does in speech.c so this test reflects
  // real desktop usage, not the raw (locale-less) libc default.
  if (setlocale(LC_CTYPE, "C.UTF-8") == NULL)
    setlocale(LC_CTYPE, "");

  // Plain ASCII must always classify correctly.
  CHECK(IsAlpha('a'), "ASCII letter");
  CHECK(IsAlpha('Z'), "ASCII letter");
  CHECK(!IsAlpha('5'), "ASCII digit is not alpha");
  CHECK(IsDigit('5'), "ASCII digit");
  CHECK(!IsDigit('a'), "ASCII letter is not digit");
  CHECK(IsSpace(' '), "ASCII space");
  CHECK(IsSpace('\t'), "ASCII tab");

  // Vietnamese/accented Latin letters (github.com/pschatzmann/arduino-espeak-ng/issues/34):
  // with a UTF-8 locale active, libc's iswalpha() correctly classifies these.
  CHECK(IsAlpha(0x00E0), "a-grave (à, U+00E0)");
  CHECK(IsAlpha(0x0103), "a-breve (ă, U+0103)");
  CHECK(IsAlpha(0x1EAD), "a-circumflex-dot-below (ậ, U+1EAD)");
  CHECK(IsAlpha(0x1EDD), "o-horn-grave (ờ, U+1EDD)");

  // Devanagari digit: handled by IsDigit()'s own explicit range check,
  // regardless of libc/locale support.
  CHECK(IsDigit(0x0966), "Devanagari digit zero");

  TEST_EXIT();
}
