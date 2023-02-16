#include "httpServer.h"
#include <WebServer.h>

// defining this board as a server for listen to StartPump and StopPump request from node server (or web browser)
WebServer server(8080);

// start server
void startServer(){
  server.begin();
}

// handle client
void handleClientHTTP(){
  server.handleClient();  
}

// defining handler functions for HTTP requests
void defineHandlers(){
  server.on("/StartPump", handleStart);
  server.on("/StopPump", handleStop);
  server.onNotFound(handleNotFound);
}

// --- HTTP request handlers ---

void handleStart() {
  Serial.println("pump started");
  // send message via serial comunication
  Serial2.write(240); // value to indicate to start pump
  Serial.println(240); // debug
  Serial.println("");
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  Serial.println("pump stopped");
  // send message via serial comunication
  Serial2.write(10); // value to indicate to stop pump
  Serial.println(10); // debug
  Serial.println("");
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "ERROR: Not Found");
}

// -----------------------------
