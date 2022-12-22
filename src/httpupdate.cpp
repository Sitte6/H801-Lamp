#include <httpupdate.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecure.h>


WiFiClientSecure client;
//SoftwareSerial espserial(1, 2); // RX | TX



const char* gitserver = "https://raw.githubusercontent.com";
const char* gitfilepath = "https://raw.githubusercontent.com/Sitte6/H801-Lamp/main/firmware/firmware.bin";


void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
  digitalWrite(13, HIGH);
  delay(1000);
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
  digitalWrite(15, HIGH);
  delay(1000);
}



void updateFromGitHub()
{
    Serial.begin(57600);
    client.setInsecure();
    ESPhttpUpdate.setLedPin(15, HIGH);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
    
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, gitfilepath);


    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
}
