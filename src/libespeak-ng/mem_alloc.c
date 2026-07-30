#include "mem_alloc.h"
#include "config-espk.h"
#include <stdlib.h>
#include <stdio.h>

// esp-idf (and therefore Arduino-ESP32, which is built on top of it)
// defines ESP_PLATFORM. That gives us heap_caps_*(), which lets us request
// memory from PSRAM specifically.
#if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
#include <esp_heap_caps.h>
#define ESPEAK_PSRAM_BACKEND_ESP32 1

// arduino-pico (earlephilhower core) uses a single ARDUINO_ARCH_RP2040
// architecture define for both genuine RP2040 boards and RP2350 boards
// (e.g. Pico 2). PSRAM itself is only wired up on specific RP2350 boards,
// which is handled inside mem_alloc_rp2350.cpp; genuine RP2040 chips have
// no PSRAM controller in silicon, so the shim is a no-op there.
#elif defined(ARDUINO_ARCH_RP2040)
#include "mem_alloc_rp2350.h"
#define ESPEAK_PSRAM_BACKEND_RP2350 1
#endif

#if defined(ESPEAK_PSRAM_BACKEND_ESP32) || defined(ESPEAK_PSRAM_BACKEND_RP2350)
#define ESPEAK_HAVE_PSRAM_API 1
#else
#define ESPEAK_HAVE_PSRAM_API 0
#endif

// PSRAM is used automatically whenever it's physically available, without
// requiring an explicit opt-in. The backing allocators simply fail (and we
// fall back to internal RAM) on boards that have no PSRAM, so this is safe
// to leave on by default. Call espeak_SetUsePsram(0) to force every
// allocation into internal RAM regardless of what's fitted.
static int use_psram = ESPEAK_HAVE_PSRAM_API;

void espeak_SetUsePsram(int enable)
{
#if ESPEAK_HAVE_PSRAM_API
	use_psram = enable;
#else
	(void)enable;
#endif
}

int espeak_GetUsePsram(void)
{
	return use_psram;
}

int espeak_PsramAvailable(void)
{
#if defined(ESPEAK_PSRAM_BACKEND_ESP32)
	return heap_caps_get_free_size(MALLOC_CAP_SPIRAM) > 0;
#elif defined(ESPEAK_PSRAM_BACKEND_RP2350)
	return espeak_rp2350_psram_available();
#else
	return 0;
#endif
}

// Callers throughout the library (phonemelist.c, translateword.c, voices.c,
// etc.) follow espeak_malloc()/espeak_calloc() with assert(ptr != NULL), so a
// failed allocation crashes the board. Log heap state here -- the one place
// every allocation funnels through -- so a crash report at least tells you
// how much was requested and how much was actually free/fragmented.
static void espeak_log_oom(const char *fn, size_t requested)
{
#if defined(ESPEAK_PSRAM_BACKEND_ESP32)
	ESPK_LOG("%s: allocation of %u bytes failed (free_internal=%u largest_internal_block=%u free_psram=%u)\n",
		fn, (unsigned)requested,
		(unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
		(unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
		(unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
#else
	ESPK_LOG("%s: allocation of %u bytes failed\n", fn, (unsigned)requested);
#endif
}

void *espeak_malloc(size_t size)
{
	if (use_psram) {
#if defined(ESPEAK_PSRAM_BACKEND_ESP32)
		void *p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		if (p != NULL)
			return p;
#elif defined(ESPEAK_PSRAM_BACKEND_RP2350)
		void *p = espeak_rp2350_pmalloc(size);
		if (p != NULL)
			return p;
#endif
		// PSRAM unavailable or exhausted: fall back to internal RAM.
	}
	void *p = malloc(size);
	if (p == NULL)
		espeak_log_oom("espeak_malloc", size);
	return p;
}

void *espeak_calloc(size_t nmemb, size_t size)
{
	if (use_psram) {
#if defined(ESPEAK_PSRAM_BACKEND_ESP32)
		void *p = heap_caps_calloc(nmemb, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		if (p != NULL)
			return p;
#elif defined(ESPEAK_PSRAM_BACKEND_RP2350)
		void *p = espeak_rp2350_pcalloc(nmemb, size);
		if (p != NULL)
			return p;
#endif
	}
	void *p = calloc(nmemb, size);
	if (p == NULL)
		espeak_log_oom("espeak_calloc", nmemb * size);
	return p;
}

void *espeak_realloc(void *ptr, size_t size)
{
#if defined(ESPEAK_PSRAM_BACKEND_ESP32)
	if (use_psram) {
		void *p = heap_caps_realloc(ptr, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		if (p != NULL || size == 0)
			return p;
	}
#endif
	// On RP2350, arduino-pico's own realloc() already detects PSRAM-owned
	// pointers transparently and does the right thing, so plain realloc()
	// is correct there too (as it is on platforms with no PSRAM backend).
	return realloc(ptr, size);
}

void espeak_free(void *ptr)
{
	free(ptr);
}
