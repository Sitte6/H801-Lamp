#include <httpupdate.h>



BearSSL::CertStore certStore;

BearSSL::WiFiClientSecure client;

const char* timeserver1 = "at.pool.ntp.org";
const char* timeserver2 = "time.nist.gov";

const char* gitserver = "https://raw.githubusercontent.com";
const char* gitfilepath = "https://raw.githubusercontent.com/Sitte6/H801-Lamp/main/firmware/firmware.bin";


void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}


// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(1, 0, timeserver1, timeserver2);  // UTC

  Serial.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }

  Serial.println(F(""));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void updateFromGitHub()
{
    LittleFS.begin();
    int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    Serial.print(F("Number of CA certs read: "));
    Serial.println(numCerts);
    if (numCerts == 0) {
        Serial.println(F("No certs found. Did you run certs-from-mozill.py and upload the LittleFS directory before running?"));
        return;  // Can't connect to anything w/o certs!
    }
    setClock();

    bool mfln = client.probeMaxFragmentLength(gitserver, 443, 1024);  // server must be the same as in ESPhttpUpdate.update()
    Serial.printf("MFLN supported: %s\n", mfln ? "yes" : "no");
    if (mfln) { client.setBufferSizes(1024, 1024); }
    client.setCertStore(&certStore);

    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

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
