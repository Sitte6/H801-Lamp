#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>


WiFiClient client;

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(4, OUTPUT);
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
  if(digitalRead(2)==LOW)
  {
    while(digitalRead(2)==LOW)
    {
      delay(50);
    }
    Serial.println("Update...");
    ESPhttpUpdate.update(client, "https://github.com/Sitte6/H801-Lamp/firmware", 80, "/firmware.bin");
  }
  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(4, LOW);
  delay(500);

}