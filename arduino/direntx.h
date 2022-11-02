
#pragma once
#if defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_RP2040)
// make dirent available
#  include "ConfigFS.h" // https://github.com/pschatzmann/arduino-posix-fs
#else
#  include <dirent.h>
#endif
