#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer server(8080);

const int led = 13;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  WiFiManager wm;

  // wm.resetSettings();

  bool res;
  res = wm.autoConnect("AutoConnectAP"); // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }


  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/StartPump", handleStart);
  server.on("/StopPump", handleStop);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}

void handleStart() {
  digitalWrite(led, 1);
  delay(2000);
  Serial.println("pompa accesa");
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  server.send(200, "text/plain", "OK");
  delay(2000);
  Serial.println("pompa spenta");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  server.send(404, "text/plain", "ERROR: Not Found");
  digitalWrite(led, 0);
}
