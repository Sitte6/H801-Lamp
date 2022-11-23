#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <httpupdate.h>
#include <MultiLED.h>
#include <Config.h>
#include <Webserver.h>
#include <MultiPurposeButton.h>

#define RED 15
#define BLUE 13
#define GREEN 12



MultiLED led;
configData cfg;
MultiPurposeButton button(2);

bool ConnectToWiFi(char ssid[], char pwd[]);
void WiFiStarter();
IRAM_ATTR void left();
IRAM_ATTR void right();
void p();
void lp();


void setup() {
  Serial.begin(115200);

  pinMode(2, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(5), left, FALLING);
  attachInterrupt(digitalPinToInterrupt(0), right, FALLING);

  button.RegisterCallbacks(p,lp);

  led.AddChannel(0, new LedChannel(RED, (LedColor)red, false));
  led.AddChannel(1, new LedChannel(GREEN, (LedColor)green, false));
  led.AddChannel(2, new LedChannel(BLUE, (LedColor)blue, false));

  
  Serial.println("Started");
  //WiFi.mode(WIFI_STA);
  //WiFi.begin("Kaernter Aussenposten", "EcExLvwd");
  //while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //  Serial.println("Connection Failed! Rebooting...");
  //  delay(5000);
  //  ESP.restart();
  //}
  WiFiStarter();

  Serial.println("WiFI Connected");

  led.StartRandomColorFadeFromTheme(ColorThemes::Rainbow, 3000, logistic, true);


}



void loop() {
  /*if(digitalRead(2)==LOW)
  {
    while(digitalRead(2)==LOW)
    {
      delay(50);
    }
    Serial.println("Update...");
    updateFromGitHub();
    
  }*/
  led.handle();
  button.handle();
  delay(10);
}


void WiFiStarter()
  {

    loadConfig(&cfg);

    unsigned int checksum = CalculateChecksum(cfg);
    if (checksum == cfg.checksum && checksum != 0)
    {
      if (ConnectToWiFi(cfg.SSID, cfg.password))
      {
        return;
      }
      else
      {
        delay(10000);
        while (true)
        {
          if (ConnectToWiFi(cfg.SSID, cfg.password))
          {
            return;
          }
          else
          {
            WiFi.mode( WIFI_OFF );
            WiFi.forceSleepBegin();
            delay(1800000);
          }
        }
      }
    }
    else
    {
      WebserverSetup();
      while (true)
      {
        handleClients();
      }
    }
  }

  bool ConnectToWiFi(char ssid[], char pwd[])
  {
    Serial.println("Connecting to Wifi...");
    WiFi.softAPdisconnect (true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    WiFi.begin(ssid, pwd);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection to Wifi failed.");
      return false;
    }
    else
    {
      Serial.println("Connection to Wifi succesfull.");
      return true;
    }
  }

  IRAM_ATTR void left()
{
  Serial.println("left");
  led.SetRed();
}
IRAM_ATTR void right()
{
  Serial.println("right");
  led.SetGreen();
}

void p()
{
  led.SetRed();
}


void lp()
{
  if(millis()<15000)
  {
    led.SetBlue();
    updateFromGitHub();
  }
  
}