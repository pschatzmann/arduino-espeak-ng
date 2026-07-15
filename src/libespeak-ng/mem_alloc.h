/*
 * Central heap allocation API for espeak-ng.
 *
 * All malloc/calloc/realloc/free calls in this library are routed through
 * espeak_malloc()/espeak_calloc()/espeak_realloc()/espeak_free() so that a
 * single place controls where memory comes from. On ESP32 (heap_caps_*) and
 * on RP2350 boards with PSRAM fitted (arduino-pico's pmalloc()/pcalloc(),
 * gated on RP2350_PSRAM_CS -- see mem_alloc_rp2350.cpp), PSRAM is used
 * automatically whenever it is physically available -- no opt-in call is
 * needed -- falling back to internal RAM if the PSRAM allocation fails or
 * no PSRAM is fitted. Use espeak_SetUsePsram(0) to opt back out (e.g. if
 * PSRAM's extra access latency is undesirable for a particular buffer).
 * Genuine RP2040 chips have no PSRAM controller in silicon and are
 * unaffected. On platforms without PSRAM support (e.g. desktop builds),
 * these calls always behave exactly like the standard library functions.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *espeak_malloc(size_t size);
void *espeak_calloc(size_t nmemb, size_t size);
void *espeak_realloc(void *ptr, size_t size);
void espeak_free(void *ptr);

// Enable (1) or disable (0) use of PSRAM for allocations made from this
// point onward. Enabled by default on platforms with PSRAM support (it is
// still only actually used when PSRAM hardware is detected at allocation
// time). No effect on platforms without PSRAM support (e.g. desktop
// builds); allocations there always use the standard heap.
void espeak_SetUsePsram(int enable);

// Returns non-zero if PSRAM use has been activated with espeak_SetUsePsram().
int espeak_GetUsePsram(void);

// Returns non-zero if PSRAM is physically present and usable on this
// device, regardless of whether espeak_SetUsePsram() has been called.
int espeak_PsramAvailable(void);

#ifdef __cplusplus
}
#endif
