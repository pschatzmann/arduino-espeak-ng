#pragma once
#ifdef __cplusplus
extern "C" {
#endif
// Custom memory map to access progmem
void* espeak_mem_map(const char* path, int* len); 
// prevent error msg for close
int close(int fd);

#ifdef __cplusplus
}
#endif
