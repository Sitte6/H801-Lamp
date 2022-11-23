#ifndef H_UPDATE
#define H_UPDATE

#include <Arduino.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>
#include <LittleFS.h>





void setClock();

void updateFromGitHub();

//Callbacks
void update_started();

void update_finished();

void update_progress(int cur, int total);

void update_error(int err);


#endif