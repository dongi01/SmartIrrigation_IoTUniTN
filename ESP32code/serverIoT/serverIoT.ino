#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer server(8080);

const int LED = 13;

void setup() {
  pinMode(LED, OUTPUT);
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
  digitalWrite(LED, HIGH);
  Serial.println("pompa accesa");
  // send message via serial comunication
  Serial.print(255) // 1111 1111
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  digitalWrite(LED, LOW);
  Serial.println("pompa spenta");
  // send message via serial comunication
  Serial.println(0); // 0000 0000
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "ERROR: Not Found");
}
