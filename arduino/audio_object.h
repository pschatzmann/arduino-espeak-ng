#pragma once
#include <stdio.h> // for snprintf
#include "Print.h"
#include "audio.h"

/**
 * @brief info about sample rate and channels and output stream
 */
struct audio_info {
    int sample_rate = 22050;    // undefined
    int channels = 1;       // undefined
    int bits_per_sample=16; // we assume int16_t
    Print *out=nullptr;
};
extern audio_info espeak_audio_info;
extern Print *audio_out;

/// Callback to define the AudioStream
extern void(*audio_stream_factory_callback)(audio_info *cfg);
extern void(*audio_stream_start_callback)();
extern void(*audio_stream_stop_callback)();

/// Defines the audio output
extern void espeak_set_audio_output(Print *p);
/// Provides the audio information
extern audio_info espeak_get_audio_info();

/**
 * @brief pcaudiolib implementation for microcontrollers
 * using the arduino Print assigned by espeak_set_audio_output() as output
 * 
 * @author pschatzmann
 */
struct audio_object {
    enum audio_object_format format;
    uint32_t rate;
    uint8_t channels;
    const char *device;
    const char *application_name;
    Print *p_out_stream = nullptr; // e.g. pointer to stream
    audio_info cfg;
    bool active = false;

    int open(enum audio_object_format format,uint32_t rate, uint8_t channels){
        ESPK_LOG("audio_object::open()\n");
        this->format = format;
        this->rate = rate;
        this->channels = channels;
        
        // update audio information
        espeak_audio_info.channels = channels;
        espeak_audio_info.sample_rate = rate;
        espeak_audio_info.bits_per_sample = 16;
        espeak_audio_info.out = p_out_stream;
        p_out_stream = audio_out;

        // callback with audio information
        if (audio_stream_factory_callback!=nullptr){
            audio_stream_factory_callback(&espeak_audio_info);
            if (espeak_audio_info.out!=p_out_stream && espeak_audio_info.out!=nullptr){
                p_out_stream = espeak_audio_info.out;
            }
        }
        if (audio_stream_start_callback!=nullptr){
            audio_stream_start_callback();
        }
        return p_out_stream!=nullptr ? 0 : -1;
    }

    void close(){
        ESPK_LOG("audio_object::close()\n");
        if (audio_stream_stop_callback!=nullptr){
            audio_stream_stop_callback();
        }
        active = false;
    }

    int write(const void *data, size_t bytes){
        if (!active && p_out_stream!=nullptr){
            if (audio_stream_start_callback!=nullptr){
                ESPK_LOG("audio_object::audio_stream_start_callback\n");
                audio_stream_start_callback();
            }
            active = true;
        }
        if (p_out_stream==nullptr) return -1;
        ESPK_LOG("audio_object::write(%d)\n", (int) bytes);
        // write in batches of 512 bytes
        int open = bytes;
        int offset = 0;
        while(open>0){
            int to_write = open>1024? 1024 : open;
            int result = p_out_stream->write(((uint8_t*)data)+offset, to_write);
            offset+=to_write;
            open-=to_write;
        }
        return 0;
    }

    int drain(){
        if (audio_stream_stop_callback!=nullptr){
            ESPK_LOG("audio_object::audio_stream_stop_callback\n");
            audio_stream_stop_callback();
            active = false;
        } else {
            // prevent some ending noise on the ESP32
            uint8_t none[1024]={0};
            for (int j=0;j<10;j++){
                p_out_stream->write(none, 1024);
            }
        }
        return 0;
    }

    int flush(){    
        return 0;
    }

    const char * strerror(int error){
        static char errormsg[50] = {0};
        snprintf(errormsg, 50, "Error %d", error);
        return (const char*)errormsg;

    }
};