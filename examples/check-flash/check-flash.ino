#include "espeak.h"

void setup() {
  Serial.begin(115200);
  while(!Serial);
}

void check(const char* path, const uint8_t* data1, size_t len){
    Serial.print("checking ");
    Serial.print(path);
    Serial.print(": ");
    // check byte access
    int total=0;
    for (size_t j=0;j<len;j++){
        total = data1[j];
    }
    Serial.print("+");
    // check access via int
    int *p_int = (int*)data1;
    int end = len/sizeof(int);
    for (size_t j=0;j<end;j++){
        total = p_int[j];
    }
    Serial.println(" +");
}


void loop() {
    check("/mem/data/phontab", espeak_ng_data_phontab,espeak_ng_data_phontab_len);
    check("/mem/data/phonindex", espeak_ng_data_phonindex,espeak_ng_data_phonindex_len);
    check("/mem/data/phondata", espeak_ng_data_phondata,espeak_ng_data_phondata_len);
    check("/mem/data/intonations", espeak_ng_data_intonations,espeak_ng_data_intonations_len);
    check("/mem/data/en_dict", espeak_ng_data_en_dict,espeak_ng_data_en_dict_len);
    check("/mem/data/lang/en", espeak_ng_data_lang_gmw_en, espeak_ng_data_lang_gmw_en_len);
    Serial.println();
}
