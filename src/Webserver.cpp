
#include <Webserver.h>
#include "Arduino.h"
#include <Config.h>


ESP8266WebServer server(80);
const int DNS_Port = 53;
IPAddress apIP(192,168,4,1);
DNSServer dnsServer;

configData config;

// HTML web page to handle Wifi-Credential Input
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title> WiFi-Credentials Input </title>
<style>
Body {
  font-family: Calibri, Helvetica, sans-serif;
  background-color: darkgrey;
}
button {
       background-color: #4CAF50;
       width: 100%;
        color: orange;
        padding: 15px;
        margin: 10px 0px;
        border: none;
        cursor: pointer;
         }
 form {
        border: 3px solid #f1f1f1;
    }
 input[type=text], input[type=password] {
        width: 100%;
        margin: 8px 0;
        padding: 12px 20px;
        display: inline-block;
        border: 2px solid green;
        box-sizing: border-box;
    }
 button:hover {
        opacity: 0.7;
    }


 .container {
        padding: 25px;
        background-color: lightblue;
    }
</style>
</head>
<body>
    <center> <h1> Enter WiFi-Credentials </h1> </center>
    <form action= "/wificredentials" method="POST">
        <div class="container">
            <label>SSID : </label>
            <input type="text" placeholder="Enter WiFi-SSID" name="ssid" required>
            <label>Password : </label>
            <input type="password" placeholder="Enter Password" name="password" required>
            <button type="submit">Submit Credentials</button>
        </div>
    </form>
</body>
</html>)rawliteral";


void WebserverSetup() {
  WiFi.softAP("ESP-SETUP");
  WiFi.mode(WIFI_AP);

  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/wificredentials", HTTP_POST, handleCredentials); // Call the 'handleLogin' function when a POST request is made to URI "/wificredentials"
  server.on("/generate_204", handleRoot);

  server.onNotFound(handleRoot);
  //MDNS.begin("esp8266");
  dnsServer.start(DNS_Port, "*", apIP);
  server.begin();
}


void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", index_html);
}

void handleCredentials() {                         // If a POST request is made to URI /login
  if ( ! server.hasArg("ssid") || ! server.hasArg("password")
       || server.arg("ssid") == NULL || server.arg("password") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  else
  {
    char ssid[31];
    char password[31];
    server.arg("ssid").toCharArray(ssid,server.arg("ssid").length()+1);
    server.arg("password").toCharArray(password,server.arg("password").length()+1);
    server.send(200, "text/html", "<h1>Trying to connect to WiFi-Network " + server.arg("ssid") + "...</h1>");
    Serial.println(server.arg("ssid"));
    Serial.println(server.arg("password"));
    Serial.println(ssid);
    Serial.println(password);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed!");
    WebserverSetup();
  }
  else{
      strcpy(config.SSID, ssid);
      strcpy(config.password, password);

      saveConfig(config);
      delay(100);
      ESP.restart();
  }

  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void handleClients()
{
  server.handleClient();                     // Listen for HTTP requests from clients
  dnsServer.processNextRequest();
  delay(10);
  yield();
}