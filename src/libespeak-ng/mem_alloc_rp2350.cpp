#include "mem_alloc_rp2350.h"

// PSRAM is only wired up on specific RP2350 boards; the arduino-pico core
// sets RP2350_PSRAM_CS (to the CS GPIO number) when the board variant has
// it. pmalloc()/pcalloc()/rp2040.getPSRAMSize() are declared in Arduino.h,
// which is C++-only, hence this separate .cpp shim.
#if defined(RP2350_PSRAM_CS)
#include <Arduino.h>
#define ESPEAK_HAVE_RP2350_PSRAM 1
#endif

extern "C" {

int espeak_rp2350_psram_available(void)
{
#if ESPEAK_HAVE_RP2350_PSRAM
	return rp2040.getPSRAMSize() > 0 ? 1 : 0;
#else
	return 0;
#endif
}

void *espeak_rp2350_pmalloc(size_t size)
{
#if ESPEAK_HAVE_RP2350_PSRAM
	return pmalloc(size);
#else
	(void)size;
	return NULL;
#endif
}

void *espeak_rp2350_pcalloc(size_t nmemb, size_t size)
{
#if ESPEAK_HAVE_RP2350_PSRAM
	return pcalloc(nmemb, size);
#else
	(void)nmemb;
	(void)size;
	return NULL;
#endif
}

} // extern "C"
