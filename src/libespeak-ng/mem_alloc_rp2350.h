/*
 * C-linkage bridge to the arduino-pico core's RP2350 PSRAM allocator
 * (pmalloc()/pcalloc(), declared in the C++-only Arduino.h). Implemented
 * in mem_alloc_rp2350.cpp so that mem_alloc.c -- a plain C translation
 * unit -- can call it without depending on C++ name mangling or pulling
 * Arduino.h into a C file.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns non-zero if this board has PSRAM physically fitted (RP2350 only).
int espeak_rp2350_psram_available(void);

// Allocate from PSRAM. Returns NULL if unavailable or on RP2040/other
// platforms. Mirrors malloc()/calloc().
void *espeak_rp2350_pmalloc(size_t size);
void *espeak_rp2350_pcalloc(size_t nmemb, size_t size);

#ifdef __cplusplus
}
#endif
