#pragma once
#include "ConfigFS.h"

#ifdef __cplusplus
extern "C" {
#endif

void* espeak_mem_map(const char* path, int* len); 

#ifdef __cplusplus
}
#endif
