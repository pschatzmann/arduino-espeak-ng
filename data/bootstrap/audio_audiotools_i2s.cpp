#include "audio.h"
#include <stdio.h>
#include "AudioTools.h"

/**
 * @brief pcaudiolib i2s implementation for microcontrollers
 * using the arduino audiotools library
 * 
 * @author pschatzmann
 */

struct audio_object{
    enum audio_object_format format;
    uint32_t rate;
    uint8_t channels;
    const char *device;
    const char *application_name;
    I2SStream i2s; // e.g. pointer to stream

    int open(enum audio_object_format format,uint32_t rate, uint8_t channels){
        this->format = format;
        this->rate = rate;
        this->channels = channels;
        auto cfg = i2s.defaultConfig();
        cfg.channels = channels;
        cfg.sample_rate = rate;
        i2s.begin(cfg);
        return 0;
    }

    void close(){
        i2s.end();
    }

    int write(const void *data, size_t bytes){
        i2s.write((uint8_t*)data, bytes);
        return 0;
    }

    int drain(){
        return 0;
    }

    int flush(){    
        return 0;
    }

    const char * strerror(int error){
        static char errormsg[50] = {0};
        sprintf(errormsg, "Error %d", error);
        return (const char*)errormsg;
    }
};

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
