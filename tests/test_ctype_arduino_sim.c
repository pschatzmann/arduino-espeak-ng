// Regression test for the ARDUINO build of IsAlpha()/IsDigit()/IsSpace() in
// src/libespeak-ng/common.c (github.com/pschatzmann/arduino-espeak-ng/issues/34).
//
// On embedded targets (ESP32, RP2040, ...) libc's iswalpha()/iswdigit()/
// iswspace() only recognise plain ASCII: their minimal newlib has no locale
// data, so setlocale() (which espeak_Initialize() calls, see speech.c) is a
// no-op there and these wide-char classifiers never see anything but the "C"
// locale. Deliberately NOT calling setlocale() here reproduces that: this
// binary is linked against the arduino-espeak-ng-arduinosim library, which is
// the same source compiled with -DARDUINO, so IsAlpha/IsDigit/IsSpace use the
// self-contained ucd_isalpha()/ucd_isdigit()/ucd_isspace() Unicode tables
// instead of libc, and must classify these codepoints correctly regardless of
// locale.
#include "libespeak-ng/common.h"
#include "test_util.h"

int main(void) {
  // Plain ASCII must always classify correctly.
  CHECK(IsAlpha('a'), "ASCII letter");
  CHECK(IsAlpha('Z'), "ASCII letter");
  CHECK(!IsAlpha('5'), "ASCII digit is not alpha");
  CHECK(IsDigit('5'), "ASCII digit");
  CHECK(!IsDigit('a'), "ASCII letter is not digit");
  CHECK(IsSpace(' '), "ASCII space");
  CHECK(IsSpace('\t'), "ASCII tab");

  // Without a working locale, libc's iswalpha(0xE0) returns false -- this is
  // exactly the bug from issue #34 ("chào" spoken as separate letters). The
  // ucd_isalpha()-based ARDUINO path must get this right anyway.
  CHECK(IsAlpha(0x00E0), "a-grave (à, U+00E0)");
  CHECK(IsAlpha(0x0103), "a-breve (ă, U+0103)");
  CHECK(IsAlpha(0x1EAD), "a-circumflex-dot-below (ậ, U+1EAD)");
  CHECK(IsAlpha(0x1EDD), "o-horn-grave (ờ, U+1EDD)");

  // Devanagari digit: handled by IsDigit()'s own explicit range check,
  // regardless of the ucd_isdigit()/iswdigit() fallback.
  CHECK(IsDigit(0x0966), "Devanagari digit zero");

  // ucd_isspace() correctly classifies general Unicode space separators
  // beyond ASCII (e.g. U+2028 LINE SEPARATOR), independent of libc.
  CHECK(IsSpace(0x2028), "Unicode line separator");

  TEST_EXIT();
}
