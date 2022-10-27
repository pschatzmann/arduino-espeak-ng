#include "audio.h"
#include <stdio.h>
#include "audio_object.h"

AudioStream* (*audio_stream_factory_callback)(AudioBaseInfo &cfg) = nullptr;

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
