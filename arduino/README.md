This directory contains the Arduino specific implementation files.

## Why this directory is separate from src/

Arduino's library format (and this repo's CMake build) only discovers and
compiles source files under `src/`. The actual espeak-ng C sources under
`src/libespeak-ng` are a close-to-upstream vendor tree, so the
Arduino-specific platform glue is kept here instead, out of that tree.

To make these files reachable by the build, `src/` contains small one-line
forwarding stubs that just `#include` the real file from here, e.g.
`src/espeak.h` is:

```c
#include "../arduino/espeak.h"
```

This works for both headers and .cpp translation units (the preprocessor
pulls in the real content either way). The stubs and what they forward to:

| Stub under `src/`                        | Forwards to                        | Purpose |
|-------------------------------------------|-------------------------------------|---------|
| `src/config-espk.h`                       | `config-espk.h`                     | Master platform config: feature flags (`ESPEAK_STACK_HACK`, `ESPEAK_HEAP_HACK`, `ESPEAK_LOGGING`, `ESPEAK_ARDUINO_POSIX_FS`), `PATH_ESPEAK_DATA`, etc. Included by every file in `src/libespeak-ng/*.c` via `#include "config-espk.h"`. |
| `src/espeak.h`                            | `espeak.h`                          | The `ESpeakFiles` C++ convenience wrapper class used by the example sketches. Not pulled in by anything under `src/libespeak-ng`, so it is only compiled when a sketch/example includes it. |
| `src/direntx.h`                           | `direntx.h`                         | Minimal `<dirent.h>` compatibility shim, used when `ESPEAK_ARDUINO_POSIX_FS` is 0. |
| `src/pcaudiolib/audio_object.h`           | `audio_object.h`                    | Declares the `audio_object_*()` API that `speech.c` calls for audio output. |
| `src/pcaudiolib/audio.h`                  | `audio.h`                           | Audio config struct (`audio_info`, sample rate/channels/etc). |
| `src/pcaudiolib/audio_audiotools_i2s.cpp` | `audio_audiotools_i2s.cpp`          | Real implementation of `audio_object_*()`, wired to [arduino-audio-tools](https://github.com/pschatzmann/arduino-audio-tools) I2S output. Replaces upstream espeak-ng's libpcaudio, which isn't suitable for embedded targets. |

`common-ext.h` (declares `espeak_mem_map()`, used by the PROGMEM/posix-fs
memory-mapped file reading path) is included directly from
`arduino/config-espk.h` rather than via its own stub, since it's only ever
needed by files that already pull in `config-espk.h`.

`src/speak_lib.h` and `src/espeak-ng-data.h` are the two files directly
under `src/` that are *not* stubs -- they're the real public C API header
and the generated PROGMEM dictionary-data header, respectively.

## Practical implication for testing changes here

Because `arduino/espeak.h` is only reachable from example sketches, not
from anything the core library build compiles, changes to it are **not**
exercised by `cmake --build` in the top-level `build/` directory. Verify
those separately (e.g. a standalone syntax check against the real header
tree, or a full example/sketch compile).

## patches/

This directory used to hold `libespeak.patch` and `speak_lib.patch`: a
captured diff between pristine upstream espeak-ng source and this repo's
Arduino-adapted `src/libespeak-ng/*.c`, meant as a re-vendoring aid when
pulling in a newer upstream release. It was removed since ongoing changes
in this repo (dynamic heap allocation, the central allocation API, PSRAM
support) have diverged far enough from the patch-tracked baseline that the
patch no longer reflected reality.
