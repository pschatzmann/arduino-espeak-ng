#include "SPIFFS.h"
 
void setup() {
  Serial.begin(115200);
  
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){ 
      Serial.print("FILE: ");
      Serial.println(file.name());
      file = root.openNextFile();
  }

  file = SPIFFS.open("/espeak-ng-data/voices/!v/adam");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  Serial.println("File Content:");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

void loop() {

}
