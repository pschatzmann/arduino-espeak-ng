#pragma once

/// Callback to define the AudioStream
extern AudioStream* (*audio_stream_factory_callback)(AudioBaseInfo &cfg);

/**
 * @brief pcaudiolib p_out_stream implementation for microcontrollers
 * using the arduino audiotools library
 * 
 * @author pschatzmann
 */

struct audio_object {
    enum audio_object_format format;
    uint32_t rate;
    uint8_t channels;
    const char *device;
    const char *application_name;
    AudioStream *p_out_stream = nullptr; // e.g. pointer to stream

    int open(enum audio_object_format format,uint32_t rate, uint8_t channels){
        this->format = format;
        this->rate = rate;
        this->channels = channels;

        AudioBaseInfo cfg;
        cfg.channels = channels;
        cfg.sample_rate = rate;
        cfg.bits_per_sample = 16;

        // setup default i2s stream if nothing has been defined
        if (p_out_stream==nullptr){
            if (audio_stream_factory_callback==nullptr){
                I2SStream *p_i2s = new I2SStream();
                p_out_stream = p_i2s;
                auto c = p_i2s->defaultConfig();
                c.copyFrom(cfg);
                p_i2s->begin(c);
            } else {
                p_out_stream = audio_stream_factory_callback(cfg);
            }
        }
        return p_out_stream!=nullptr ? 0 : -1;
    }

    void close(){
        p_out_stream->end();
    }

    int write(const void *data, size_t bytes){
        p_out_stream->write((uint8_t*)data, bytes);
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