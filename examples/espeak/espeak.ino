    #include <espeak_lib.h>
    
    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCH_PLAYBACK;
    char *path = NULL;
    void* user_data;
    unsigned int *identifier;
    
    void setup() {
      Serial.begin(115200);

      char text[] = {"Hello world!"};
      int buflength = 500, options = 0;
      unsigned int position = 0, end_position = 0, flags = espeakCHARS_AUTO;
      espeak_POSITION_TYPE position_type=POS_CHARACTER;
      espeak_Initialize(output, buflength, path, options );
      espeak_VOICE voice;
      memset(&voice, 0, sizeof(espeak_VOICE)); // Zero out the voice first
      const char *langNativeString = "en"; // Set voice by properties
      voice.languages = langNativeString;
      voice.name = "US";
      voice.variant = 2;
      voice.gender = 2;
      espeak_SetVoiceByProperties(&voice);
      Serial.println(text);
      espeak_Synth(text, buflength, position, position_type, end_position, flags, identifier, user_data);
      Serial.println("Done");
    }

    void loop(){
    }