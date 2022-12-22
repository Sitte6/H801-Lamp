#include <httpupdate.h>
#include <SoftwareSerial.h>



BearSSL::CertStore certStore;

BearSSL::WiFiClientSecure client;
SoftwareSerial espserial2(1, 2); // RX | TX


const char* timeserver1 = "at.pool.ntp.org";
const char* timeserver2 = "time.nist.gov";

const char* gitserver = "https://raw.githubusercontent.com";
const char* gitfilepath = "https://raw.githubusercontent.com/Sitte6/H801-Lamp/main/firmware/firmware.bin";


void update_started() {
  espserial2.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  espserial2.println("CALLBACK:  HTTP update process finished");
  digitalWrite(13, HIGH);
  delay(1000);
}

void update_progress(int cur, int total) {
  espserial2.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  espserial2.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
  digitalWrite(15, HIGH);
  delay(1000);
}


// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(1, 0, timeserver1, timeserver2);  // UTC

  espserial2.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    espserial2.print(F("."));
    now = time(nullptr);
  }

  espserial2.println(F(""));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  espserial2.print(F("Current time: "));
  espserial2.print(asctime(&timeinfo));
}

void updateFromGitHub()
{
  espserial2.begin(57600);
    LittleFS.begin();
    int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    espserial2.print(F("Number of CA certs read: "));
    espserial2.println(numCerts);
    if (numCerts == 0) {
        espserial2.println(F("No certs found. Did you run certs-from-mozill.py and upload the LittleFS directory before running?"));
        return;  // Can't connect to anything w/o certs!
    }
    setClock();

    bool mfln = client.probeMaxFragmentLength(gitserver, 443, 1024);  // server must be the same as in ESPhttpUpdate.update()
    espserial2.printf("MFLN supported: %s\n", mfln ? "yes" : "no");
    if (mfln) { client.setBufferSizes(1024, 1024); }
    client.setCertStore(&certStore);

    ESPhttpUpdate.setLedPin(15, HIGH);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
    
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, gitfilepath);


    switch (ret) {
      case HTTP_UPDATE_FAILED:
        espserial2.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        espserial2.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        espserial2.println("HTTP_UPDATE_OK");
        break;
    }
}
