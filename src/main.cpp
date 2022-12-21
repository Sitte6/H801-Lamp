#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <httpupdate.h>
#include <MultiLED.h>
#include <Config.h>
#include <Webserver.h>
#include <MultiPurposeButton.h>
#include <SoftwareSerial.h>

#define RED 15
#define BLUE 12
#define GREEN 13
#define WHITELED 14
//#define BUTTON 2
//#define INKLEFT 5
//#define INKRIGHT 0

//H801
#define BUTTON 0
#define INKLEFT 2
#define INKRIGHT 3

#define FLASH_SAVE_INTERVAL 60000

SoftwareSerial espserial(1, 0); // RX | TX

#define WHITE 0
#define RGB 1
#define FADE 2


MultiLED led;
configData cfg;
MultiPurposeButton button(BUTTON);

bool ConnectToWiFi(char ssid[], char pwd[]);
void WiFiStarter();
IRAM_ATTR void left();
//IRAM_ATTR void right();
void p();
void lp();
void SetLEDToMode();

int inclock = 0;
int mode = WHITE;
int colortemperature = 5000;
float brightness = 1;
int color = 0;

bool dim = 0;
int bootmenu = 0;


unsigned long lastflashsave = 0;

void setup() {

  led.AddChannel(0, new LedChannel(RED, (LedColor)red, false));
  led.AddChannel(1, new LedChannel(GREEN, (LedColor)green, false));
  led.AddChannel(2, new LedChannel(BLUE, (LedColor)blue, false));



  loadConfig(&cfg);

  unsigned int checksum = CalculateChecksum(cfg);
  if (checksum == cfg.checksum && checksum != 0)
  {
    SetLEDToMode();
  }  

  espserial.begin(57600);


  pinMode(INKRIGHT, INPUT_PULLUP);
  pinMode(INKLEFT, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(WHITELED, OUTPUT);
  digitalWrite(WHITELED, LOW);

  button.RegisterCallbacks(p,lp);
  
  
  espserial.println("Started");
  //WiFi.mode(WIFI_STA);
  //WiFi.begin("Kaernter Aussenposten", "EcExLvwd");
  //while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //  espserial.println("Connection Failed! Rebooting...");
  //  delay(5000);
  //  ESP.restart();
  //}
  WiFiStarter();

  espserial.println("WiFI Connected");
  
}



void loop() {
  led.handle();
  button.handle();
  int A = digitalRead(INKLEFT);
  int B = digitalRead(INKRIGHT);
  if(A==LOW && B==HIGH && inclock)
  {
    espserial.println("left");
    if(dim)
    {
      cfg.brightness -= 0.1;
      cfg.configchanged = true;
      if(cfg.brightness < 0.1)
      {
        cfg.brightness = 0.1;
      }
      SetLEDToMode();
    }
    else if(bootmenu)
    {
      bootmenu = 1;
      led.SetBlue();
    }
    else if(cfg.mode == WHITE)
    {
      cfg.colortemperature -= 500;
      cfg.configchanged = true;
      if(cfg.colortemperature < 1000)
      {
        cfg.colortemperature = 1000;
      }
      SetLEDToMode();
    }
    else if(cfg.mode == RGB)
    {
      cfg.color -= 1;
      cfg.configchanged = true;
      if(cfg.color < 0)
      {
        cfg.color = 11;
      }
      SetLEDToMode();
    }
    
    inclock = 0;
  }
  else if(A==HIGH && B == LOW && inclock)
  {
    espserial.println("right");
    if(dim)
    {
      cfg.brightness += 0.1;
      cfg.configchanged = true;
      if(cfg.brightness > 1)
      {
        cfg.brightness = 1;
      }
      SetLEDToMode();
    }
    else if(bootmenu)
    {
      bootmenu = 2;
      led.SetRed();
    }
    else if(cfg.mode == WHITE)
    {
      cfg.colortemperature += 500;
      cfg.configchanged = true;
      if(cfg.colortemperature > 10000)
      {
        cfg.colortemperature = 10000;
      }
      SetLEDToMode();
    }
    else if(cfg.mode == RGB)
    {
      cfg.color += 1;
      cfg.configchanged = true;
      if(cfg.color > 11)
      {
        cfg.color = 0;
      }
      SetLEDToMode();
    }
    inclock = 0;
  }
  else if(A==HIGH && B==HIGH)
  {
    inclock = 1;
  }

  if(millis() - lastflashsave > FLASH_SAVE_INTERVAL)
  {
    if(cfg.configchanged)
    {
      cfg.configchanged = false;
      saveConfig(cfg);
    }  
    lastflashsave = millis();
  }
  delay(1);
}


void WiFiStarter()
  {
    unsigned int checksum = CalculateChecksum(cfg);
    if (checksum == cfg.checksum && checksum != 0)
    {
      if (ConnectToWiFi(cfg.SSID, cfg.password))
      {
        return;
      }
      /*else  // No blocking of lamp if WiFi is not found
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
      }*/
    }
    else
    {
      led.SetBlue();
      WebserverSetup();
      while (true)
      {
        handleClients();
      }
    }
  }

  bool ConnectToWiFi(char ssid[], char pwd[])
  {
    espserial.println("Connecting to Wifi...");
    WiFi.softAPdisconnect (true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    WiFi.begin(ssid, pwd);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      espserial.println("Connection to Wifi failed.");
      return false;
    }
    else
    {
      espserial.println("Connection to Wifi succesfull.");
      return true;
    }
  }

  IRAM_ATTR void left()
{
  if(digitalRead(0) == LOW)
  {
    espserial.println("left");
    led.SetBlue();
  }
  else
  {
    espserial.println("right");
    led.SetGreen();
  }
  
}
//IRAM_ATTR void right()
//{
//  espserial.println("right");
//  led.SetGreen();
//}

void p()
{
  espserial.println("Press");
  if(dim)
  {
    led.Pulse(Color::Blue, 250);
    dim = false;
    return;
  }
  else if(bootmenu)
  {
    if(bootmenu == 1)
    {
      led.Pulse(Color::Green, 1000);
      led.SetOff();
      espserial.println("Firmware-Update from Github");
      updateFromGitHub();
    }
    if(bootmenu == 2)
    {
      led.Pulse(Color::Red, 1000);
      espserial.println("Factory-Reset");
      eraseConfig(cfg);
      loadConfig(&cfg);
      cfg.mode = 0;
      cfg.brightness = 1;
      cfg.color = 0;
      cfg.colortemperature = 4000;
      cfg.checksum = 0;
      saveConfig(cfg);
      ESP.reset();
    }
    bootmenu = 0;
  }
  else if(++cfg.mode >= 3)
  {
    cfg.mode = 0;
  }
  cfg.configchanged = true;
  espserial.println(cfg.mode);
  SetLEDToMode();
}


void lp()
{
  if(millis()<8000)
  {
    bootmenu = 1;
    espserial.println("Boot Menü");
    led.Pulse(Color::Red, 500, false);
    led.Pulse(Color::Red, 500, false);
    led.Pulse(Color::Red, 500, false);
    led.SetBlue();
  }
  else
  {
    espserial.println("Long Press");
    led.Pulse(Color::Blue, 250);
    dim = 1;
  }
  
}
void SetLEDToMode()
{
  espserial.println("test");
  espserial.println(cfg.brightness);
  espserial.println(cfg.colortemperature);
  espserial.println(cfg.color);
  led.SetBrightness(cfg.brightness);
  led.updatestate();
  switch (cfg.mode)
  {
  case WHITE:
    led.FadeToValueAsync(KelvinRGB::KelvinToRGB(cfg.colortemperature), 250);
    break;
  case RGB:
    led.FadeToValueAsync(ColorThemes::ColorCircle[cfg.color], 250);
    break;
  case FADE:
    led.StartRandomColorFadeFromTheme(ColorThemes::Rainbow,9000);
    break;

  default: led.SetBlue();
    break;
  }
  led.handle();
}