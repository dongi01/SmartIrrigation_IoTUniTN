#include "httpClient.h"
#include <HTTPClient.h>
#include <string.h>

// node server address
String serverNode = "http://192.168.29.205:3000";

// send sensors data to node server
void sendData(const int &dataMoisture, const int &dataLight, const int &dataTemp) {

  HTTPClient http;

  // open a HTTP connection between this board and node server
  String serverPath = serverNode + "/addSensorsData";
  http.begin(serverPath.c_str());

  // add HTTP POST header
  http.addHeader("Content-Type", "application/json");

  // create json string
  String data_json = "{\"soil_moisture\":\"" + (String)dataMoisture + "\",\"brightness\":\"" + (String)dataLight + "\",\"temperature\":\"" + (String)dataTemp + "\"}";

  // send HTTP POST request
  int httpResponseCode = http.POST(data_json);

  // close HTTP connection
  http.end();

}

// send HTTP message to node server if pump state change
void sendStartPump() {

  HTTPClient http;

  // open a HTTP connection between this board and node server
  String serverPath = serverNode + "/alertStartPump";
  http.begin(serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  // close HTTP connection
  http.end();
}

void sendStopPump() {

  HTTPClient http;

  // open a HTTP connection between this board and ndoe server
  String serverPath = serverNode + "/alertStopPump";
  http.begin(serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  // close HTTP connection
  http.end();
}
