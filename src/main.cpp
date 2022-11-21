#include <Arduino.h>
#include <ESP8266WiFi.h>


void setup() {
  Serial.begin(115200);
  Serial.println("Started");
  WiFi.mode(WIFI_STA);
  WiFi.begin("Kaernter Aussenposten", "EcExLvwd");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("WiFI Connected");
}

void loop() {
  // put your main code here, to run repeatedly:
}