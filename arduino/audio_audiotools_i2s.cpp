#include "config-espk.h"
#include "audio.h"
#include "audio_object.h"

#if ESPEAK_ARDUINO_POSIX_FS
#  include "FileSystems.h" // https://github.com/pschatzmann/arduino-posix-fs
#endif

#ifdef ARDUINO_ARCH_MBED
   REDIRECT_STDOUT_TO(Serial)  // MBED  printf(...) to console
#endif


// audio callback
void (*audio_stream_factory_callback)(audio_info *cfg) = nullptr;
void (*audio_stream_start_callback)() = nullptr;
void (*audio_stream_stop_callback)() = nullptr;

// actual audio output
Print *audio_out = nullptr;
// actual audio info
audio_info espeak_audio_info;

void espeak_set_audio_output(Print *p) {
    audio_out = p;
}

audio_info espeak_get_audio_info(){
    return espeak_audio_info;
}

int audio_object_open(audio_object *object,
                  audio_object_format format,
                  uint32_t rate,
                  uint8_t channels){
    return object->open(format, rate, channels);
}

void audio_object_close(audio_object *object){
    return object->close();
}

void audio_object_destroy(audio_object *object){
    delete object;
}

int audio_object_write(audio_object *object,
                   const void *data,
                   size_t bytes){
    return object->write(data, bytes);
}

int audio_object_drain(struct audio_object *object){
    return object->drain();
}

int audio_object_flush(struct audio_object *object){    
    return object->flush();
}

const char * audio_object_strerror(struct audio_object *object, int error){
    return object->strerror(error);
}

struct audio_object * create_audio_device_object(const char *device,
                           const char *application_name,
                           const char *description){
    struct audio_object* result = new audio_object();
    result->device = device;
    result->application_name = application_name;
    return result;
}

// Integration to https://github.com/pschatzmann/arduino-posix-fs.git to disable define ESPEAK_ARDUINO_POSIX_FS 0
void* espeak_mem_map(const char* path, int *len) {
#if ESPEAK_ARDUINO_POSIX_FS
    size_t size;
    auto result = file_systems::Registry::DefaultRegistry().fileSystemByName("FileSystemMemory").mem_map(path, &size);
    if (len!=nullptr){
        *len = size;
    }
    return result;
#else
    return nullptr;
#endif
}

