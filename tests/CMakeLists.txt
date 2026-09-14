# Tests for github.com/pschatzmann/arduino-espeak-ng/issues/34 and its fix
# ("fix broken ISAlpha()"): IsAlpha()/IsDigit()/IsSpace() in common.c must
# classify non-ASCII letters (Vietnamese, etc.) correctly both on desktop
# (where libc's iswalpha() etc. work once espeak_Initialize() sets a UTF-8
# locale) and on embedded targets (where that locale setup is unavailable,
# so the ARDUINO build uses the self-contained ucd_isalpha() Unicode tables
# instead of libc).

add_executable(test_ctype_desktop test_ctype_desktop.c)
target_link_libraries(test_ctype_desktop arduino-espeak-ng)
add_test(NAME test_ctype_desktop COMMAND test_ctype_desktop)

add_executable(test_ctype_arduino_sim test_ctype_arduino_sim.c)
target_link_libraries(test_ctype_arduino_sim arduino-espeak-ng-arduinosim)
add_test(NAME test_ctype_arduino_sim COMMAND test_ctype_arduino_sim)

add_executable(test_vietnamese_phonemes test_vietnamese_phonemes.cpp)
target_link_libraries(test_vietnamese_phonemes arduino-espeak-ng-arduinosim arduino-posix-fs arduino_emulator)
add_test(NAME test_vietnamese_phonemes COMMAND test_vietnamese_phonemes)

# Regression test for github.com/pschatzmann/arduino-espeak-ng/issues/35
# ("When I change voice in runtime, it causes exception on ESP32-C6"):
# switching between voices at runtime must not crash.
add_executable(test_voice_switch test_voice_switch.cpp)
target_link_libraries(test_voice_switch arduino-espeak-ng-arduinosim arduino-posix-fs arduino_emulator)
add_test(NAME test_voice_switch COMMAND test_voice_switch)
