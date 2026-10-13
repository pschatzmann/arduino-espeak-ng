// Regression test for github.com/pschatzmann/arduino-espeak-ng/issues/36:
// "Setup failed on ESP32-S3 N16R8" -- open() reported garbage file names
// ('`g%<\xef\xbf\xbd%#<') and bogus fileIDs even though the caller-built
// path was correct.
//
// Root cause (in the arduino-posix-fs dependency, not this repo): callers
// like LoadVoice() (voices.c) build the path to open() in a local stack
// buffer that is only valid for the duration of that call. Registry::
// openFile() stored the raw `const char *path` pointer in the RegEntry
// without copying it and without setting file_name_owned. Once the
// caller's stack frame was reused -- which a different compiler/stack
// layout (as on ESP32-S3 vs. ESP32-C6) makes more or less likely to
// clobber immediately -- entry.file_name became a dangling pointer, so
// any later use of it (logging, lookups) read garbage.
//
// This test reproduces the pattern directly: build a path in a stack
// buffer inside a helper function that returns, deliberately clobber the
// stack, then confirm the fileID's file_name is still the real path.
#include "FileSystems.h"
#include "test_util.h"
#include <cstring>

extern "C" {
int open(const char *name, int flags, ...);
int close(int fd);
}

static int open_with_stack_local_path(const char *name) {
  // Mirrors LoadVoice() building "buf" as a local char[] and passing it
  // straight to fopen()/open() -- the buffer does not outlive this call.
  char path[64];
  snprintf(path, sizeof(path), "/mem/data/%s", name);
  return open(path, 0);
}

// Overwrites stack memory that open_with_stack_local_path()'s now-dead
// "path" buffer occupied, the way unrelated calls do on real hardware.
static void __attribute__((noinline)) clobber_stack() {
  volatile char junk[256];
  memset((void *)junk, 0xAA, sizeof(junk));
}

int main() {
  file_systems::FileSystemMemory fsm("/mem");
  const unsigned char data[] = {1, 2, 3, 4};
  fsm.add("/mem/data/lang/en", data, sizeof(data));

  int fd = open_with_stack_local_path("lang/en");
  CHECK(fd >= 0, "open() should succeed for a file added to FileSystemMemory");

  clobber_stack();
  clobber_stack();

  file_systems::RegEntry &entry =
      file_systems::Registry::DefaultRegistry().getEntry(fd);
  CHECK(entry.file_name != nullptr, "file_name should not be null");
  CHECK(entry.file_name != nullptr &&
            strcmp(entry.file_name, "/mem/data/lang/en") == 0,
        "file_name should still be the real path, not stack garbage, after "
        "the caller's stack frame is reused");

  if (fd >= 0) close(fd);

  TEST_EXIT();
}
