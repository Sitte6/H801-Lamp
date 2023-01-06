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

SoftwareSerial espserial(1, 0); // RX | TX =2

#define WHITE 0
#define RGBHUE 1
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
int fadespeed = 9000;

bool dim = 0;
int bootmenu = 0;


unsigned long lastflashsave = 0;

void setup() {

  led.AddChannel(0, new LedChannel(RED, (LedColor)red, false));
  led.AddChannel(1, new LedChannel(GREEN, (LedColor)green, false, 100));
  led.AddChannel(2, new LedChannel(BLUE, (LedColor)blue, false, 100));


  pinMode(WHITELED, OUTPUT);
 /*digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);
  digitalWrite(WHITELED, HIGH);
  delay(6000);*/

  loadConfig(&cfg);

  unsigned int checksum = CalculateChecksum(cfg);
  if (checksum == cfg.checksum && checksum != 0)
  {
    led.SetBrightness(cfg.brightness);
    led.updatestate();
    switch (cfg.mode)
    {
    case WHITE:
      led.FadeToValue(KelvinRGB::KelvinToRGB(cfg.colortemperature), 500);
      analogWrite(WHITELED, 1023*cfg.brightness);
      break;
    case RGBHUE:
      led.FadeToValue(ColorThemes::ColorCircle[cfg.color], 500);
      digitalWrite(WHITELED, LOW);
      break;
    case FADE:
      led.StartRandomColorFadeFromTheme(ColorThemes::Rainbow, cfg.fadespeed);
      digitalWrite(WHITELED, LOW);
      break;

    default: led.SetBlue();
      break;
    }
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
  RGB val = led.GetCurrentValue();
  espserial.printf("R: %d, G: %d, B: %d \n", val.r, val.g, val.b);
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
      cfg.colortemperature -= 250;
      cfg.configchanged = true;
      if(cfg.colortemperature < 1000)
      {
        cfg.colortemperature = 1000;
      }
      SetLEDToMode();
    }
    else if(cfg.mode == RGBHUE)
    {
      cfg.color -= 5;
      cfg.configchanged = true;
      if(cfg.color < 0)
      {
        cfg.color = 355;
      }
      SetLEDToMode();
    }
    else if(cfg.mode == FADE)
    {
      cfg.fadespeed -= 1000;
      cfg.configchanged = true;
      if(cfg.fadespeed < 1000)
      {
        cfg.fadespeed = 1000;
      }
      led.ChangeFadeColorDuration(led.fadeSteps, cfg.fadespeed);
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
      cfg.colortemperature += 250;
      cfg.configchanged = true;
      if(cfg.colortemperature > 6500)
      {
        cfg.colortemperature = 6500;
      }
      SetLEDToMode();
    }
    else if(cfg.mode == RGBHUE)
    {
      cfg.color += 5;
      cfg.configchanged = true;
      if(cfg.color > 355)
      {
        cfg.color = 0;
      }
      SetLEDToMode();
    }
    else if(cfg.mode == FADE)
    {
      cfg.fadespeed += 1000;
      cfg.configchanged = true;
      if(cfg.fadespeed > 60000)
      {
        cfg.fadespeed = 60000;
      }
      led.ChangeFadeColorDuration(led.fadeSteps, cfg.fadespeed);
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
  delay(10);
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
      cfg.fadespeed = 10000;
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
    led.SetRGBValue(KelvinRGB::KelvinToRGB(cfg.colortemperature));
    analogWrite(WHITELED, 1023*cfg.brightness);
    break;
  case RGBHUE:
    led.FadeToValueAsync(MultiLED::HSVtoRGB((HSV){cfg.color, 255, 255}), 100);
    digitalWrite(WHITELED, LOW);
    break;
  case FADE:
    led.StartRandomColorFadeFromTheme(ColorThemes::Rainbow, 9000, linear);
    digitalWrite(WHITELED, LOW);
    break;

  default: led.SetBlue();
    break;
  }
  led.handle();
}