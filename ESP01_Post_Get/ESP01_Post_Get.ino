/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

const char* ssid = "A52s wifi";
const char* password = "dongiliwifi";

//Your Domain name with URL path or IP address with path
const char* serverName1 = "http://192.168.78.205:3000/addSensorsData";
const char* serverName2 = "http://192.168.78.205:3000/getPumpState";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

String sensorReadings;
float sensorReadingsArr[3];
String httpGETRequest(const char* serverName);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  //Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  /* Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); */
 
  //Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {

    Serial.write(99); // c
    Serial.write(100); // d
    Serial.write(101); // e
    Serial.write(102); // f
    Serial.write(103); // g
    Serial.write(104); // h
    Serial.write(105); // i
    Serial.write(106); // j
    
    //Serial.write("\n");
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName1);
  
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      // ------------------------- POST METHOD ------------------------
      // Specify content-type header
      // Data to send with HTTP POST
      // Send HTTP POST request

      // Specify content-type header
      // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // Send HTTP POST request
      // int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"soil_humidity\":\"123.345\",\"brightness\":\"123.123\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      /* Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode); */

      // -------------------------- GET METHOD ----------------------------
      // Send an HTTP POST request depending on timerDelay
      
              
      sensorReadings = httpGETRequest(serverName2);
      //Serial.println(sensorReadings);
/*       JSONVar myObject = JSON.parse(sensorReadings);
 */
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      /* if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      } */
    
      /* Serial.print("JSON object = ");
      Serial.println(myObject); */
    
      // myObject.keys() can be used to get an array of all the keys in the object
      /* JSONVar keys = myObject.keys();
    
      for (int i = 0; i < keys.length(); i++) {
        JSONVar value = myObject[keys[i]];
        Serial.print(keys[i]);
        Serial.print(" = ");
        Serial.println(value);
        sensorReadingsArr[i] = double(value);
      }
      Serial.print("1 = ");
      Serial.println(sensorReadingsArr[0]);
      Serial.print("2 = ");
      Serial.println(sensorReadingsArr[1]);
      Serial.print("3 = ");
      Serial.println(sensorReadingsArr[2]); */
    }
    else {
      //Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    /* Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode); */
    payload = http.getString();
  }
  else {
    /* Serial.print("Error code: ");
    Serial.println(httpResponseCode); */
  }
  // Free resources
  http.end();

  return payload;
}