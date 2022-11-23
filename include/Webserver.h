#ifndef H_WEBSERVER
#define H_WEBSERVER

#include <ESP8266WebServer.h>
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include <DNSServer.h>
#include "Config.h"
//#include "Globals.h"
//#include "LED.h"
//#include "main.h"
//#include "Flash.h"

void WebserverSetup();


void handleRoot();

void handleCredentials();

void handleNotFound();

void handleClients();

#endif